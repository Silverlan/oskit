// SPDX-FileCopyrightText: (c) 2025 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#ifdef __linux__
#include <sdbus-c++/sdbus-c++.h>
#endif

module pragma.oskit;

import :notify;

using namespace pragma::oskit;

class INotificationManager {
public:
	virtual bool ShowNotification(const NotificationInfo &info) = 0;
private:
};

#ifdef __linux__
class NotificationManager : public INotificationManager {
public:
	NotificationManager();
	virtual bool ShowNotification(const NotificationInfo &info) override;
private:
	std::unique_ptr<sdbus::IProxy> m_proxy;
};

NotificationManager::NotificationManager() {
	sdbus::ServiceName destination{"org.freedesktop.portal.Desktop"};
	sdbus::ObjectPath objectPath{"/org/freedesktop/portal/desktop"};
	m_proxy = sdbus::createProxy(
		std::move(destination), std::move(objectPath)
	);
}

#include <sys/mman.h>
bool NotificationManager::ShowNotification(const NotificationInfo &info) {
	static uint64_t seq = 1;
	std::string id = "notif-" + std::to_string(seq++);

	std::map<std::string, sdbus::Variant> notif;
	notif["title"] = sdbus::Variant(info.title);
	notif["body"]  = sdbus::Variant(info.body);

	// This does not work.
	// TODO: Figure out how to set the icon
	/*auto f = memfd_create("logo", MFD_ALLOW_SEALING);
	// materials/logo/pragma_logo.png
	sdbus::UnixFd fd(f);
	auto fdVariant = sdbus::Variant(fd);
	auto fdStruct = sdbus::make_struct(
		std::string("file-descriptor"),
	 fdVariant
	);
	notif["icon"] = sdbus::Variant(fdVariant);*/

	m_proxy->callMethod("AddNotification")
		 .onInterface("org.freedesktop.portal.Notification")
		 .withArguments(id, notif);

	return true;
}

#else

// TODO: Implement for Windows
class NotificationManager : public INotificationManager {
public:
	NotificationManager();
	virtual bool ShowNotification(const NotificationInfo &info) override { return false; }
};

#endif

static std::unique_ptr<INotificationManager> m_manager = nullptr;
static INotificationManager &get_manager() {
	if (!m_manager)
		m_manager = std::make_unique<NotificationManager>();
	return *m_manager;
}

static void clear_manager() {
	m_manager = nullptr;
}

bool pragma::oskit::show_notification(const NotificationInfo &info) {
	auto &manager = get_manager();
	return manager.ShowNotification(info);
}

void pragma::oskit::shutdown() {
	clear_manager();
}

