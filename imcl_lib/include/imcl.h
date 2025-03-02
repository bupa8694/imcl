// Copyright 2024, Macx Buddhi Chaturanga. All Rights Reserved.

#ifndef IMCL_LIB_INCLUDE_IMCL_H_
#define IMCL_LIB_INCLUDE_IMCL_H_

#include "imcl_types.h"  // NOLINT

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
    #ifdef BUILDING_DLL
        #define IMCL_API __declspec(dllexport)
    #else
        #define IMCL_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #define IMCL_API __attribute__((visibility("default")))
#else
    #define IMCL_API
#endif

#if defined(_WIN32)
#ifndef PLATFORM_WINDOWS
    #define PLATFORM_WINDOWS
#endif
#elif defined(__APPLE__)
#ifndef PLATFORM_MACOS
    #define PLATFORM_MACOS
#endif
#elif defined(__linux__)
    #ifndef PLATFORM_LINUX
    #define PLATFORM_LINUX
#endif
#endif

// Platform-specific code can be added here if needed
#ifdef PLATFORM_WINDOWS
    // Windows-specific code
#elif defined(PLATFORM_MACOS)
    // macOS-specific code
#elif defined(PLATFORM_LINUX)
    // Linux-specific code
#endif

// Library initialization function
IMCL_API IMCL_STATUS imcl_init();
IMCL_API IMCL_STATUS imcl_load(const char*);
#ifdef __cplusplus
}
#endif

#endif  // IMCL_LIB_INCLUDE_IMCL_H_
