#pragma once

#define __PLATFORM_POSIX 1

namespace implementation {

    namespace posix {

        class filesystem : public platform::filesystem {
        public:
            std::string seperator();

            bool rm(std::string filename);
            bool mv(std::string src, std::string dest);
            bool cp(std::string src, std::string dest);

            bool mkdir(std::string path, unsigned int mask=0766);
            bool rmdir(std::string path);

            std::string pwd(std::string path = "");

            std::vector<unsigned long> stat(std::string path);
            std::vector<unsigned long> lstat(std::string path);

            bool exists(std::string path);

            std::string filetype(std::string path);

            std::pair<int, std::string> error();

            std::vector<std::string> read_directory(std::string path, bool hidden=false);

            bool is_directory(std::string path);

            std::string join(std::vector<std::string> arguments);

            std::string dirname(const std::string& path);
            std::string basename(const std::string& path);

            std::string home(const std::string& path = "");
            std::string appdata(const std::string& path = "");
        };

        class assets : public platform::assets {
        public:
            void init(void* ref=NULL);

            std::vector<std::string> list(const std::string& path);

            std::istream& retrieve(const std::string& path);

            void release();

            std::string load(const std::string& type, const std::string& resource, const std::string& id = "");

            std::string base;
        };

        class network : public platform::network {
        public:
            std::string hostname();
            std::string ip(std::string hostname = "");
            std::string mac();
            unsigned long pid();

            class client : public platform::network::client {
            public:
                client();
                virtual ~client();
                virtual void connect();
            };

            class server : public platform::network::server {
            public:
                server();
                virtual ~server();
                virtual void start();
                virtual void stop();
            };
        };

    }

}


