#pragma once

#if defined WIN32
#define __PLATFORM_WINDOWS 1
#endif

#if defined __ANDROID__
#define __PLATFORM_ANDROID 1
#endif

#if defined __linux__
#define __PLATFORM_LINUX 1
#endif


#if defined(__PLATFORM_WINDOWS)
#ifndef _UNICODE
#error Set "Use Unicode Character Set" in the general project settings
#endif
#define WIDE 1
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#if defined __PLATFORM_WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <rpc.h>
#include <windows.h>
#include <lmerr.h>
#define fseeko fseek
#define ftello ftell
#include <stdio.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <shellapi.h>
#endif

#if defined __PLATFORM_ANDROID
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <utime.h>
#include <netdb.h>
#include <resolv.h>
#include <cstring>
#ifndef MAP_FILE
#define MAP_FILE 0
#endif
#endif

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <sys/timeb.h>
#include <stdarg.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <errno.h>

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <sstream>
#include <fstream>

/// Utilities
#include "utilities/type_cast.hpp"

/// Types
#include "types/interfaces/info.hpp"
#include "types/interfaces/audio.hpp"
#include "types/interfaces/shader.hpp"
#include "types/interfaces/program.hpp"
#include "types/interfaces/image.hpp"
#include "types/interfaces/material.hpp"
#include "types/interfaces/object.hpp"
#include "types/interfaces/font.hpp"


/// Supported Formats
#include "types/formats/wav.hpp"
#include "types/formats/frag.hpp"
#include "types/formats/vert.hpp"
#include "types/formats/tga.hpp"
#include "types/formats/png.hpp"


/// Platform Abstractions
#include "platform/interfaces/audio.hpp"
#include "platform/interfaces/filesystem.hpp"
#include "platform/interfaces/assets.hpp"
#include "platform/interfaces/graphics.hpp"


/// Linux platform 
#if defined __PLATFORM_ANDROID
#include "platform/implementations/opengl.hpp"
#include "platform/implementations/opensl.hpp"
#include "platform/implementations/posix.hpp"
#include "platform/implementations/android.hpp"
inline platform::audio* audio = new implementation::opensl::audio();
inline platform::filesystem* filesystem = new implementation::posix::filesystem();
inline platform::assets* assets = new implementation::android::assets();
#endif

/// Windows platform
#if defined __PLATFORM_WINDOWS
#include "platform/implementations/opengl.hpp"
#include "platform/implementations/openal.hpp"
#include "platform/implementations/windows.hpp"
inline platform::audio* audio = new implementation::openal::audio();
inline platform::filesystem* filesystem = new implementation::windows::filesystem();
inline platform::assets* assets = new implementation::windows::assets();
#endif

/// Just a simple prototype implementation to test infrastructure portability
#include "prototype.hpp"

/// Application definition instance to pass information into the prototype
inline application* instance = new application();

