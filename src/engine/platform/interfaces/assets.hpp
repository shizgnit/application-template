#pragma once

namespace platform {

    class assets : public properties {
    public:
        assets() {};
        virtual ~assets() {
            for (auto type : cache) {
                std::vector<std::string> ids;
                for (auto entry : type.second) {
                    ids.push_back(entry.first);
                }
                for (auto id : ids) {
                    delete type.second[id];
                    type.second.erase(id);
                }
            }
        }

        virtual void search(std::string type, std::string path) {
            paths[type] = path;
        }

        struct source {
            std::string path;
            std::istream* stream;
        };

        virtual std::string id(std::vector<std::string> parts) {
            return utilities::join("/", parts);
        }

        virtual std::string resolve(const std::string& path) {
            std::vector<std::string> directories;
            for (auto entry : stack) {
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

        virtual std::vector<std::string> list(const std::string& path) = 0;

        virtual std::istream& retrieve(const std::string& path) = 0;

        virtual void release() = 0;

        typedef void (assets::* callback)();
        utilities::scoped<assets*, callback> traverse(const source& node) {
            stack.push_back(node);
            return utilities::scoped<assets*, callback>(this, &assets::release);
        }

        virtual std::string load(const std::string& type, const std::string& resource, const std::string& id="") = 0;

        template<typename T> bool has(const std::string& id) {
            auto type = T().type();
            return cache.find(type) != cache.end() && cache[type].find(id) != cache[type].end();
        }

        template<typename T> void release(const std::string& id) {
            auto type = T().type();
            if (has<T>(id)) {
                delete cache[type][id];
                cache[type].erase(id);
            }
        }

        template<typename T> T& create(const std::string& id) {
            auto type = T().type();
            release<T>(id);
            cache[type][id] = new T;
            cache[type][id]->id(id);
            return *dynamic_cast<T*>(cache[type][id]);
        }

        template<typename T> T& get(const std::string& id) {
            auto type = T().type();
            return has<T>(id) ? *dynamic_cast<T*>(cache[type][id]) : create<T>(id);
        }

        template<typename T> std::vector<T*> get() {
            auto type = T().type();

            std::vector<T*> results;
            for (auto entry : cache[type]) {
                results.push_back(dynamic_cast<T*>(entry.second));
            }

            return results;
        }

        class common : public properties {
        public:
            virtual std::string load(platform::assets *, const std::string& type, const std::string& resource, const std::string& id) = 0;
        };

    protected:
        std::map<std::string, std::string> paths;

        std::map<std::string, std::map<std::string, type::info*>> cache;

        common* loader = NULL;

        std::vector<assets::source> stack;
    };

}
