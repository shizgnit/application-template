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
#define GLEW_STATIC
//#include <GL\glew.h>
//#include <GL\gl.h>
//#include <GL\glu.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <shellapi.h>
#endif

#if defined __PLATFORM_ANDROID
#  include <unistd.h>
#  include <fcntl.h>
#  include <sys/mman.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <dirent.h>
#  include <utime.h>
#  include <netdb.h>
#  include <resolv.h>
#  include <cstring>
#  ifndef MAP_FILE
#    define MAP_FILE 0
#  endif
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

#include "framework/type_cast.hpp"

#include "platform/interfaces/filesystem.hpp"

#if defined __PLATFORM_LINUX
#include "platform/implementations/posix.hpp"
inline interfaces::filesystem* filesystem = new posix::filesystem();
#endif

#if defined __PLATFORM_WINDOWS
#include "platform/implementations/winapi.hpp"
inline interfaces::filesystem* filesystem = new winapi::filesystem();
#endif

/*
auto test_return() {
	return std::pair<int, int>({ 2, 10 });
}

void test_capabilities() {
	int something = 2;
	auto [foo, bar] = test_return();

	auto data = std::map<int, int>({ { 103, 103 } });

	for (const auto& [left, right] : data) {
		int x = left + right;
	}
}
*/