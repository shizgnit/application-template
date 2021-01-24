# Application Template 1.0

This project is both meant to be a practical framework for client/server infrastructure and a personal opportunity to learn modern C++ language/platform features.  In a corporate environment it is common that the following are true.

  * WAN access is only through a restricted proxy.  This can cause difficulties in cross project dependencies and prevent IDE capabilities from fully working.
  * Software platforms are often required to build/run on significantly older standards.  In my case only partial C++11 support is available (gcc443).
    https://gcc.gnu.org/projects/cxx-status.html#cxx11

Removing those limitations allows for modern solutions to be experimented with.  This project hosted on github is a synchronized repository from an on-prem gitlab instance being used for CI/CD, so not all aspects of the project will be visible.

## Build

Use Microsoft Visual Studio 2019 and the build.sln solution file.

## Dependencies

Submodules.

    git submodule update --init --recursive
    (includes; loadpng, pugixml, picojson, zstream)

The following projects need to be installed on the client PC running the IDE.

    https://nodejs.org/
    https://developer.android.com/studio

Bitmat fonts are created with the BMFont tool.

    https://www.angelcode.com/products/bmfont/

## Notes

Work is being tracked on a Kanban board.

    https://trello.com/b/wMOMrSaL/application-template

Recommended references.

    https://www.amazon.com/s?i=stripbooks&rh=p_66%3A1435458869
