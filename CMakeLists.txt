
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
        src/engine/platform/implementations/macos.cpp
        src/engine/platform/implementations/macos.hpp
        src/engine/platform/implementations/metal.cpp
        src/engine/platform/implementations/metal.hpp
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
    file(GLOB_RECURSE CLIENT_SOURCES
        src/client/android-client.NativeActivity/*.cpp
        src/client/android-client.NativeActivity/*.c
        src/client/android-client.NativeActivity/*.h
        src/client/android-client.NativeActivity/*.hpp
    )
    file(GLOB PLATFORM_SOURCES
        src/engine/platform/implementations/android.cpp
        src/engine/platform/implementations/android.hpp
        src/engine/platform/implementations/opensl.cpp
        src/engine/platform/implementations/opensl.hpp
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    file(GLOB_RECURSE CLIENT_SOURCES
        src/client/ios-client.Application/*.m
        src/client/ios-client.Application/*.h
        src/client/ios-client.Application/*.hpp
    )
    file(GLOB PLATFORM_SOURCES
        src/engine/platform/implementations/ios.cpp
        src/engine/platform/implementations/ios.hpp
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    file(GLOB_RECURSE CLIENT_SOURCES
        src/client/emscripten-client.Application/main.cpp
    )
    file(GLOB PLATFORM_SOURCES
        src/engine/platform/implementations/emscripten.cpp
        src/engine/platform/implementations/emscripten.hpp
    )
endif()

# Add all sources together
set(FRAMEWORK_SOURCES
    ${ENGINE_SOURCES}
    ${CLIENT_SOURCES}
    ${PLATFORM_SOURCES}
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

# Install rules (optional)
# install(TARGETS framework DESTINATION lib)
