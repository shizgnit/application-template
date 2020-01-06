#include "tests.hpp"

std::string testDataPath(std::string subpath) {
    std::string path = SOLUTION_PATH;
#if defined __PLATFORM_LINUX
    return(path + "tests/data/" + subpath + "/");
#endif
#if defined __PLATFORM_WINDOWS
    return(path + "tests\\data\\" + subpath + "\\");
#endif
}