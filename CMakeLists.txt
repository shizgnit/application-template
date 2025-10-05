
cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(framework)

# Always include these directories
file(GLOB_RECURSE ENGINE_SOURCES
    src/engine/platform/interfaces/*.cpp
    src/engine/platform/interfaces/*.h
    src/engine/platform/interfaces/*.hpp
    src/engine/types/**/*.cpp
    src/engine/types/**/*.h
    src/engine/types/**/*.hpp
    src/engine/utilities/*.cpp
    src/engine/utilities/*.h
    src/engine/utilities/*.hpp
)

# Platform-specific sources
set(CLIENT_SOURCES)
set(PLATFORM_SOURCES)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    file(GLOB_RECURSE CLIENT_SOURCES
        src/client/windows-client.Executable/*.cpp
        src/client/windows-client.Executable/*.h
        src/client/windows-client.Executable/*.hpp
    )
    file(GLOB PLATFORM_SOURCES
        src/engine/platform/implementations/universal.cpp
        src/engine/platform/implementations/universal.hpp
        src/engine/platform/implementations/opengl.cpp
        src/engine/platform/implementations/opengl.hpp
        src/engine/platform/implementations/openal.cpp
        src/engine/platform/implementations/openal.hpp
        src/engine/platform/implementations/windows.cpp
        src/engine/platform/implementations/windows.hpp
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    file(GLOB_RECURSE CLIENT_SOURCES
        src/client/macos-client.Executable/*.m
        src/client/macos-client.Executable/*.h
        src/client/macos-client.Executable/*.hpp
    )
    file(GLOB PLATFORM_SOURCES
        src/engine/platform/implementations/universal.cpp
        src/engine/platform/implementations/universal.hpp
        src/engine/platform/implementations/opengl.cpp
        src/engine/platform/implementations/opengl.hpp
        src/engine/platform/implementations/macos.cpp
        src/engine/platform/implementations/macos.hpp
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
    file(GLOB_RECURSE CLIENT_SOURCES
        src/client/android-client.NativeActivity/*.cpp
        src/client/android-client.NativeActivity/*.c
        src/client/android-client.NativeActivity/*.h
        src/client/android-client.NativeActivity/*.hpp
    )
    file(GLOB PLATFORM_SOURCES
        src/engine/platform/implementations/universal.cpp
        src/engine/platform/implementations/universal.hpp
        src/engine/platform/implementations/opengl.cpp
        src/engine/platform/implementations/opengl.hpp
        src/engine/platform/implementations/opensl.cpp
        src/engine/platform/implementations/opensl.hpp
        src/engine/platform/implementations/android.cpp
        src/engine/platform/implementations/android.hpp
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    file(GLOB_RECURSE CLIENT_SOURCES
        src/client/ios-client.Application/*.m
        src/client/ios-client.Application/*.h
        src/client/ios-client.Application/*.hpp
    )
    file(GLOB PLATFORM_SOURCES
        src/engine/platform/implementations/universal.cpp
        src/engine/platform/implementations/universal.hpp
        src/engine/platform/implementations/opengl.cpp
        src/engine/platform/implementations/opengl.hpp
        src/engine/platform/implementations/ios.cpp
        src/engine/platform/implementations/ios.hpp
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    file(GLOB_RECURSE CLIENT_SOURCES
        src/client/emscripten-client.Application/main.cpp
    )
    file(GLOB PLATFORM_SOURCES
        src/engine/platform/implementations/universal.cpp
        src/engine/platform/implementations/universal.hpp
        src/engine/platform/implementations/opengl.cpp
        src/engine/platform/implementations/opengl.hpp
        src/engine/platform/implementations/openal.cpp
        src/engine/platform/implementations/openal.hpp
        src/engine/platform/implementations/posix.cpp
        src/engine/platform/implementations/posix.hpp
        src/engine/platform/implementations/emscripten.cpp
        src/engine/platform/implementations/emscripten.hpp
    )
    file(GLOB DEPENDENCY_SOURCES
        deps/lodepng/lodepng.cpp
        deps/lodepng/lodepng.h
        deps/pugixml/src/pugiconfig.cpp
        deps/pugixml/src/pugixml.cpp
        deps/pugixml/src/pugixml.hpp
        deps/miniz-2.2.0/miniz.c
        deps/miniz-2.2.0/miniz.h
    )
endif()

# Add all sources together
set(FRAMEWORK_SOURCES
    ${ENGINE_SOURCES}
    ${CLIENT_SOURCES}
    ${PLATFORM_SOURCES}
    ${DEPENDENCY_SOURCES}
)

add_library(framework STATIC ${FRAMEWORK_SOURCES})

# Find OpenGL
find_package(OpenGL REQUIRED)

# Link OpenGL to the executable
target_link_libraries(framework ${OPENGL_gl_LIBRARY})

# Add OpenGL include directories
target_include_directories(framework PRIVATE ${OPENGL_INCLUDE_DIR})


# Optionally, set include directories
target_include_directories(framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/engine ${CMAKE_CURRENT_SOURCE_DIR}/deps)

set_target_properties(framework PROPERTIES LINKER_LANGUAGE CXX)

set_target_properties(framework PROPERTIES LINK_FLAGS "-sFULL_ES3=1 -sMAX_WEBGL_VERSION=2")

# Install rules (optional)
# install(TARGETS framework DESTINATION lib)
