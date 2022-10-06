#pragma once

namespace type {

    class font : virtual public type::info {
    public:
        typedef spatial::vector::type_t type_t;

        class glyph {
        public:
            int identifier;

            int x;
            int y;

            type_t width;
            type_t height;

            type_t xoffset;
            type_t yoffset;

            type_t xadvance;

            int page;
            int channel;

            type::object quad;
        };

        class kerning {
        public:
            int left;
            int right;
            type_t amount;
        };

        std::vector<image> pages;
        std::vector<glyph> glyphs;
        std::vector<kerning> kernings;

        void scale(type_t scale) {
            for(auto& glyph: glyphs) {
                glyph.width *= scale;
                glyph.height *= scale;
                
                glyph.xoffset *= scale;
                glyph.yoffset *= scale;
                
                glyph.xadvance *= scale;
                
                glyph.quad = glyph.quad.scale(spatial::matrix().scale(scale));
            }
            
            for(auto& kerning: kernings) {
                kerning.amount *= scale;
            }
        }
        
        font& operator=(const font& ref) {
            type::info::operator=(ref);
            
            pages.resize(ref.pages.size());
            for(int i=0; i<ref.pages.size(); i++) {
                pages[i] = ref.pages[i];
            }
            
            glyphs.resize(ref.glyphs.size());
            for(int i=0; i<ref.glyphs.size(); i++) {
                glyphs[i] = ref.glyphs[i];
                glyphs[i].quad.texture.color = &pages[glyphs[i].page];
            }
            
            kernings.resize(ref.kernings.size());
            for(int i=0; i<ref.kernings.size(); i++) {
                kernings[i] = ref.kernings[i];
            }
            
            identifier = ref.identifier;

            name = ref.name;

            size = ref.size;

            bold = ref.bold;
            italic = ref.italic;

            padding_top = ref.padding_top;
            padding_left = ref.padding_left;
            padding_bottom = ref.padding_bottom;
            padding_right = ref.padding_right;

            spacing_left = ref.spacing_left;
            spacing_right = ref.spacing_right;

            glyph_height = ref.glyph_height;
            glyph_width = ref.glyph_width;

            line_leading = ref.line_leading;
            
            return *this;
        }
        
        
        int kern(int left, int right) {
            for (auto kern : kernings) {
                if (kern.left == left && kern.right == right) {
                    return(kern.amount);
                }
            }
            return(0);
        }

        int height() {
            return glyph_height;
        }
        int width() {
            return glyph_width;
        }

        int leading() {
            if (line_leading == 0) {
                // set to 120% by default
                line_leading = glyph_height + (glyph_height / 5);
            }

            return line_leading;
        }

        int point() {
            return size;
        }

        std::string type() {
            return "type::font";
        }

        bool empty() {
            return glyphs.empty();
        }

    protected:

        int identifier;

        std::string name;

        int size;

        char bold;
        char italic;

        int padding_top;
        int padding_left;
        int padding_bottom;
        int padding_right;

        int spacing_left;
        int spacing_right;

        int glyph_height = 0;
        int glyph_width = 0;

        int line_leading = 0;

        friend type::font& operator>>(type::font& input, type::font& instance) {
            instance = input;
            return instance;
        }
    };

}
