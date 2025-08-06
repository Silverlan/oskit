// SPDX-FileCopyrightText: (c) 2025 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __PRAGMA_OSKIT_DEFINITIONS_HPP__
#define __PRAGMA_OSKIT_DEFINITIONS_HPP__

#if POSKIT_EXPORT
#ifdef __linux__
#define DLLPOSKIT __attribute__((visibility("default")))
#else
#define DLLPOSKIT __declspec(dllexport)
#endif
#else
#ifdef __linux__
#define DLLPOSKIT
#else
#define DLLPOSKIT __declspec(dllimport)
#endif
#endif

#endif
