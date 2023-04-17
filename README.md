# Application Template 1.0

A simple 3d application framework.  Currently supports windows and android, with linux, macos and ios currently being worked on.  The [public](https://github.com/shizgnit/public) repository has example applications using the framework.

## Build

The two primary build systems are Visual Studio and XCode.

The following components are recommended for Visual Studio.

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

Bitmat fonts are created with the BMFont tool.

    https://www.angelcode.com/products/bmfont/

## Contributing

Currently just moving to gitflow for developer collaboration.  No expectations that there will be any, but having the workflow in place now will make it easier if the project ever grows.

## Notes

Recommended references.

    https://www.amazon.com/s?k=978-1435458864 - Mathematics for 3D Game Programming and Computer Graphics
    https://www.amazon.com/s?k=978-0321637635 - OpenGL Shading Language

Recommended sites.

    http://www.songho.ca/opengl/gl_projectionmatrix.html
    https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
    http://web.archive.org/web/20130416194336/http://olivers.posterous.com/linear-depth-in-glsl-for-real
    https://stackoverflow.com/questions/6652253/getting-the-true-z-value-from-the-depth-buffer
    https://stackoverflow.com/questions/48288154/pack-depth-information-in-a-rgba-texture-using-mediump-precison

Android

    adb tcpip 5556
    adb connect 192.168.0.177:5556
    adb devices

Gitflow

    Standard workflow

        git clone <project>
        git checkout -b feature/<name>
        ... Make changes
        git commit -am "did stuff"
        git pull origin develop
        git push
        ... Create a PR to develop

    Just helpful

        git log --pretty=format:"%h%x09%an%x09%ad%x09%s"
