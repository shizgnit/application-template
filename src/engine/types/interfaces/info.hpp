#pragma once

namespace type {

    enum format {
        FORMAT_FNT   = 0x01,
        FORMAT_FRAG  = 0x02,
        FORMAT_MTL   = 0x03,
        FORMAT_OBJ   = 0x04,
        FORMAT_PNG   = 0x05,
        FORMAT_TGA   = 0x06,
        FORMAT_VERT  = 0x07,
        FORMAT_WAV   = 0x08,
        FORMAT_FBX   = 0x09,
        FORMAT_METAL = 0X0A
    };

    class info {
    protected:
        virtual void define() {
            // Just leave the info blank for now
        };
        info() {
            define();
        }

        std::vector<std::string> _extensions;
        format _format;

        bool _compiled = false;

        std::string _id;

    public:
        struct opaque_t;
        opaque_t *resource = nullptr;

        info(std::vector<std::string> extensions, format spec) {
            _extensions = extensions;
            _format = spec;
        }

        std::string id(std::string id = "") {
            if (id.empty() == false) {
                _id = id;
            }
            return _id;
        }

        virtual std::string type() {
            return "type::undefined";
        }

        type::format format() {
            return _format;
        }

        bool compiled() {
            return _compiled;
        }

        bool compiled(bool flag) {
            return _compiled = flag;
        }

        virtual bool empty() {
            return true;
        };
    };

}
