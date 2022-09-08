#pragma once

namespace platform {

    class filesystem : public properties {
    public:
        virtual std::string seperator() = 0;

        virtual bool rm(std::string filename) = 0;
        virtual bool mv(std::string src, std::string dest) = 0;
        virtual bool cp(std::string src, std::string dest) = 0;

        virtual bool mkpath(const std::string& path, unsigned int mask = 0766) {
            auto current = path;
            std::list<std::string> directories;
            while(current.empty() == false && is_directory(current) == false) {
                directories.push_front(basename(current));
                current = dirname(current);
            }
            for (auto directory : directories) {
                auto create = join({ current, directory });
                if (mkdir(create, mask) == false) {
                    return false;
                }
                current = create;
            }
            return true;
        }

        virtual bool mkdir(std::string path, unsigned int mask = 0) = 0;
        virtual bool rmdir(std::string path) = 0;

        virtual std::string pwd(std::string path = "") = 0;

        virtual std::vector<unsigned long> stat(std::string path) = 0;
        virtual std::vector<unsigned long> lstat(std::string path) = 0;

        virtual bool exists(std::string path) = 0;

        virtual std::string filetype(std::string path) = 0;

        virtual std::pair<int, std::string> error() = 0;

        virtual std::vector<std::string> read_directory(std::string path, bool hidden=false) = 0;

        virtual bool is_directory(std::string path) = 0;

        virtual std::string join(std::vector<std::string> arguments) = 0;

        virtual std::string dirname(const std::string& path) = 0;
        virtual std::string basename(const std::string& path) = 0;

        virtual std::string home(const std::string& path = "") = 0;
        virtual std::string appdata(const std::string& path = "") = 0;

    protected:
        std::string _home;
        std::string _appdata;
    };

}
