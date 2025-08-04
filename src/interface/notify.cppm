// SPDX-FileCopyrightText: (c) 2025 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <string>

export module pragma.oskit:notify;

export namespace pragma::oskit {
	struct NotificationInfo {
		std::string appName;
		std::string appIcon;
		std::string title;
		std::string body;
	};

	bool show_notification(const NotificationInfo &info);
	void shutdown();
};
