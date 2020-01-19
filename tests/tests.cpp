#include "tests.hpp"

std::string testDataPath(std::string subpath) {
    std::string path = SOLUTION_PATH;

    if (subpath.empty() == false) {
#if defined __PLATFORM_LINUX
        return(path + "tests/data/" + subpath + "/");
#endif
#if defined __PLATFORM_WINDOWS
        return(path + "tests\\data\\" + subpath + "\\");
#endif
    }
    else {
#if defined __PLATFORM_LINUX
        return(path + "tests/data/");
#endif
#if defined __PLATFORM_WINDOWS
        return(path + "tests\\data\\");
#endif
    }
}