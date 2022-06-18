#pragma once

#include "pugixml/src/pugixml.hpp"

namespace format {

    class fnt : virtual public type::font {
    public:
        fnt() : type::info({ { "fnt" }, type::format::FORMAT_FNT }) {
            /* NULL */
        }

        fnt(std::string filename) : fnt() {
            std::ifstream file(filename, std::ios::binary);
            if (file.is_open()) {
                file >> *this;
            }
        }

        operator type::font& () {
            return(*this);
        }

        friend type::font& operator>>(std::istream& input, format::fnt& instance) {
            /// ignore any non-good stream states
            if (input.good() == false) {
                assets->release();
                return instance;
            }

            /// Preallocate the glyphs
            instance.glyphs.resize(256);

            /// Load the stream into a string
            std::string str(std::istreambuf_iterator<char>(input), {});

            pugi::xml_document doc;
            if (auto status = doc.load_string(str.c_str())) {

                auto info = doc.select_node("/font/info");

                // face="Arial" size="32" bold="0" italic="0" charset="" unicode="1" stretchH="100" smooth="1" aa="1" padding="0,0,0,0" spacing="1,1" outline="0"

                instance.name = info.node().attribute("face").value();
                instance.size = info.node().attribute("size").as_int();

                instance.bold = info.node().attribute("bold").value() == "1";
                instance.italic = info.node().attribute("italic").value() == "1";

                auto padding = utilities::tokenize(info.node().attribute("padding").value(), ",");
                instance.padding_top = utilities::type_cast<int>(padding[0]);
                instance.padding_left = utilities::type_cast<int>(padding[1]);
                instance.padding_bottom = utilities::type_cast<int>(padding[2]);
                instance.padding_right = utilities::type_cast<int>(padding[3]);

                auto spacing = utilities::tokenize(info.node().attribute("spacing").value(), ",");
                instance.spacing_left = utilities::type_cast<int>(spacing[0]);
                instance.spacing_right = utilities::type_cast<int>(spacing[1]);

                auto kernings = doc.select_nodes("/font/kernings/kerning");
                for (auto kerning : kernings) {
                    format::fnt::kerning details;

                    // Map XML attributes to struct members
                    details.left = kerning.node().attribute("first").as_int();
                    details.right = kerning.node().attribute("second").as_int();
                    details.amount = kerning.node().attribute("amount").as_int();

                    instance.kernings.push_back(details);
                }

                auto pages = doc.select_nodes("/font/pages/page");
                for (auto page : pages) {
                    int index = page.node().attribute("id").as_int();
                    auto file = page.node().attribute("file").value();

                    type::image texture;
                    assets->retrieve(file) >> format::parser::png >> texture;

                    instance.pages.resize(index + 1);
                    instance.pages[index] = texture;
                }

                auto characters = doc.select_nodes("/font/chars/char");
                for (auto character : characters) {
                    format::fnt::glyph details;

                    // Map XML attributes to struct members
                    details.identifier = character.node().attribute("id").as_int();
                    details.x = character.node().attribute("x").as_int();
                    details.y = character.node().attribute("y").as_int();
                    details.width = character.node().attribute("width").as_int();
                    details.height = character.node().attribute("height").as_int();
                    details.xoffset = character.node().attribute("xoffset").as_int();
                    details.yoffset = character.node().attribute("yoffset").as_int();
                    details.xadvance = character.node().attribute("xadvance").as_int();
                    details.page = character.node().attribute("page").as_int();
                    details.channel = character.node().attribute("chnl").as_int();

                    details.quad = spatial::quad(details.width, details.height);
                    details.quad.texture.color = &instance.pages[details.page];
                    details.quad.xy_projection(details.x, details.y, details.width, details.height);

                    //details.quad.id = character.node().attribute("id").value();

                    instance.glyphs[details.identifier] = details;

                    if (details.height > instance.glyph_height) {
                        instance.glyph_height = details.height;
                    }
                    if (details.width > instance.glyph_width) {
                        instance.glyph_width = details.width;
                    }
                }

            }

            // Done parsing the incoming asset
            assets->release();

            return instance;
        }
    };

    namespace parser {
        inline format::fnt fnt;
    }
}
