#pragma once

#include <QWidget>

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;

class QuattroDashboard final : public QWidget {
    Q_OBJECT

public:
    enum class Mode { Tun = 0, SystemProxy = 1 };

    explicit QuattroDashboard(QWidget *parent = nullptr);

    void setConnectionState(bool connected, const QString &status, const QString &server = {});
    void setProfiles(const QList<QPair<int, QString>> &profiles, int selectedId, bool includeAuto = true);
    void setMode(Mode mode);
    void setRussiaBypass(bool enabled);
    void setAutoStart(bool enabled);
    QString subscriptionUrl() const;

signals:
    void connectionToggled(bool connect);
    void subscriptionSubmitted(const QString &url);
    void subscriptionRefreshRequested();
    void modeChanged(QuattroDashboard::Mode mode);
    void profileChanged(int profileId);
    void russiaBypassChanged(bool enabled);
    void autoStartChanged(bool enabled);
    void channelsRequested();
    void routingRequested();
    void settingsRequested();
    void advancedRequested();

private:
    bool m_connected = false;
    QLabel *m_statusDot = nullptr;
    QLabel *m_statusText = nullptr;
    QLabel *m_activeServer = nullptr;
    QPushButton *m_connectButton = nullptr;
    QLineEdit *m_subscription = nullptr;
    QComboBox *m_profiles = nullptr;
    QPushButton *m_tunButton = nullptr;
    QPushButton *m_proxyButton = nullptr;
    QCheckBox *m_russiaBypass = nullptr;
    QCheckBox *m_autoStart = nullptr;

    void applyModeStyle();
};
