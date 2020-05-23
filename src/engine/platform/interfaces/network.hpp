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

            virtual void connect() = 0;

            void send(std::vector<char>& bytes) {
                sending.lock();
                bytes_out += bytes.size();
                output.swap(bytes);
                pending = true;
                sending.unlock();
            }

            void receive(std::vector<char>& bytes) {
                receiving.lock();
                bytes_in += bytes.size();
                input.swap(bytes);
                receiving.unlock();
            }

            unsigned int bytes_in = 0;
            unsigned int bytes_out = 0;

            pthread_t thread;

            std::vector<char> input;
            std::vector<char> output;

            bool pending;

            std::mutex sending;
            std::mutex receiving;
        };

        class server {
        public:
            server() {}
            virtual ~server() {}
            virtual void start() = 0;
            virtual void stop() = 0;

            pthread_t thread;

            client* add(const std::string& address, client* instance) {
                connection.lock();
                clients[address] = instance; // instantiated in the derived class
                connection.unlock();
                return(clients[address]);
            }

            void remove(const std::string& address) {
                connection.lock();
                if (clients.find(address) != clients.end()) {
                    delete clients[address];
                    clients.erase(address);
                }
                connection.unlock();
            }

            std::mutex connection;

            std::map<std::string, client*> clients;
        };
    };
}