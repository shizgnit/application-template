#pragma once

namespace platform {

    class filesystem {
    public:
        virtual bool rm(std::string filename) = 0;
        virtual bool mv(std::string src, std::string dest) = 0;
        virtual bool cp(std::string src, std::string dest) = 0;

        virtual bool mkdir(std::string path, std::string mask) = 0;
        virtual bool rmdir(std::string path) = 0;

        virtual std::string pwd(std::string path = "") = 0;

        virtual std::vector<unsigned long> stat(std::string path) = 0;
        virtual std::vector<unsigned long> lstat(std::string path) = 0;

        virtual bool exists(std::string path) = 0;

        virtual std::string filetype(std::string path) = 0;

        virtual std::pair<int, std::string> error() = 0;

        virtual std::vector<std::string> read_directory(std::string path) = 0;

        virtual bool is_directory(std::string path) = 0;
    };

}
