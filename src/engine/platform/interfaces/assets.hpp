#pragma once

namespace platform {

    class assets {
    public:
        struct source {
            std::string path;
            std::istream* stream;
        };

        virtual void init(void *ref) { /*NULL*/ }

        virtual std::vector<std::string> list(std::string path) = 0;

        virtual std::istream& retrieve(std::string path) = 0;

        virtual void release() = 0;

        type::font& font(std::string id) {
            static type::font empty;
            if (fonts.find(id) == fonts.end()) {
                return empty;
            }
            return(fonts[id]);
        }

        type::program& shader(std::string id) {
            static type::program empty;
            if (shaders.find(id) == shaders.end()) {
                return empty;
            }
            return(shaders[id]);
        }

        type::audio& sound(std::string id) {
            static type::audio empty;
            if (sounds.find(id) == sounds.end()) {
                return empty;
            }
            return(sounds[id]);
        }

        type::object& object(std::string id) {
            static type::object empty;
            if (objects.find(id) == objects.end()) {
                return empty;
            }
            return(objects[id]);
        }

        type::entity& entity(std::string id) {
            static type::entity empty;
            if (entities.find(id) == entities.end()) {
                return empty;
            }
            return(entities[id]);
        }

        std::map<std::string, type::font> fonts;
        std::map<std::string, type::program> shaders;
        std::map<std::string, type::audio> sounds;
        std::map<std::string, type::object> objects;
        std::map<std::string, type::entity> entities;

        virtual bool load(std::string type, std::string resource, std::string id) = 0;

        class common {
        public:
            virtual bool load(platform::assets *, std::string type, std::string resource, std::string id) = 0;
        };

    protected:
        common* loader = NULL;

        std::vector<assets::source> stack;
    };

}
