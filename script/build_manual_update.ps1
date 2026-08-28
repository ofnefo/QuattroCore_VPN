[CmdletBinding()]
param(
    [string]$Version = "0.1.1",
    [int]$VersionMajor = 0,
    [int]$VersionMinor = 1,
    [int]$VersionPatch = 1,
    [int]$VersionBuild = 0
)

$ErrorActionPreference = "Stop"
$projectRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot ".."))
$deploymentRoot = Join-Path $projectRoot "deployment\manual-update"
$payloadRoot = Join-Path $deploymentRoot "Quattro"
$runtimeRoot = Join-Path $projectRoot "deployment\windows-amd64"
$archivePath = Join-Path $deploymentRoot "Quattro.zip"
$makensis = "${env:ProgramFiles(x86)}\NSIS\makensis.exe"

if (-not $deploymentRoot.StartsWith($projectRoot, [StringComparison]::OrdinalIgnoreCase)) {
    throw "Unexpected manual-update staging path: $deploymentRoot"
}

if (Test-Path -LiteralPath $deploymentRoot) {
    Remove-Item -LiteralPath $deploymentRoot -Recurse -Force
}
New-Item -ItemType Directory -Path $payloadRoot -Force | Out-Null

$runtimeFiles = @("Quattro.exe", "QuattroCore.exe", "QuattroUpdater.exe", "libcronet.dll")
foreach ($name in $runtimeFiles) {
    $source = Join-Path $runtimeRoot $name
    if (-not (Test-Path -LiteralPath $source)) {
        throw "Missing staged runtime file: $source"
    }
    Copy-Item -LiteralPath $source -Destination (Join-Path $payloadRoot $name)
}

Compress-Archive -LiteralPath $payloadRoot -DestinationPath $archivePath -CompressionLevel Optimal

if (-not (Test-Path -LiteralPath $makensis)) {
    throw "NSIS was not found: $makensis"
}

Push-Location $projectRoot
try {
    & $makensis /NOCD /V3 /INPUTCHARSET UTF8 `
        "/DAPP_VERSION=$Version" `
        "/DAPP_VERSION_MAJOR=$VersionMajor" `
        "/DAPP_VERSION_MINOR=$VersionMinor" `
        "/DAPP_VERSION_PATCH=$VersionPatch" `
        "/DAPP_VERSION_BUILD=$VersionBuild" `
        script\windows_manual_update.nsi
    if ($LASTEXITCODE -ne 0) {
        throw "NSIS failed with exit code $LASTEXITCODE"
    }
}
finally {
    Pop-Location
}

Get-Item -LiteralPath (Join-Path $projectRoot "QuattroUpdate.exe")
