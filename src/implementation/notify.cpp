// SPDX-FileCopyrightText: (c) 2025 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "definitions.hpp"
#ifdef __linux__

#ifdef USE_SDBUS
#include <sdbus-c++/sdbus-c++.h>
#include <sys/mman.h>
#elif USE_LIBNOTIFY
#include <libnotify/notify.h>
#endif

#elif _WIN32
#include <wintoastlib.h>
#include <codecvt>
#endif
#include <memory>

module pragma.oskit;

import :notify;

using namespace pragma::oskit;

class DLLPOSKIT INotificationManager {
  public:
	virtual ~INotificationManager() = default;
	virtual bool ShowNotification(const NotificationInfo &info) = 0;
  private:
};

#ifdef __linux__

#ifdef USE_SDBUS
class DLLPOSKIT NotificationManager : public INotificationManager {
  public:
	NotificationManager();
	virtual bool ShowNotification(const NotificationInfo &info) override;
  private:
	std::unique_ptr<sdbus::IProxy> m_proxy;
};

NotificationManager::NotificationManager()
{
	sdbus::ServiceName destination {"org.freedesktop.portal.Desktop"};
	sdbus::ObjectPath objectPath {"/org/freedesktop/portal/desktop"};
	m_proxy = sdbus::createProxy(std::move(destination), std::move(objectPath));
}

bool NotificationManager::ShowNotification(const NotificationInfo &info)
{
	static uint64_t seq = 1;
	std::string id = "notif-" + std::to_string(seq++);

	std::map<std::string, sdbus::Variant> notif;
	notif["title"] = sdbus::Variant(info.title);
	notif["body"] = sdbus::Variant(info.body);

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

	m_proxy->callMethod("AddNotification").onInterface("org.freedesktop.portal.Notification").withArguments(id, notif);

	return true;
}
#elif USE_LIBNOTIFY
class DLLPOSKIT NotificationManager : public INotificationManager {
  public:
	NotificationManager();
	~NotificationManager() override;
	virtual bool ShowNotification(const NotificationInfo &info) override;
};

NotificationManager::NotificationManager() { notify_init("oskit"); }

NotificationManager::~NotificationManager() { notify_uninit(); }

bool NotificationManager::ShowNotification(const NotificationInfo &info)
{
	if(!notify_is_initted())
		return false;

	auto &iconPath = info.appIcon;
	auto *n = notify_notification_new(info.title.c_str(), info.body.c_str(), iconPath.c_str());

	GError *error = nullptr;
	auto success = notify_notification_show(n, &error);

	if(!success)
		g_error_free(error);

	g_object_unref(G_OBJECT(n));
	return success;
}
#endif

#elif _WIN32

using namespace WinToastLib;

class WinToastHandlerExample : public IWinToastHandler {
  public:
	WinToastHandlerExample() {}
	// Public interfaces
	void toastActivated() const override {}
	void toastActivated(int actionIndex) const override {}
	void toastActivated(std::wstring response) const override {}
	void toastDismissed(WinToastDismissalReason state) const override {}
	void toastFailed() const override {}
};

class DLLPOSKIT NotificationManager : public INotificationManager {
  public:
	NotificationManager() : INotificationManager {} {}
	virtual bool ShowNotification(const NotificationInfo &info) override;
};

static std::wstring string_to_wstring(const std::string &str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(str);
}

bool NotificationManager::ShowNotification(const NotificationInfo &info)
{
	if(!WinToast::isCompatible()) {
		// System not supported
		return false;
	}

	WinToast::instance()->setAppName(string_to_wstring(info.appName));
	const auto aumi = WinToast::configureAUMI(L"silverlan", L"pragma", L"pragma", L"1.0.0");
	WinToast::instance()->setAppUserModelId(aumi);

	if(!WinToast::instance()->initialize()) {
		// Failed to initialize library
		return false;
	}

	WinToastHandlerExample *handler = new WinToastHandlerExample;
	WinToastTemplate templ = WinToastTemplate(WinToastTemplate::ImageAndText02);
	templ.setImagePath(string_to_wstring(info.appIcon));
	templ.setTextField(string_to_wstring(info.title), WinToastTemplate::FirstLine);
	templ.setTextField(string_to_wstring(info.body), WinToastTemplate::SecondLine);

	WinToast::WinToastError error;
	const auto toast_id = WinToast::instance()->showToast(templ, handler, &error);
	if(toast_id < 0) {
		// Failed to show toast
		return false;
	}
	return true;
}

#endif

static std::unique_ptr<INotificationManager> m_manager = nullptr;
static INotificationManager &get_manager()
{
	if(!m_manager)
		m_manager = std::make_unique<NotificationManager>();
	return *m_manager;
}

static void clear_manager() { m_manager = nullptr; }

bool pragma::oskit::show_notification(const NotificationInfo &info)
{
	auto &manager = get_manager();
	return manager.ShowNotification(info);
}

void pragma::oskit::shutdown() { clear_manager(); }
