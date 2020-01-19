#pragma once

#define __PLATFORM_SUPPORTS_OPENGL 1

#if defined __PLATFORM_WINDOWS
#define GLEW_STATIC
#include <GL\glew.h>
#include <GL\gl.h>
#include <GL\glu.h>
#endif

#if defined __PLATFORM_ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif