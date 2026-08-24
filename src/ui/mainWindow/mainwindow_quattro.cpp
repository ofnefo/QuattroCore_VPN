#include "include/ui/mainwindow.h"

#include "include/configs/sub/GroupUpdater.hpp"
#include "include/database/GroupsRepo.h"
#include "include/database/ProfilesRepo.h"
#include "include/database/RoutesRepo.h"
#include "include/database/entities/RouteRule.h"
#include "include/sys/AutoRun.hpp"
#include "include/stats/autoselector/AutoSelectorMonitor.hpp"
#include "include/ui/widget/QuattroDashboard.hpp"

#include <QIcon>
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFrame>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QSizePolicy>
#include <QStackedWidget>
#include <QToolButton>
#include <QUrl>
#include <QVBoxLayout>

void MainWindow::setupQuattroDashboard() {
    if (quattroDashboard != nullptr) return;

    auto *legacyCentralWidget = takeCentralWidget();
    quattroStack = new QStackedWidget(this);
    advancedCentralWidget = new QWidget(quattroStack);
    advancedCentralWidget->setObjectName(QStringLiteral("quattroAdvanced"));
    auto *advancedLayout = new QVBoxLayout(advancedCentralWidget);
    advancedLayout->setContentsMargins(0, 0, 0, 0);
    advancedLayout->setSpacing(0);

    auto *advancedHeader = new QFrame(advancedCentralWidget);
    advancedHeader->setObjectName(QStringLiteral("advancedHeader"));
    auto *advancedHeaderLayout = new QHBoxLayout(advancedHeader);
    advancedHeaderLayout->setContentsMargins(16, 11, 16, 11);
    advancedHeaderLayout->setSpacing(11);
    auto *advancedLogo = new QLabel(advancedHeader);
    advancedLogo->setPixmap(QPixmap(QStringLiteral(":/Quattro/Quattro.png")).scaled(
        32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    advancedLogo->setFixedSize(34, 34);
    advancedHeaderLayout->addWidget(advancedLogo);
    auto *advancedBrand = new QVBoxLayout;
    advancedBrand->setSpacing(0);
    auto *advancedTitle = new QLabel(tr("Quattro Workspace"), advancedHeader);
    advancedTitle->setObjectName(QStringLiteral("advancedTitle"));
    auto *advancedHint = new QLabel(tr("Расширенное управление профилями и маршрутами"), advancedHeader);
    advancedHint->setObjectName(QStringLiteral("advancedHint"));
    advancedBrand->addWidget(advancedTitle);
    advancedBrand->addWidget(advancedHint);
    advancedHeaderLayout->addLayout(advancedBrand);
    advancedHeaderLayout->addSpacing(12);
    ui->data_view->setMinimumWidth(0);
    ui->data_view->setMaximumHeight(42);
    advancedHeaderLayout->addWidget(ui->data_view, 1);
    auto *advancedBadge = new QLabel(tr("ADVANCED"), advancedHeader);
    advancedBadge->setObjectName(QStringLiteral("advancedBadge"));
    advancedHeaderLayout->addWidget(advancedBadge);
    auto *backButton = new QPushButton(tr("← Главная"), advancedHeader);
    backButton->setObjectName(QStringLiteral("advancedBack"));
    backButton->setCursor(Qt::PointingHandCursor);
    backButton->setMinimumHeight(38);
    advancedHeaderLayout->addWidget(backButton);
    advancedLayout->addWidget(advancedHeader);
    advancedLayout->addWidget(legacyCentralWidget, 1);

    if (auto *layout = legacyCentralWidget->layout()) {
        layout->setContentsMargins(14, 12, 14, 14);
        layout->setSpacing(10);
    }

    const QList<QToolButton *> advancedNav = {
        ui->toolButton_program, ui->toolButton_preferences, ui->toolButton_testing,
        ui->toolButton_routing, ui->toolButton_tools,
    };
    const QStringList advancedLabels = {
        tr("Меню"), tr("Параметры"), tr("Проверка"), tr("Маршруты"), tr("Инструменты"),
    };
    for (int i = 0; i < advancedNav.size(); ++i) {
        auto *button = advancedNav.at(i);
        button->setText(advancedLabels.at(i));
        button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        button->setIconSize(QSize(18, 18));
        button->setMinimumWidth(0);
        button->setMaximumWidth(132);
        button->setMinimumHeight(42);
        button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        button->setStyleSheet(QString());
    }
    ui->tabWidget->setStyleSheet(QString());
    ui->profilesTableView->setStyleSheet(QString());

    advancedCentralWidget->setStyleSheet(QStringLiteral(R"qss(
        QWidget#quattroAdvanced, QWidget#quattroAdvanced > QWidget {
            background: #141a23;
            color: #edf1f6;
            font-family: "Segoe UI Variable Text", "Segoe UI";
            font-size: 13px;
        }
        QFrame#advancedHeader {
            background: #171e27;
            border-bottom: 1px solid #303946;
        }
        QLabel#advancedTitle { color: #ffffff; font-size: 16px; font-weight: 750; }
        QLabel#advancedHint { color: #929dab; font-size: 11px; }
        QLabel#advancedBadge {
            color: #65ddeb;
            background: #173c48;
            border: 1px solid #2bbfd4;
            border-radius: 10px;
            padding: 4px 8px;
            font-size: 10px;
            font-weight: 750;
        }
        QPushButton#advancedBack {
            color: #ecf1f6;
            background: #222a35;
            border: 1px solid #3a4553;
            border-radius: 9px;
            padding: 0 13px;
            font-weight: 650;
        }
        QPushButton#advancedBack:hover { border-color: #48d0e1; color: #6de2ef; }
        QToolButton {
            color: #dfe5ec;
            background: #1b222c;
            border: 1px solid #303946;
            border-radius: 9px;
            padding: 7px 9px;
            font-weight: 600;
        }
        QToolButton:hover, QToolButton:checked {
            color: #6de2ef;
            background: #1c333e;
            border-color: #37c7d9;
        }
        QToolButton::menu-indicator { image: none; width: 0; }
        QCheckBox { color: #d9dfe7; spacing: 7px; }
        QSplitter::handle { background: #303946; height: 2px; }
        QTabWidget::pane {
            background: #1a212b;
            border: 1px solid #303946;
            border-radius: 10px;
            top: -1px;
        }
        QTabBar { background: transparent; qproperty-drawBase: 0; }
        QTabBar::tab {
            color: #9fa9b6;
            background: #181f28;
            border: 1px solid #303946;
            border-bottom: none;
            border-top-left-radius: 8px;
            border-top-right-radius: 8px;
            padding: 8px 14px;
            margin-right: 4px;
        }
        QTabBar::tab:selected { color: #69e0ed; background: #1c333e; border-color: #36c5d8; }
        QTableView, QTreeView, QListView, QTextEdit, QPlainTextEdit, QTextBrowser {
            color: #e8edf3;
            background: #171e27;
            alternate-background-color: #1b232d;
            border: 1px solid #303946;
            border-radius: 8px;
            gridline-color: #2b3440;
            selection-background-color: #174b58;
            selection-color: #ffffff;
        }
        QHeaderView::section {
            color: #aeb7c3;
            background: #202833;
            border: none;
            border-right: 1px solid #303946;
            border-bottom: 1px solid #303946;
            padding: 7px 8px;
            font-weight: 650;
        }
        QLineEdit, QComboBox, QSpinBox {
            color: #edf1f6;
            background: #151b24;
            border: 1px solid #3a4553;
            border-radius: 7px;
            padding: 5px 8px;
            selection-background-color: #1f8291;
        }
        QLineEdit:focus, QComboBox:focus, QSpinBox:focus { border-color: #3bd0e3; }
        QPushButton {
            color: #e4e9ef;
            background: #222a35;
            border: 1px solid #3a4553;
            border-radius: 8px;
            padding: 6px 11px;
        }
        QPushButton:hover { color: #6de2ef; border-color: #43cddd; background: #1c333e; }
        QScrollBar:vertical { background: #141a23; width: 8px; }
        QScrollBar::handle:vertical { background: #3d4856; border-radius: 4px; min-height: 26px; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
    )qss"));

    const QString menuStyle = QStringLiteral(
        "QMenu{background:#1b222c;color:#edf1f6;border:1px solid #3a4553;padding:6px;}"
        "QMenu::item{padding:7px 28px 7px 10px;border-radius:6px;}"
        "QMenu::item:selected{background:#174b58;color:#ffffff;}"
        "QMenu::separator{height:1px;background:#303946;margin:5px 8px;}");
    for (auto *menu : {ui->menu_program, ui->menu_preferences, ui->menuTesting,
                       ui->menuRouting_Menu, ui->menuTools}) {
        menu->setStyleSheet(menuStyle);
    }

    quattroDashboard = new QuattroDashboard(quattroStack);
    quattroStack->addWidget(quattroDashboard);
    quattroStack->addWidget(advancedCentralWidget);
    quattroStack->setCurrentWidget(quattroDashboard);
    setCentralWidget(quattroStack);

    setWindowIcon(QIcon(QStringLiteral(":/Quattro/Quattro.png")));
    setWindowTitle(QStringLiteral("Quattro"));
    resize(qMax(width(), 940), qMax(height(), 760));

    connect(backButton, &QPushButton::clicked, this, [this] {
        quattroStack->setCurrentWidget(quattroDashboard);
    });

    connect(quattroDashboard, &QuattroDashboard::advancedRequested, this, [this] {
        quattroStack->setCurrentWidget(advancedCentralWidget);
    });
    connect(quattroDashboard, &QuattroDashboard::settingsRequested,
            this, &MainWindow::on_menu_basic_settings_triggered);
    connect(quattroDashboard, &QuattroDashboard::routingRequested,
            this, &MainWindow::on_menu_routing_settings_triggered);
    connect(quattroDashboard, &QuattroDashboard::channelsRequested,
            this, &MainWindow::showQuattroChannels);
    connect(quattroDashboard, &QuattroDashboard::subscriptionSubmitted,
            this, &MainWindow::importQuattroSubscription);
    connect(quattroDashboard, &QuattroDashboard::subscriptionRefreshRequested, this, [this] {
        const auto group = Configs::dataManager->groupsRepo->CurrentGroup();
        if (!group || group->url.isEmpty()) {
            MessageBoxWarning(tr("Подписка Quattro"), tr("Сначала добавьте ссылку подписки."));
            return;
        }
        quattroDashboard->setSubscriptionState(true, tr("Обновляю список серверов…"));
        Subscription::groupUpdater->AsyncUpdate(group->url, group->id, [this] {
            runOnUiThread([this] {
                refreshQuattroDashboard(true);
                const auto current = Configs::dataManager->groupsRepo->CurrentGroup();
                const int count = current ? current->Profiles().size() : 0;
                quattroDashboard->setSubscriptionState(
                    false, count > 0 ? tr("Готово: %1 серверов").arg(count)
                                     : tr("Серверы не получены. Проверьте ссылку и журнал."));
            });
        }, true);
    });
    connect(quattroDashboard, &QuattroDashboard::modeChanged, this,
            [this](QuattroDashboard::Mode mode) {
        quattroSelectedMode = static_cast<int>(mode);
        if (mode == QuattroDashboard::Mode::Tun) {
            set_spmode_system_proxy(false);
            set_spmode_vpn(true);
        } else {
            set_spmode_vpn(false);
            set_spmode_system_proxy(true);
        }
        refreshQuattroDashboard();
    });
    connect(quattroDashboard, &QuattroDashboard::profileChanged, this, [this](int id) {
        if (id < -1) return;
        quattroSelectedProfileId = id;
        const int targetId = id < 0 ? ensureQuattroAutoSelector() : id;
        auto &settings = Configs::dataManager->settingsRepo;
        if (settings->remember_enable) {
            settings->remember_id = targetId;
            settings->Save();
        }
        if (running && targetId >= 0 && running->id != targetId) profile_start(targetId);
    });
    connect(quattroDashboard, &QuattroDashboard::connectionToggled, this, [this](bool connectNow) {
        if (!connectNow) {
            profile_stop(false, false, true);
            return;
        }
        if (quattroSelectedMode == static_cast<int>(QuattroDashboard::Mode::Tun)) {
            set_spmode_system_proxy(false);
            set_spmode_vpn(true);
            if (!Configs::dataManager->settingsRepo->spmode_vpn) return;
        } else {
            set_spmode_vpn(false);
            set_spmode_system_proxy(true);
            if (!Configs::dataManager->settingsRepo->spmode_system_proxy) return;
        }
        const int id = quattroSelectedProfileId < 0 ? ensureQuattroAutoSelector() : quattroSelectedProfileId;
        if (id < 0) {
            MessageBoxWarning(tr("Quattro"), tr("В подписке пока нет доступных серверов."));
            return;
        }
        profile_start(id);
    });
    connect(quattroDashboard, &QuattroDashboard::russiaBypassChanged,
            this, &MainWindow::setQuattroRussiaBypass);
    connect(quattroDashboard, &QuattroDashboard::autoStartChanged, this, [this](bool enabled) {
        auto &settings = Configs::dataManager->settingsRepo;
        int rememberedId = settings->remember_id;
        if (enabled) {
            rememberedId = quattroSelectedProfileId < 0
                               ? ensureQuattroAutoSelector()
                               : quattroSelectedProfileId;
            if (rememberedId < 0) {
                quattroDashboard->setAutoStart(false);
                MessageBoxWarning(tr("Автоподключение"), tr("Сначала добавьте подписку и дождитесь списка серверов."));
                return;
            }
        }
        settings->remember_enable = enabled;
        if (enabled) {
            settings->remember_id = rememberedId;
            settings->remember_tun = quattroSelectedMode == static_cast<int>(QuattroDashboard::Mode::Tun);
            settings->remember_system_proxy = !settings->remember_tun;
        }
        settings->Save();
        AutoRun_SetEnabled(enabled);
        const bool active = enabled && AutoRun_IsEnabled();
        if (enabled && !active) {
            settings->remember_enable = false;
            settings->Save();
            MessageBoxWarning(tr("Автоподключение"), tr("Windows не разрешила создать задачу автозапуска."));
        }
        ui->actionStart_with_system->setChecked(active);
        ui->actionRemember_last_proxy->setChecked(active);
        quattroDashboard->setAutoStart(active);
    });

    refreshQuattroDashboard(true);
}

void MainWindow::refreshQuattroDashboard(bool reloadProfiles) {
    if (quattroDashboard == nullptr) return;

    const auto &settings = Configs::dataManager->settingsRepo;
    QString server = running ? running->outbound->DisplayName() : QString();
    if (running && running->type == "autoselector") {
        const auto selector = Stats::autoSelectorMonitor->Snapshot();
        if (selector.valid && !selector.selectedName.isEmpty()) server = selector.selectedName;
    }
    QString status;
    if (m_profileConnecting) status = tr("Подключение…");
    else if (m_profileDisconnecting) status = tr("Отключение…");
    else status = running ? tr("Подключено") : tr("Отключено");
    quattroDashboard->setConnectionState(running != nullptr, status, server,
                                          m_profileConnecting || m_profileDisconnecting);
    const auto selectedMode = settings->spmode_system_proxy && !settings->spmode_vpn
                                  ? QuattroDashboard::Mode::SystemProxy
                                  : QuattroDashboard::Mode::Tun;
    quattroSelectedMode = static_cast<int>(selectedMode);
    quattroDashboard->setMode(selectedMode);
    quattroDashboard->setAutoStart(settings->remember_enable && ui->actionStart_with_system->isChecked());

    const auto route = Configs::dataManager->routesRepo->GetRouteProfile(settings->current_route_id);
    quattroDashboard->setRussiaBypass(route && route->name.contains("Russia", Qt::CaseInsensitive));

    if (!reloadProfiles) {
        if (running) {
            quattroSelectedProfileId = running->type == "autoselector" ? -1 : running->id;
            quattroDashboard->setSelectedProfile(quattroSelectedProfileId);
        }
        return;
    }
    QList<QPair<int, QString>> items;
    const auto group = Configs::dataManager->groupsRepo->CurrentGroup();
    if (group) {
        for (const auto &profile : Configs::dataManager->profilesRepo->GetProfileBatch(group->Profiles())) {
            if (!profile || profile->type == "autoselector") continue;
            items << qMakePair(profile->id, profile->outbound->DisplayName());
        }
    }
    int selected = running ? (running->type == "autoselector" ? -1 : running->id)
                           : quattroSelectedProfileId;
    if (selected < 0 && settings->remember_id >= 0) {
        const auto remembered = Configs::dataManager->profilesRepo->GetProfile(settings->remember_id);
        if (remembered && remembered->type != "autoselector") selected = remembered->id;
    }
    quattroSelectedProfileId = selected;
    quattroDashboard->setProfiles(items, selected);
    if (group && !group->url.isEmpty()) {
        quattroDashboard->setSubscriptionState(
            false, items.isEmpty() ? tr("Подписка сохранена, но серверы пока не загружены")
                                   : tr("Подписка активна • %1 серверов").arg(items.size()));
    } else {
        quattroDashboard->setSubscriptionState(false, tr("Вставьте ссылку Quattro, чтобы начать"));
    }
}

void MainWindow::importQuattroSubscription(const QString &url) {
    const QUrl parsed(url);
    if (!parsed.isValid() || (parsed.scheme() != "http" && parsed.scheme() != "https") || parsed.host().isEmpty()) {
        MessageBoxWarning(tr("Подписка Quattro"), tr("Нужна корректная ссылка http(s) на подписку."));
        return;
    }

    std::shared_ptr<Configs::Group> group;
    for (const int id : Configs::dataManager->groupsRepo->GetAllGroupIds()) {
        const auto candidate = Configs::dataManager->groupsRepo->GetGroup(id);
        if (candidate && QUrl(candidate->url).host().compare(parsed.host(), Qt::CaseInsensitive) == 0) {
            group = candidate;
            break;
        }
    }
    if (!group) {
        const auto current = Configs::dataManager->groupsRepo->CurrentGroup();
        if (current && current->Profiles().isEmpty() && current->url.isEmpty()) group = current;
    }
    if (!group) {
        group = Configs::GroupsRepo::NewGroup();
        group->name = QStringLiteral("Quattro");
        Configs::dataManager->groupsRepo->AddGroup(group);
    }

    group->name = QStringLiteral("Quattro");
    group->url = url;
    group->skip_auto_update = false;
    Configs::dataManager->groupsRepo->Save(group);
    Configs::dataManager->settingsRepo->current_group = group->id;
    Configs::dataManager->settingsRepo->sub_auto_update = 60;
    Configs::dataManager->settingsRepo->Save();
    refresh_groups();
    show_group(group->id);

    quattroDashboard->setSubscriptionState(true, tr("Проверяю подписку и загружаю серверы…"));
    Subscription::groupUpdater->AsyncUpdate(url, group->id, [this] {
        runOnUiThread([this] {
            refreshQuattroDashboard(true);
            const auto current = Configs::dataManager->groupsRepo->CurrentGroup();
            const int count = current ? current->Profiles().size() : 0;
            quattroDashboard->setSubscriptionState(
                false, count > 0 ? tr("Готово: %1 серверов").arg(count)
                                 : tr("Серверы не получены. Проверьте ссылку и журнал."));
        });
    });
}

int MainWindow::ensureQuattroAutoSelector() {
    const auto group = Configs::dataManager->groupsRepo->CurrentGroup();
    if (!group || group->Profiles().isEmpty()) return -1;

    for (const auto &profile : Configs::dataManager->profilesRepo->GetProfileBatch(group->Profiles())) {
        if (profile && profile->type == "autoselector") {
            auto *selector = profile->AutoSelector();
            if (selector) {
                // Pick the fastest member on start, then keep it while it is healthy.
                // A very wide hysteresis prevents routine latency jitter from moving
                // sessions; the core still fails over immediately when health drops.
                selector->toleranceMs = 60000;
                selector->watchIntervalSec = 15;
                selector->expected = 3;
                selector->balance = false;
                selector->interruptOnSwitch = true;
                selector->Normalize();
                Configs::dataManager->profilesRepo->Save(profile);
            }
            return profile->id;
        }
    }

    auto profile = Configs::ProfilesRepo::NewProfile(QStringLiteral("autoselector"));
    if (!profile || !profile->AutoSelector()) return -1;
    profile->name = tr("Авто — быстрый + failover");
    auto *selector = profile->AutoSelector();
    selector->name = profile->name;
    selector->gid = group->id;
    selector->resultValidityMins = 1440;
    selector->intervalSec = 300;
    selector->benchIntervalSec = 600;
    selector->watchIntervalSec = 15;
    selector->expected = 3;
    selector->activeSize = 8;
    selector->toleranceMs = 60000;
    selector->interruptOnSwitch = true;
    selector->balance = false;
    selector->Normalize();
    if (!Configs::dataManager->profilesRepo->AddProfile(profile, group->id)) return -1;
    return profile->id;
}

void MainWindow::setQuattroRussiaBypass(bool enabled) {
    auto &settings = Configs::dataManager->settingsRepo;
    const auto current = Configs::dataManager->routesRepo->GetRouteProfile(settings->current_route_id);
    const bool hasChannels = current && current->name.startsWith(QStringLiteral("Quattro Channels"));
    std::shared_ptr<Configs::RouteProfile> base;
    for (const auto &route : Configs::dataManager->routesRepo->GetAllRouteProfiles()) {
        if (!route || route->name.startsWith(QStringLiteral("Quattro Channels"))) continue;
        const bool match = enabled ? route->name.contains("Russia", Qt::CaseInsensitive)
                                   : route->name.compare("Default", Qt::CaseInsensitive) == 0;
        if (match) {
            base = route;
            break;
        }
    }

    if (!base) {
        if (enabled) {
            MessageBoxWarning(tr("Russia Bypass"),
                              tr("Профиль Bypass Russia ещё не установлен. Открою менеджер маршрутов — выберите Download Profiles → Russia один раз."));
            on_menu_routing_settings_triggered();
        } else {
            MessageBoxWarning(tr("Russia Bypass"), tr("Не найден стандартный профиль маршрутизации Default."));
        }
        refreshQuattroDashboard();
        return;
    }

    if (hasChannels) {
        if (base->isRaw) {
            MessageBoxWarning(tr("Russia Bypass"),
                              tr("Этот профиль Russia использует необъединяемые raw-правила. Выберите другой профиль Russia в менеджере маршрутов."));
            refreshQuattroDashboard();
            return;
        }
        auto merged = std::make_shared<Configs::RouteProfile>(*base);
        merged->id = current->id;
        merged->name = enabled ? QStringLiteral("Quattro Channels · Bypass Russia")
                               : QStringLiteral("Quattro Channels");
        merged->isRemote = false;
        merged->remoteURL.clear();
        merged->autoUpdate = false;
        QList<std::shared_ptr<Configs::RouteRule>> custom;
        for (const auto &rule : current->Rules) {
            if (rule && rule->name.startsWith(QStringLiteral("Quattro •")))
                custom << std::make_shared<Configs::RouteRule>(*rule);
        }
        merged->Rules = custom + merged->Rules;
        if (!Configs::dataManager->routesRepo->Save(merged)) {
            MessageBoxWarning(tr("Russia Bypass"), tr("Не удалось сохранить объединённые правила маршрутизации."));
            refreshQuattroDashboard();
            return;
        }
        settings->current_route_id = merged->id;
    } else {
        settings->current_route_id = base->id;
    }
    settings->Save();
    if (settings->started_id >= 0) profile_start(settings->started_id);
    refreshQuattroDashboard();
}

void MainWindow::showQuattroChannels() {
    const auto current = Configs::dataManager->routesRepo->GetRouteProfile(
        Configs::dataManager->settingsRepo->current_route_id);
    if (current && current->isRaw) {
        MessageBoxWarning(tr("Каналы Quattro"),
                          tr("Текущий raw-профиль нельзя безопасно объединить с каналами приложений. Выберите Default или структурированный Bypass Russia."));
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle(tr("Каналы Quattro"));
    dialog.setMinimumWidth(520);
    auto *layout = new QFormLayout(&dialog);
    layout->setContentsMargins(24, 22, 24, 22);
    layout->setSpacing(13);

    auto *hint = new QLabel(tr("Основной канал уже обслуживает ChatGPT, Copilot, Telegram и Adobe. "
                               "Здесь можно закрепить Google/Gemini за подходящим гео-сервером и "
                               "оставить Steam и Minimax на прямом соединении."), &dialog);
    hint->setWordWrap(true);
    layout->addRow(hint);

    auto *googleServer = new QComboBox(&dialog);
    googleServer->addItem(tr("Как основной VPN"), Configs::proxyID);
    const auto group = Configs::dataManager->groupsRepo->CurrentGroup();
    if (group) {
        for (const auto &profile : Configs::dataManager->profilesRepo->GetProfileBatch(group->Profiles())) {
            if (!profile || profile->type == "autoselector") continue;
            googleServer->addItem(profile->outbound->DisplayName(), profile->id);
        }
    }
    bool hadChannelSettings = false;
    bool savedSteamDirect = false;
    bool savedMinimaxDirect = false;
    int savedGoogleServer = Configs::proxyID;
    if (current && current->name.startsWith(QStringLiteral("Quattro Channels"))) {
        for (const auto &rule : current->Rules) {
            if (!rule || !rule->name.startsWith(QStringLiteral("Quattro •"))) continue;
            hadChannelSettings = true;
            if (rule->name == QStringLiteral("Quattro • Google geo channel"))
                savedGoogleServer = rule->outboundID;
            if (rule->domain_suffix.contains(QStringLiteral("steampowered.com")) ||
                rule->process_name.contains(QStringLiteral("steam.exe")))
                savedSteamDirect = true;
            if (rule->domain_suffix.contains(QStringLiteral("minimax.io")))
                savedMinimaxDirect = true;
        }
    }
    const int googleIndex = googleServer->findData(savedGoogleServer);
    if (googleIndex >= 0) googleServer->setCurrentIndex(googleIndex);
    layout->addRow(tr("Google / Gemini:"), googleServer);

    auto *steamDirect = new QCheckBox(tr("Steam — напрямую"), &dialog);
    auto *minimaxDirect = new QCheckBox(tr("Minimax — напрямую"), &dialog);
    steamDirect->setChecked(hadChannelSettings ? savedSteamDirect : true);
    minimaxDirect->setChecked(hadChannelSettings ? savedMinimaxDirect : true);
    layout->addRow(steamDirect);
    layout->addRow(minimaxDirect);

    auto *protectedServices = new QLabel(tr("Через основной VPN: ChatGPT/OpenAI, Microsoft/Copilot, Telegram, Adobe"), &dialog);
    protectedServices->setStyleSheet(QStringLiteral("color:#5f6670"));
    protectedServices->setWordWrap(true);
    layout->addRow(protectedServices);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, &dialog);
    buttons->button(QDialogButtonBox::Save)->setText(tr("Применить"));
    buttons->button(QDialogButtonBox::Cancel)->setText(tr("Отмена"));
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addRow(buttons);
    if (dialog.exec() != QDialog::Accepted) return;

    const bool updateExisting = current && current->name.startsWith(QStringLiteral("Quattro Channels"));
    const bool inheritsRussiaBypass = current && current->name.contains("Russia", Qt::CaseInsensitive);
    auto channels = current ? std::make_shared<Configs::RouteProfile>(*current)
                            : Configs::RouteProfile::GetDefaultChain();
    channels->id = updateExisting ? current->id : -1;
    channels->name = inheritsRussiaBypass
                         ? QStringLiteral("Quattro Channels · Bypass Russia")
                         : QStringLiteral("Quattro Channels");
    channels->isRemote = false;
    channels->remoteURL.clear();
    channels->autoUpdate = false;

    QList<std::shared_ptr<Configs::RouteRule>> custom;
    if (steamDirect->isChecked() || minimaxDirect->isChecked()) {
        auto directDomains = std::make_shared<Configs::RouteRule>();
        directDomains->name = QStringLiteral("Quattro • Direct services");
        directDomains->type = Configs::simpleAddressBypass;
        directDomains->action = QStringLiteral("route");
        directDomains->outboundID = Configs::directID;
        if (steamDirect->isChecked()) {
            directDomains->domain_suffix << "steampowered.com" << "steamcommunity.com"
                                         << "steamstatic.com" << "steamcontent.com";
        }
        if (minimaxDirect->isChecked()) {
            directDomains->domain_suffix << "minimax.io" << "minimaxi.com";
        }
        custom << directDomains;
    }

    if (steamDirect->isChecked()) {
        auto directProcesses = std::make_shared<Configs::RouteRule>();
        directProcesses->name = QStringLiteral("Quattro • Direct applications");
        directProcesses->type = Configs::simpleProcessNameBypass;
        directProcesses->action = QStringLiteral("route");
        directProcesses->outboundID = Configs::directID;
        directProcesses->process_name << "steam.exe" << "steamwebhelper.exe";
        custom << directProcesses;
    }

    auto google = std::make_shared<Configs::RouteRule>();
    google->name = QStringLiteral("Quattro • Google geo channel");
    google->type = Configs::simpleAddressProxy;
    google->action = QStringLiteral("route");
    google->outboundID = googleServer->currentData().toInt();
    google->domain_suffix << "google.com" << "googleapis.com" << "gstatic.com"
                          << "googleusercontent.com" << "youtube.com" << "ytimg.com"
                          << "gemini.google.com";
    custom << google;

    QList<std::shared_ptr<Configs::RouteRule>> inherited;
    for (const auto &rule : channels->Rules) {
        if (!rule || rule->name.startsWith("Quattro •")) continue;
        inherited << rule;
    }
    channels->Rules = custom + inherited;
    const bool saved = updateExisting
                           ? Configs::dataManager->routesRepo->Save(channels)
                           : Configs::dataManager->routesRepo->AddRouteProfile(channels);
    if (!saved) {
        MessageBoxWarning(tr("Каналы Quattro"), tr("Не удалось сохранить профиль маршрутизации."));
        return;
    }
    Configs::dataManager->settingsRepo->current_route_id = channels->id;
    Configs::dataManager->settingsRepo->Save();
    if (Configs::dataManager->settingsRepo->started_id >= 0)
        profile_start(Configs::dataManager->settingsRepo->started_id);
    refreshQuattroDashboard();
}
