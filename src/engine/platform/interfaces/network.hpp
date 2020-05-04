#pragma once

namespace platform {
    class network {
    public:
        virtual std::string hostname() = 0;
        virtual std::string ip(std::string hostname="") = 0;
        virtual std::string mac() = 0;
        virtual unsigned long pid() = 0;

        class client {
        public:
            client() {}
            virtual ~client() {}
            virtual void start() = 0;
        };

        class server {
        public:
            server() {}
            virtual ~server() {}
            virtual void start() = 0;
        };
    };
}