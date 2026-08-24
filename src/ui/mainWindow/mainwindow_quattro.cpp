#include "include/ui/mainwindow.h"

#include "include/configs/sub/GroupUpdater.hpp"
#include "include/database/GroupsRepo.h"
#include "include/database/ProfilesRepo.h"
#include "include/database/RoutesRepo.h"
#include "include/database/entities/RouteRule.h"
#include "include/sys/AutoRun.hpp"
#include "include/ui/widget/QuattroDashboard.hpp"

#include <QIcon>
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QUrl>

void MainWindow::setupQuattroDashboard() {
    if (quattroDashboard != nullptr) return;

    advancedCentralWidget = takeCentralWidget();
    quattroStack = new QStackedWidget(this);
    quattroDashboard = new QuattroDashboard(quattroStack);
    quattroStack->addWidget(quattroDashboard);
    quattroStack->addWidget(advancedCentralWidget);
    quattroStack->setCurrentWidget(quattroDashboard);
    setCentralWidget(quattroStack);

    setWindowIcon(QIcon(QStringLiteral(":/Quattro/Quattro.png")));
    setWindowTitle(QStringLiteral("Quattro"));
    resize(qMax(width(), 940), qMax(height(), 760));

    auto *backButton = new QPushButton(tr("← Quattro"), advancedCentralWidget);
    backButton->setMinimumHeight(34);
    if (auto *layout = advancedCentralWidget->layout()) layout->setMenuBar(backButton);
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
        Subscription::groupUpdater->AsyncUpdate(group->url, group->id, [this] {
            runOnUiThread([this] { refreshQuattroDashboard(true); });
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
        quattroSelectedProfileId = id;
        auto &settings = Configs::dataManager->settingsRepo;
        if (settings->remember_enable) {
            settings->remember_id = id < 0 ? ensureQuattroAutoSelector() : id;
            settings->Save();
        }
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
        settings->remember_enable = enabled;
        if (enabled) {
            settings->remember_id = quattroSelectedProfileId < 0
                                        ? ensureQuattroAutoSelector()
                                        : quattroSelectedProfileId;
        }
        settings->Save();
        AutoRun_SetEnabled(enabled);
    });

    refreshQuattroDashboard(true);
}

void MainWindow::refreshQuattroDashboard(bool reloadProfiles) {
    if (quattroDashboard == nullptr) return;

    const auto &settings = Configs::dataManager->settingsRepo;
    const QString server = running ? running->outbound->DisplayName() : QString();
    QString status;
    if (m_profileConnecting) status = tr("Подключение…");
    else if (m_profileDisconnecting) status = tr("Отключение…");
    else status = running ? tr("Подключено") : tr("Отключено");
    quattroDashboard->setConnectionState(running != nullptr, status, server);
    const auto selectedMode = settings->spmode_system_proxy && !settings->spmode_vpn
                                  ? QuattroDashboard::Mode::SystemProxy
                                  : QuattroDashboard::Mode::Tun;
    quattroSelectedMode = static_cast<int>(selectedMode);
    quattroDashboard->setMode(selectedMode);
    quattroDashboard->setAutoStart(settings->remember_enable);

    const auto route = Configs::dataManager->routesRepo->GetRouteProfile(settings->current_route_id);
    quattroDashboard->setRussiaBypass(route && route->name.contains("Russia", Qt::CaseInsensitive));

    if (!reloadProfiles) return;
    QList<QPair<int, QString>> items;
    const auto group = Configs::dataManager->groupsRepo->CurrentGroup();
    if (group) {
        for (const auto &profile : Configs::dataManager->profilesRepo->GetProfileBatch(group->Profiles())) {
            if (!profile || profile->type == "autoselector") continue;
            items << qMakePair(profile->id, profile->outbound->DisplayName());
        }
    }
    int selected = quattroSelectedProfileId;
    if (selected < 0 && settings->remember_id >= 0) {
        const auto remembered = Configs::dataManager->profilesRepo->GetProfile(settings->remember_id);
        if (remembered && remembered->type != "autoselector") selected = remembered->id;
    }
    quattroSelectedProfileId = selected;
    quattroDashboard->setProfiles(items, selected);
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

    Subscription::groupUpdater->AsyncUpdate(url, group->id, [this] {
        runOnUiThread([this] { refreshQuattroDashboard(true); });
    });
}

int MainWindow::ensureQuattroAutoSelector() {
    const auto group = Configs::dataManager->groupsRepo->CurrentGroup();
    if (!group || group->Profiles().isEmpty()) return -1;

    for (const auto &profile : Configs::dataManager->profilesRepo->GetProfileBatch(group->Profiles())) {
        if (profile && profile->type == "autoselector") return profile->id;
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
    selector->toleranceMs = 20;
    selector->interruptOnSwitch = true;
    selector->balance = false;
    selector->Normalize();
    if (!Configs::dataManager->profilesRepo->AddProfile(profile, group->id)) return -1;
    return profile->id;
}

void MainWindow::setQuattroRussiaBypass(bool enabled) {
    auto &settings = Configs::dataManager->settingsRepo;
    if (!enabled) {
        for (const auto &route : Configs::dataManager->routesRepo->GetAllRouteProfiles()) {
            if (route && route->name.compare("Default", Qt::CaseInsensitive) == 0) {
                settings->current_route_id = route->id;
                settings->Save();
                if (settings->started_id >= 0) profile_start(settings->started_id);
                return;
            }
        }
        return;
    }

    for (const auto &route : Configs::dataManager->routesRepo->GetAllRouteProfiles()) {
        if (route && route->name.contains("Russia", Qt::CaseInsensitive)) {
            settings->current_route_id = route->id;
            settings->Save();
            if (settings->started_id >= 0) profile_start(settings->started_id);
            return;
        }
    }

    MessageBoxWarning(tr("Russia Bypass"),
                      tr("Профиль Bypass Russia ещё не установлен. Открою менеджер маршрутов — выберите Download Profiles → Russia один раз."));
    on_menu_routing_settings_triggered();
    refreshQuattroDashboard();
}

void MainWindow::showQuattroChannels() {
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
    layout->addRow(tr("Google / Gemini:"), googleServer);

    auto *steamDirect = new QCheckBox(tr("Steam — напрямую"), &dialog);
    auto *minimaxDirect = new QCheckBox(tr("Minimax — напрямую"), &dialog);
    steamDirect->setChecked(true);
    minimaxDirect->setChecked(true);
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

    const auto current = Configs::dataManager->routesRepo->GetRouteProfile(
        Configs::dataManager->settingsRepo->current_route_id);
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
