#pragma once

#include "engine.hpp"

namespace posix {

    class filesystem : public interfaces::filesystem {
    public:
        filesystem() {}

        bool rm(std::string filename);
        bool mv(std::string src, std::string dest);
        bool cp(std::string src, std::string dest);

        bool mkdir(std::string path, std::string mask);
        bool rmdir(std::string path);

        std::string pwd(std::string path = "");

        std::vector<unsigned long> stat(std::string path);
        std::vector<unsigned long> lstat(std::string path);

        bool exists(std::string path);

        std::string filetype(std::string path);

        std::pair<int, std::string> error();
    };

}


