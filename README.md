# Application Template 1.0

A simple 3d application framework targetting windows and android.  No specific goal in mind currently.

## Build

Use Microsoft Visual Studio 2022 and the build.sln solution file.  I have the following components installed.

* ASP.NET and web development
* Python development
* Desktop development with C++
    * MSVC v143
    * Windows 10 SDK
    * Test Adapter for Google Test
* Universal Windows Platform development
* Mobile development with C++ ()
    * Android NDK
    * Apache Ant
    * C++ Android development tools
* Game development with C++
* Linux development with C++

## Dependencies

Submodules.

    git submodule update --init --recursive
    (includes; loadpng, pugixml, picojson, zstream)

The following projects need to be installed on the client PC running the IDE.

    https://nodejs.org/
    https://developer.android.com/studio

Prebuilt package of deps/miniz-2.2.0 installed.  Currently just added to the repo directly (pulled from the following).

    https://github.com/richgel999/miniz/releases

Bitmat fonts are created with the BMFont tool.

    https://www.angelcode.com/products/bmfont/

## Contributing

Currently just moving to gitflow for developer collaboration.  No expectations that there will be any, but having the workflow in place now will make it easier if the project ever grows.

## Notes

Work is being tracked on a Kanban board.

    https://trello.com/b/wMOMrSaL/application-template

Recommended references.

    https://www.amazon.com/s?k=978-1435458864 - Mathematics for 3D Game Programming and Computer Graphics
    https://www.amazon.com/s?k=978-0321637635 - OpenGL Shading Language

