#pragma once

namespace type {

    enum format {
        FORMAT_FNT  = 0x01,
        FORMAT_FRAG = 0x02,
        FORMAT_MTL  = 0x03,
        FORMAT_OBJ  = 0x04,
        FORMAT_PNG  = 0x05,
        FORMAT_TGA  = 0x06,
        FORMAT_VERT = 0x07,
        FORMAT_WAV  = 0x08,
        FORMAT_FBX  = 0x09
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
        format _type;

        bool _compiled = false;

    public:
        info(std::vector<std::string> extensions, format type) {
            _extensions = extensions;
            _type = type;
        }

        type::format type() {
            return _type;
        }

        bool compile() {
            bool needed = _compiled == false;
            _compiled = true;
            return needed;
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