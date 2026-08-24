package main

import (
	"archive/zip"
	"errors"
	"fmt"
	"io"
	"io/fs"
	"log"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"strings"
	"time"
)

const (
	archiveName = "Quattro.zip"
	extractName = "Quattro_update"
	payloadName = "Quattro"
)

func main() {
	if err := run(); err != nil {
		message := fmt.Sprintf("Quattro update failed: %v\n", err)
		log.Print(message)
		_ = os.WriteFile("QuattroUpdateError.log", []byte(message), 0o600)
		os.Exit(1)
	}
}

func run() error {
	workingDir, err := os.Getwd()
	if err != nil {
		return fmt.Errorf("get working directory: %w", err)
	}

	archivePath := filepath.Join(workingDir, archiveName)
	if _, err = os.Stat(archivePath); err != nil {
		return fmt.Errorf("update archive is unavailable: %w", err)
	}

	extractDir := filepath.Join(workingDir, extractName)
	if err = os.RemoveAll(extractDir); err != nil {
		return fmt.Errorf("clean extraction directory: %w", err)
	}
	if err = unzipSafe(archivePath, extractDir); err != nil {
		return err
	}

	payloadDir := filepath.Join(extractDir, payloadName)
	info, err := os.Stat(payloadDir)
	if err != nil || !info.IsDir() {
		return fmt.Errorf("release must contain a top-level %s directory", payloadName)
	}
	if err = installTree(payloadDir, workingDir); err != nil {
		return err
	}

	_ = os.Remove(archivePath)
	_ = os.RemoveAll(extractDir)
	_ = os.Remove("QuattroUpdateError.log")
	return restartQuattro(workingDir)
}

func unzipSafe(archivePath, destination string) error {
	reader, err := zip.OpenReader(archivePath)
	if err != nil {
		return fmt.Errorf("open update archive: %w", err)
	}
	defer reader.Close()

	destination = filepath.Clean(destination)
	prefix := destination + string(os.PathSeparator)
	for _, entry := range reader.File {
		cleanName := filepath.Clean(filepath.FromSlash(entry.Name))
		if cleanName == "." || filepath.IsAbs(cleanName) || cleanName == ".." || strings.HasPrefix(cleanName, ".."+string(os.PathSeparator)) {
			return fmt.Errorf("unsafe archive path %q", entry.Name)
		}
		target := filepath.Join(destination, cleanName)
		if target != destination && !strings.HasPrefix(target, prefix) {
			return fmt.Errorf("archive path escapes destination: %q", entry.Name)
		}
		if entry.FileInfo().IsDir() {
			if err = os.MkdirAll(target, 0o755); err != nil {
				return err
			}
			continue
		}
		if err = os.MkdirAll(filepath.Dir(target), 0o755); err != nil {
			return err
		}
		source, openErr := entry.Open()
		if openErr != nil {
			return openErr
		}
		mode := entry.Mode()
		if mode == 0 {
			mode = 0o644
		}
		output, createErr := os.OpenFile(target, os.O_CREATE|os.O_TRUNC|os.O_WRONLY, mode)
		if createErr != nil {
			source.Close()
			return createErr
		}
		_, copyErr := io.Copy(output, source)
		closeErr := output.Close()
		source.Close()
		if copyErr != nil {
			return copyErr
		}
		if closeErr != nil {
			return closeErr
		}
	}
	return nil
}

func installTree(sourceRoot, destinationRoot string) error {
	return filepath.WalkDir(sourceRoot, func(path string, entry fs.DirEntry, walkErr error) error {
		if walkErr != nil {
			return walkErr
		}
		relative, err := filepath.Rel(sourceRoot, path)
		if err != nil || relative == "." {
			return err
		}
		parts := strings.Split(relative, string(os.PathSeparator))
		if strings.EqualFold(parts[0], "config") {
			if entry.IsDir() {
				return filepath.SkipDir
			}
			return nil
		}

		destination := filepath.Join(destinationRoot, relative)
		if entry.IsDir() {
			return os.MkdirAll(destination, 0o755)
		}
		info, err := entry.Info()
		if err != nil {
			return err
		}
		return replaceFile(path, destination, info.Mode())
	})
}

func replaceFile(source, destination string, mode fs.FileMode) error {
	input, err := os.Open(source)
	if err != nil {
		return err
	}
	defer input.Close()

	if err = os.MkdirAll(filepath.Dir(destination), 0o755); err != nil {
		return err
	}
	temporary := destination + ".quattro-new"
	_ = os.Remove(temporary)
	output, err := os.OpenFile(temporary, os.O_CREATE|os.O_TRUNC|os.O_WRONLY, mode)
	if err != nil {
		return err
	}
	if _, err = io.Copy(output, input); err != nil {
		output.Close()
		return err
	}
	if err = output.Close(); err != nil {
		return err
	}
	if err = os.Chmod(temporary, mode); err != nil {
		return err
	}

	for attempt := 0; attempt < 20; attempt++ {
		_ = os.Remove(destination)
		if err = os.Rename(temporary, destination); err == nil {
			return nil
		}
		time.Sleep(250 * time.Millisecond)
	}
	return fmt.Errorf("replace %s: %w", filepath.Base(destination), err)
}

func restartQuattro(workingDir string) error {
	executable := filepath.Join(workingDir, "Quattro")
	if runtime.GOOS == "windows" {
		executable += ".exe"
	}
	if _, err := os.Stat(executable); err != nil {
		return fmt.Errorf("updated executable is unavailable: %w", err)
	}
	command := exec.Command(executable)
	command.Dir = workingDir
	if err := command.Start(); err != nil {
		return fmt.Errorf("restart Quattro: %w", err)
	}
	if command.Process == nil {
		return errors.New("restart Quattro: process was not created")
	}
	return command.Process.Release()
}
