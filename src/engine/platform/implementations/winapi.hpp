#pragma once

#include "engine.hpp"

namespace winapi {
    /*
    class asset : public asset_interface {
    public:
        void init(void* instance);
        void search(std::string path);
        std::buffer retrieve(std::string request, std::string path = "");
    };
    */

    class filesystem : public interfaces::filesystem {
    public:
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
