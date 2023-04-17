/*
================================================================================
  Copyright (c) 2023, Pandemos
  All rights reserved.
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  * Neither the name of the organization nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
================================================================================
*/

#pragma once

namespace platform {

    class network : public properties {
    public:
        virtual std::string hostname() = 0;
        virtual std::string ip(std::string hostname="") = 0;
        virtual std::string mac() = 0;
        virtual unsigned long pid() = 0;

        class server; // forward declare so the client can have a reference to the server

        class client {
        public:
            server* parent;

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

            typedef void(*callback)();

            std::vector<callback> callbacks;

            virtual void handler(callback c) {
                callbacks.push_back(c);
            }
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

            typedef void(*callback)(client* caller);

            std::vector<callback> callbacks;

            virtual void handler(callback c) {
                callbacks.push_back(c);
            }

            std::mutex connection;

            std::map<std::string, client*> clients;
        };
    };
}
