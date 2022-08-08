#pragma once

//TODO cleanup how the 32/64 bit architecture is defined

#if defined WIN32
#define __PLATFORM_WINDOWS 1
#define __PLATFORM_64BIT 1
#endif

#if defined __ANDROID__
#define __PLATFORM_ANDROID 1
//#define __PLATFORM_32BIT 1
#endif

#if defined __linux__
#define __PLATFORM_LINUX 1
//#define __PLATFORM_64BIT 1
#endif

#if defined _M_ARM
#undef __PLATFORM_64BIT
#define __PLATFORM_32BIT 1
#endif

#if defined __APPLE__

#include <TargetConditionals.h>

#if defined TARGET_OS_IOS && TARGET_OS_IOS == 1
#define __PLATFORM_APPLE 1
#define __PLATFORM_IOS 1
#endif

#if defined TARGET_OS_OSX && TARGET_OS_OSX == 1
#define __PLATFORM_APPLE 1
#define __PLATFORM_MACOS 1
#endif

#endif

#if defined __PLATFORM_WINDOWS
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
#include <xinput.h>
#include <shlobj.h> 
#define HAVE_STRUCT_TIMESPEC
#endif

#if defined __PLATFORM_ANDROID || __PLATFORM_MACOS
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
#include <cstdlib>
#ifndef MAP_FILE
#define MAP_FILE 0
#endif
#endif

#if defined __PLATFORM_IOS
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
#include <cstdlib>
#ifndef MAP_FILE
#define MAP_FILE 0
#endif
#endif


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
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
#include <list>
#include <mutex>
#include <map>
#include <memory>
#include <iostream>
#include <sstream>
#include <fstream>
#include <random>
#include <thread>
#include <functional>
#include <chrono>
#include <variant>

#include <pthread.h>

/// Utilities
#include "utilities/common.hpp"
#include "utilities/type_cast.hpp"
#include "utilities/spatial.hpp"
#include "utilities/properties.hpp"

#include "platform/interfaces/input.hpp"

/// Types
#include "types/interfaces/info.hpp"
#include "types/interfaces/audio.hpp"
#include "types/interfaces/shader.hpp"
#include "types/interfaces/program.hpp"
#include "types/interfaces/image.hpp"
#include "types/interfaces/material.hpp"
#include "types/interfaces/object.hpp"
#include "types/interfaces/font.hpp"
#include "types/interfaces/entity.hpp"

#undef interface // Thanks Microsoft

/// Platform Abstractions
#include "platform/interfaces/audio.hpp"
#include "platform/interfaces/filesystem.hpp"
#include "platform/interfaces/assets.hpp"
#include "platform/interfaces/interface.hpp"
#include "platform/interfaces/graphics.hpp"
#include "platform/interfaces/network.hpp"
#include "platform/interfaces/application.hpp"


/// Linux platform 
#if defined __PLATFORM_ANDROID
#include "platform/implementations/universal.hpp"
#include "platform/implementations/opengl.hpp"
#include "platform/implementations/opensl.hpp"
#include "platform/implementations/posix.hpp"
#include "platform/implementations/android.hpp"
inline platform::audio* audio = new implementation::opensl::audio();
inline platform::filesystem* filesystem = new implementation::posix::filesystem();
inline platform::assets* assets = new implementation::android::assets();
inline platform::graphics* graphics = new implementation::opengl::graphics();
inline platform::input* input = new implementation::universal::input();
inline platform::interface* gui = new implementation::universal::interface();
inline platform::network::client* client = new implementation::posix::network::client();
inline platform::network::server* server = new implementation::posix::network::server();
#endif


/// Windows platform
#if defined __PLATFORM_WINDOWS
#include "platform/implementations/universal.hpp"
#include "platform/implementations/opengl.hpp"
#include "platform/implementations/openal.hpp"
#include "platform/implementations/windows.hpp"
inline platform::audio* audio = new implementation::openal::audio();
inline platform::filesystem* filesystem = new implementation::windows::filesystem();
inline platform::assets* assets = new implementation::windows::assets();
inline platform::graphics* graphics = new implementation::opengl::graphics();
inline platform::input* input = new implementation::universal::input();
inline platform::interface* gui = new implementation::universal::interface();
inline platform::network::client* client = new implementation::windows::network::client();
inline platform::network::server* server = new implementation::windows::network::server();
#endif

#if defined __PLATFORM_IOS
#include "platform/implementations/universal.hpp"
#include "platform/implementations/opengl.hpp"
#include "platform/implementations/openal.hpp"
#include "platform/implementations/posix.hpp"
#include "platform/implementations/ios.hpp"
inline platform::audio* audio = new implementation::openal::audio();
inline platform::filesystem* filesystem = new implementation::posix::filesystem();
inline platform::assets* assets = new implementation::ios::assets();
inline platform::graphics* graphics = new implementation::opengl::graphics();
inline platform::input* input = new implementation::universal::input();
inline platform::interface* gui = new implementation::universal::interface();
//inline platform::network::client* client = new implementation::posix::network::client();
//inline platform::network::server* server = new implementation::posix::network::server();
#endif

#if defined __PLATFORM_MACOS
#include "platform/implementations/universal.hpp"
#include "platform/implementations/opengl.hpp"
#include "platform/implementations/openal.hpp"
#include "platform/implementations/posix.hpp"
//#include "platform/implementations/macos.hpp"
inline platform::audio* audio = new implementation::openal::audio();
inline platform::filesystem* filesystem = new implementation::posix::filesystem();
inline platform::assets* assets;// = new implementation::macos::assets();
inline platform::graphics* graphics = new implementation::opengl::graphics();
inline platform::input* input = new implementation::universal::input();
inline platform::interface* gui = new implementation::universal::interface();
//inline platform::network::client* client = new implementation::posix::network::client();
//inline platform::network::server* server = new implementation::posix::network::server();
#endif


/// Supported Formats
#include "types/formats/wav.hpp"
#include "types/formats/frag.hpp"
#include "types/formats/vert.hpp"
#include "types/formats/tga.hpp"
#include "types/formats/png.hpp"
#include "types/formats/fnt.hpp"
#include "types/formats/mtl.hpp"
#include "types/formats/obj.hpp"
#include "types/formats/fbx.hpp"


#include "utilities/scene.hpp"

