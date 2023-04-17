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

            std::vector<std::string> list(const std::string& path, const std::string& type="");

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


