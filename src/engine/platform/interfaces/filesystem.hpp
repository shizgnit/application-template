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

    class filesystem : public properties {
    public:
        virtual std::string seperator() = 0;

        virtual bool rm(const std::string& path) = 0;
        virtual bool mv(const std::string& src, const std::string& dst) = 0;
        virtual bool cp(const std::string& src, const std::string& dst) = 0;

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

        virtual bool mkdir(const std::string& path, unsigned int mask = 0) = 0;
        virtual bool rmdir(const std::string& path) = 0;

        virtual std::string pwd(const std::string& path = "") = 0;

        virtual std::vector<unsigned long> stat(const std::string& path) = 0;
        virtual std::vector<unsigned long> lstat(const std::string& path) = 0;

        virtual bool exists(const std::string& path) = 0;

        virtual std::string filetype(const std::string& path) = 0;

        virtual std::pair<int, std::string> error() = 0;

        virtual std::vector<std::string> read_directory(const std::string& path, bool hidden=false) = 0;

        virtual bool is_directory(const std::string& path) = 0;

        virtual std::string join(const std::vector<std::string>& arguments) = 0;

        virtual std::string dirname(const std::string& path) = 0;
        virtual std::string basename(const std::string& path) = 0;

        virtual std::string home(const std::string& path = "") = 0;
        virtual std::string appdata(const std::string& path = "") = 0;

        virtual std::string normalize_path(const std::string& path) {
            if (path.empty()) {
                return "";
            }
            std::string result = path;
            if (result[0] == '~') {
                result.replace(0, 1, home());
            }
            if (seperator() == "/") {
                result = utilities::replace(result, "\\", seperator());
            }
            if (seperator() == "\\") {
                result = utilities::replace(result, "/", seperator());
            }
            return result;
        }

    protected:
        std::string _home;
        std::string _appdata;
    };

}
