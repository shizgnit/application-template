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

#include <memory>

namespace platform {

    class assets : public properties {
    public:
        typedef std::string identifier_t;

        assets() {};
        virtual ~assets() {
            for (auto& type : _cache) {
                type.second.clear();
            }
        }

        struct source {
            std::string path;
            std::istream* stream;
        };

        virtual identifier_t id(std::vector<std::string> parts) {
            return utilities::join("/", parts);
        }

        virtual std::string resolve(const std::string& path) {
            std::vector<std::string> directories;
            for (auto entry : _stack) {
                if (entry.path.empty() == false) {
                    directories.push_back(entry.path);
                }
            }
            for (auto path : utilities::tokenize(path, "/")) {
                if (path == ".") {
                    // just ignore
                }
                else if (path == "..") {
                    directories.pop_back();
                }
                else {
                    directories.push_back(path);
                }
            }
            return utilities::join("/", directories);
        }

        virtual void init(void *ref=NULL) { /*NULL*/ }

        virtual std::vector<identifier_t> list(const std::string& path, const std::string& type="") = 0;

        virtual std::istream& retrieve(const std::string& path) = 0;

        virtual void release() = 0;

        typedef void (assets::* callback)();
        utilities::scoped<assets*, callback> traverse(const source& node) {
            _stack.push_back(node);
            return utilities::scoped<assets*, callback>(this, &assets::release);
        }

        virtual std::string load(const std::string& type, const std::string& resource, const identifier_t& id="") = 0;

        template<typename T> bool has(const identifier_t& id) {
            auto type = T().type();
            return _cache.find(type) != _cache.end() && _cache[type].find(id) != _cache[type].end();
        }

        template<typename T> void release(const identifier_t& id) {
            auto type = T().type();
            if (has<T>(id)) {
                _cache[type][id].reset();
                _cache[type].erase(id);
            }
        }

        template<typename T> std::shared_ptr<T> create(const identifier_t& id) {
            auto type = T().type();
            release<T>(id);
            _cache[type][id] = std::make_shared<T>();
            _cache[type][id]->id(id);
            return std::dynamic_pointer_cast<T>(_cache[type][id]);
        }

        template<typename T> std::shared_ptr<T> reference(const identifier_t& id) {
            auto type = T().type();
            return has<T>(id) ? std::dynamic_pointer_cast<T>(_cache[type][id]) : create<T>(id);
        }

        template<typename T> T& get(const identifier_t& id) {
            auto type = T().type();
            return has<T>(id) ? *dynamic_cast<T*>(_cache[type][id].get()) : *create<T>(id).get();
        }

        template<typename T> std::vector<std::shared_ptr<T>> get() {
            auto type = T().type();
            std::vector<std::shared_ptr<T>> results;
            for (auto& entry : _cache[type]) {
                results.push_back(std::dynamic_pointer_cast<T>(entry.second));
            }
            return results;
        }

        template<typename T> T& find(const value_t &criteria) {
            return T::find(criteria);
        }

        class common : public properties {
        public:
            virtual std::string load(platform::assets *, const std::string& type, const std::string& resource, const identifier_t& id) = 0;
        };

    protected:
        std::map<std::string, std::map<identifier_t, std::shared_ptr<type::info>>> _cache;

        common* _loader = NULL;

        std::vector<assets::source> _stack;

        std::string _base;
    };

}
