#pragma once

#define __PLATFORM_WINDOWS 1

namespace implementation {

    namespace windows {

        class filesystem : public platform::filesystem {
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

            std::vector<std::string> read_directory(std::string path);

            bool is_directory(std::string path);
        };

        class assets : public platform::assets {
        public:
            void init(void* ref);

            std::vector<std::string> list(std::string path);

            std::istream& retrieve(std::string path);

            void release();

            std::string base;
        };

        class network : public platform::network {
        public:
            std::string hostname();
            std::string ip(std::string hostname="");
            std::string mac();
            unsigned long pid();

            class client : public platform::network::client {
            public:
                client();
                virtual ~client();

                virtual void connect();

                SOCKET connection;
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
