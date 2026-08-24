#include "include/ui/widget/QuattroDashboard.hpp"

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPixmap>
#include <QScrollArea>
#include <QSignalBlocker>
#include <QStyle>
#include <QVBoxLayout>

namespace {
QFrame *card(QWidget *parent = nullptr) {
    auto *frame = new QFrame(parent);
    frame->setObjectName(QStringLiteral("card"));
    return frame;
}

QPushButton *secondaryButton(const QString &text, QWidget *parent = nullptr) {
    auto *button = new QPushButton(text, parent);
    button->setProperty("kind", "secondary");
    button->setCursor(Qt::PointingHandCursor);
    return button;
}
}

QuattroDashboard::QuattroDashboard(QWidget *parent) : QWidget(parent) {
    setObjectName(QStringLiteral("quattroDashboard"));
    setMinimumSize(760, 620);

    auto *page = new QWidget(this);
    auto *root = new QVBoxLayout(page);
    root->setContentsMargins(36, 28, 36, 28);
    root->setSpacing(18);

    auto *header = new QHBoxLayout;
    auto *logo = new QLabel(page);
    logo->setPixmap(QPixmap(QStringLiteral(":/Quattro/Quattro.png")).scaled(
        54, 54, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logo->setFixedSize(58, 58);
    header->addWidget(logo);

    auto *brand = new QVBoxLayout;
    auto *title = new QLabel(QStringLiteral("QUATTRO"), page);
    title->setObjectName(QStringLiteral("brandTitle"));
    auto *subtitle = new QLabel(tr("Умное подключение на базе Quattro"), page);
    subtitle->setObjectName(QStringLiteral("muted"));
    brand->addWidget(title);
    brand->addWidget(subtitle);
    header->addLayout(brand);
    header->addStretch();

    auto *statusPill = new QFrame(page);
    statusPill->setObjectName(QStringLiteral("statusPill"));
    auto *statusLayout = new QHBoxLayout(statusPill);
    statusLayout->setContentsMargins(12, 7, 12, 7);
    statusLayout->setSpacing(8);
    m_statusDot = new QLabel(QStringLiteral("●"), statusPill);
    m_statusDot->setObjectName(QStringLiteral("statusDot"));
    m_statusText = new QLabel(tr("Отключено"), statusPill);
    statusLayout->addWidget(m_statusDot);
    statusLayout->addWidget(m_statusText);
    header->addWidget(statusPill);
    root->addLayout(header);

    auto *hero = card(page);
    auto *heroLayout = new QHBoxLayout(hero);
    heroLayout->setContentsMargins(28, 24, 28, 24);
    heroLayout->setSpacing(24);

    auto *heroText = new QVBoxLayout;
    auto *heroTitle = new QLabel(tr("Без лишних настроек"), hero);
    heroTitle->setObjectName(QStringLiteral("sectionTitle"));
    auto *heroHint = new QLabel(tr("Российские сайты идут напрямую. Заблокированные сервисы — через VPN."), hero);
    heroHint->setObjectName(QStringLiteral("muted"));
    heroHint->setWordWrap(true);
    m_activeServer = new QLabel(tr("Сервер: авто, самый быстрый"), hero);
    m_activeServer->setObjectName(QStringLiteral("serverLabel"));
    heroText->addWidget(heroTitle);
    heroText->addWidget(heroHint);
    heroText->addSpacing(10);
    heroText->addWidget(m_activeServer);
    heroText->addStretch();
    heroLayout->addLayout(heroText, 1);

    m_connectButton = new QPushButton(tr("ПОДКЛЮЧИТЬ"), hero);
    m_connectButton->setObjectName(QStringLiteral("connectButton"));
    m_connectButton->setCursor(Qt::PointingHandCursor);
    m_connectButton->setFixedSize(176, 176);
    connect(m_connectButton, &QPushButton::clicked, this, [this] {
        emit connectionToggled(!m_connected);
    });
    heroLayout->addWidget(m_connectButton, 0, Qt::AlignCenter);
    root->addWidget(hero);

    auto *row = new QHBoxLayout;
    row->setSpacing(18);

    auto *connectionCard = card(page);
    auto *connectionLayout = new QVBoxLayout(connectionCard);
    connectionLayout->setContentsMargins(22, 20, 22, 20);
    connectionLayout->setSpacing(12);
    auto *connectionTitle = new QLabel(tr("Подключение"), connectionCard);
    connectionTitle->setObjectName(QStringLiteral("cardTitle"));
    connectionLayout->addWidget(connectionTitle);

    auto *modeRow = new QHBoxLayout;
    m_tunButton = secondaryButton(QStringLiteral("TUN"), connectionCard);
    m_proxyButton = secondaryButton(tr("Системный прокси"), connectionCard);
    m_tunButton->setCheckable(true);
    m_proxyButton->setCheckable(true);
    auto *modeGroup = new QButtonGroup(this);
    modeGroup->setExclusive(true);
    modeGroup->addButton(m_tunButton);
    modeGroup->addButton(m_proxyButton);
    modeRow->addWidget(m_tunButton);
    modeRow->addWidget(m_proxyButton);
    connectionLayout->addLayout(modeRow);
    connect(m_tunButton, &QPushButton::clicked, this, [this] { emit modeChanged(Mode::Tun); });
    connect(m_proxyButton, &QPushButton::clicked, this, [this] { emit modeChanged(Mode::SystemProxy); });

    auto *serverCaption = new QLabel(tr("Сервер"), connectionCard);
    serverCaption->setObjectName(QStringLiteral("fieldLabel"));
    m_profiles = new QComboBox(connectionCard);
    connect(m_profiles, &QComboBox::currentIndexChanged, this, [this](int index) {
        if (index >= 0) emit profileChanged(m_profiles->itemData(index).toInt());
    });
    connectionLayout->addWidget(serverCaption);
    connectionLayout->addWidget(m_profiles);
    row->addWidget(connectionCard, 1);

    auto *routingCard = card(page);
    auto *routingLayout = new QVBoxLayout(routingCard);
    routingLayout->setContentsMargins(22, 20, 22, 20);
    routingLayout->setSpacing(12);
    auto *routingTitle = new QLabel(tr("Маршрутизация"), routingCard);
    routingTitle->setObjectName(QStringLiteral("cardTitle"));
    routingLayout->addWidget(routingTitle);
    m_russiaBypass = new QCheckBox(tr("Russia Bypass"), routingCard);
    m_russiaBypass->setToolTip(tr("Российские домены и IP идут напрямую"));
    m_autoStart = new QCheckBox(tr("Запускать и подключать при входе"), routingCard);
    m_autoStart->setToolTip(tr("Quattro запустится свёрнутым в трей и восстановит выбранный режим и сервер"));
    routingLayout->addWidget(m_russiaBypass);
    routingLayout->addWidget(m_autoStart);
    auto *channels = secondaryButton(tr("Каналы приложений"), routingCard);
    auto *routes = secondaryButton(tr("Правила и исключения"), routingCard);
    routingLayout->addWidget(channels);
    routingLayout->addWidget(routes);
    connect(m_russiaBypass, &QCheckBox::toggled, this, &QuattroDashboard::russiaBypassChanged);
    connect(m_autoStart, &QCheckBox::toggled, this, &QuattroDashboard::autoStartChanged);
    connect(channels, &QPushButton::clicked, this, &QuattroDashboard::channelsRequested);
    connect(routes, &QPushButton::clicked, this, &QuattroDashboard::routingRequested);
    row->addWidget(routingCard, 1);
    root->addLayout(row);

    auto *subCard = card(page);
    auto *subLayout = new QGridLayout(subCard);
    subLayout->setContentsMargins(22, 16, 22, 16);
    subLayout->setSpacing(10);
    auto *subText = new QVBoxLayout;
    auto *subTitle = new QLabel(tr("Подписка Quattro"), subCard);
    subTitle->setObjectName(QStringLiteral("cardTitle"));
    auto *subHint = new QLabel(tr("Вставь ссылку один раз — список серверов будет обновляться автоматически."), subCard);
    subHint->setObjectName(QStringLiteral("muted"));
    subText->addWidget(subTitle);
    subText->addWidget(subHint);
    subLayout->addLayout(subText, 0, 0);
    m_subscription = new QLineEdit(subCard);
    m_subscription->setPlaceholderText(QStringLiteral("https://auth.quattro-cloud.ru/…"));
    m_subscription->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    m_subscription->setMinimumWidth(300);
    subLayout->addWidget(m_subscription, 0, 1);
    m_subscriptionSave = new QPushButton(tr("Добавить"), subCard);
    m_subscriptionSave->setObjectName(QStringLiteral("primaryButton"));
    m_subscriptionRefresh = secondaryButton(tr("Обновить"), subCard);
    subLayout->addWidget(m_subscriptionSave, 0, 2);
    subLayout->addWidget(m_subscriptionRefresh, 0, 3);
    m_subscriptionStatus = new QLabel(subCard);
    m_subscriptionStatus->setObjectName(QStringLiteral("muted"));
    m_subscriptionStatus->setWordWrap(true);
    subLayout->addWidget(m_subscriptionStatus, 1, 1, 1, 3);
    connect(m_subscriptionSave, &QPushButton::clicked, this, [this] {
        emit subscriptionSubmitted(m_subscription->text().trimmed());
    });
    connect(m_subscription, &QLineEdit::returnPressed, m_subscriptionSave, &QPushButton::click);
    connect(m_subscriptionRefresh, &QPushButton::clicked, this, &QuattroDashboard::subscriptionRefreshRequested);
    root->addWidget(subCard);

    auto *footer = new QHBoxLayout;
    auto *engine = new QLabel(tr("Quattro engine • sing-box"), page);
    engine->setObjectName(QStringLiteral("muted"));
    footer->addWidget(engine);
    footer->addStretch();
    auto *settings = secondaryButton(tr("Настройки"), page);
    auto *advanced = secondaryButton(tr("Расширенный режим"), page);
    footer->addWidget(settings);
    footer->addWidget(advanced);
    connect(settings, &QPushButton::clicked, this, &QuattroDashboard::settingsRequested);
    connect(advanced, &QPushButton::clicked, this, &QuattroDashboard::advancedRequested);
    root->addLayout(footer);

    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidget(page);
    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->addWidget(scroll);

    setStyleSheet(QStringLiteral(R"qss(
        QWidget#quattroDashboard, QWidget#quattroDashboard QScrollArea, QWidget#quattroDashboard QScrollArea > QWidget > QWidget {
            background: #f3f4f6; color: #111317; font-family: "Segoe UI"; font-size: 14px;
        }
        QFrame#card { background: #ffffff; border: 1px solid #e5e7eb; border-radius: 18px; }
        QFrame#statusPill { background: #ffffff; border: 1px solid #e5e7eb; border-radius: 15px; }
        QLabel#brandTitle { font-size: 25px; font-weight: 800; letter-spacing: 2px; }
        QLabel#sectionTitle { font-size: 24px; font-weight: 700; }
        QLabel#cardTitle { font-size: 16px; font-weight: 700; }
        QLabel#fieldLabel { font-size: 12px; color: #6b7280; }
        QLabel#muted { color: #6b7280; }
        QLabel#serverLabel { color: #111317; font-weight: 600; }
        QLabel#statusDot { color: #9ca3af; }
        QPushButton { min-height: 38px; border-radius: 10px; padding: 0 14px; }
        QPushButton[kind="secondary"] { background: #f3f4f6; border: 1px solid #e5e7eb; color: #22252a; }
        QPushButton[kind="secondary"]:hover { background: #e9eaed; }
        QPushButton[kind="secondary"]:checked { background: #111317; border-color: #111317; color: #ffffff; }
        QPushButton#primaryButton { background: #ef1717; border: none; color: #ffffff; font-weight: 700; }
        QPushButton#primaryButton:hover { background: #d80e0e; }
        QPushButton#connectButton { background: #111317; border: 10px solid #eef0f2; border-radius: 88px; color: #ffffff; font-size: 15px; font-weight: 800; }
        QPushButton#connectButton:hover { background: #20242a; border-color: #ffe2e2; }
        QPushButton#connectButton[connected="true"] { background: #ef1717; border-color: #ffe2e2; }
        QLineEdit, QComboBox { min-height: 38px; background: #f8f9fa; border: 1px solid #dfe2e6; border-radius: 10px; padding: 0 12px; }
        QComboBox::drop-down { border: none; width: 26px; }
        QCheckBox { min-height: 28px; spacing: 9px; }
    )qss"));

    setMode(Mode::Tun);
    setConnectionState(false, tr("Отключено"));
}

void QuattroDashboard::setConnectionState(bool connected, const QString &status, const QString &server,
                                          bool transitioning) {
    m_connected = connected;
    m_transitioning = transitioning;
    m_statusText->setText(status);
    m_statusDot->setStyleSheet(connected ? QStringLiteral("color:#18a957") : QStringLiteral("color:#9ca3af"));
    m_connectButton->setText(transitioning ? status.toUpper()
                                          : connected ? tr("ОТКЛЮЧИТЬ") : tr("ПОДКЛЮЧИТЬ"));
    m_connectButton->setProperty("connected", connected);
    m_connectButton->style()->unpolish(m_connectButton);
    m_connectButton->style()->polish(m_connectButton);
    m_activeServer->setText(server.isEmpty() ? tr("Сервер: авто, самый быстрый") : tr("Сервер: %1").arg(server));
    updateConnectionButton();
}

void QuattroDashboard::setProfiles(const QList<QPair<int, QString>> &profiles, int selectedId, bool includeAuto) {
    const QSignalBlocker blocker(m_profiles);
    m_profiles->clear();
    m_hasProfiles = !profiles.isEmpty();
    if (!m_hasProfiles) {
        m_profiles->addItem(tr("Сначала добавьте подписку"), -2);
    } else {
        if (includeAuto) m_profiles->addItem(tr("Авто — быстрый, без скачков"), -1);
        for (const auto &[id, name] : profiles) m_profiles->addItem(name, id);
    }
    const int index = m_profiles->findData(selectedId);
    m_profiles->setCurrentIndex(index >= 0 ? index : 0);
    m_profiles->setEnabled(m_hasProfiles);
    updateConnectionButton();
}

void QuattroDashboard::setSelectedProfile(int profileId) {
    const QSignalBlocker blocker(m_profiles);
    const int index = m_profiles->findData(profileId);
    if (index >= 0) m_profiles->setCurrentIndex(index);
}

void QuattroDashboard::setMode(Mode mode) {
    const QSignalBlocker tunBlock(m_tunButton);
    const QSignalBlocker proxyBlock(m_proxyButton);
    m_tunButton->setChecked(mode == Mode::Tun);
    m_proxyButton->setChecked(mode == Mode::SystemProxy);
    applyModeStyle();
}

void QuattroDashboard::setRussiaBypass(bool enabled) {
    const QSignalBlocker blocker(m_russiaBypass);
    m_russiaBypass->setChecked(enabled);
}

void QuattroDashboard::setAutoStart(bool enabled) {
    const QSignalBlocker blocker(m_autoStart);
    m_autoStart->setChecked(enabled);
}

void QuattroDashboard::setSubscriptionState(bool busy, const QString &message) {
    m_subscription->setEnabled(!busy);
    m_subscriptionSave->setEnabled(!busy);
    m_subscriptionRefresh->setEnabled(!busy);
    m_subscriptionSave->setText(busy ? tr("Загрузка…") : tr("Добавить"));
    m_subscriptionStatus->setText(message);
}

QString QuattroDashboard::subscriptionUrl() const { return m_subscription->text().trimmed(); }

void QuattroDashboard::applyModeStyle() {
    for (auto *button : {m_tunButton, m_proxyButton}) {
        button->style()->unpolish(button);
        button->style()->polish(button);
    }
}

void QuattroDashboard::updateConnectionButton() {
    m_connectButton->setEnabled(!m_transitioning && (m_connected || m_hasProfiles));
}
