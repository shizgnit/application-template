#pragma once

namespace type {

    class font : virtual public type::info {
    public:
        class glyph {
        public:
            int identifier;

            int x;
            int y;

            int width;
            int height;

            int xoffset;
            int yoffset;

            int xadvance;

            int page;
            int channel;

            type::object quad;
        };

        class kerning {
        public:
            int left;
            int right;
            int amount;
        };

        std::vector<image> pages;
        std::vector<glyph> glyphs;
        std::vector<kerning> kernings;

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