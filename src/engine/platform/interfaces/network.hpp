#pragma once

namespace platform {
    class network {
    public:
        virtual std::string hostname() = 0;
        virtual std::string ip(std::string hostname="") = 0;
        virtual std::string mac() = 0;
        virtual unsigned long pid() = 0;
    };
}