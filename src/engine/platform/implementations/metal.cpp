#include "engine.hpp"

#if defined __PLATFORM_SUPPORTS_METAL

struct type::info::opaque_t {
    void *ptr;
};


void implementation::metal::graphics::projection(int fov) {
    ortho = spatial::matrix().ortho(0, display_width, 0, display_height);
    perspective = spatial::matrix().perspective(fov, (float)display_width / (float)display_height, 0.0f, 100.0f);
}

void implementation::metal::graphics::dimensions(int width, int height, float scale) {
    bool init = (display_width == 0 && display_height == 0);

    display_width = width;
    display_height = height;
}

void implementation::metal::graphics::init(void) {
    // TODO
}

void implementation::metal::graphics::clear(void) {
    // TODO
}

void implementation::metal::graphics::flush(void) {
    frames.push_back(frame);
    time_t now = time(NULL);
    if (timestamp != now) {
        stats total;
        for (auto frame : frames) {
            total.lines += frame.lines;
            total.triangles += frame.triangles;
            total.vertices += frame.vertices;
        }
        total.frames = frames.size();
        activity.push_back(total);
        if (activity.size() > 60) { // TODO: make this configurable
            activity.pop_front();
        }
        frames.clear();
    }
    frame.clear();
}

bool implementation::metal::graphics::compile(type::shader& shader) {
    if (shader.compile() == false) {
        return false;
    }

    // TODO

    return true;
}

bool implementation::metal::graphics::compile(type::program& program) {
    if (program.compile() == false) {
        return false;
    }

    if (compile(program.vertex) == false) {
        return false;
    }
    if (compile(program.fragment) == false) {
        return false;
    }

    // TODO
    
    return true;
}

bool implementation::metal::graphics::compile(type::material& material) {
    if (material.compile() == false) {
        return false;
    }
    if (material.color == NULL && material.normal == NULL) {
        return false;
    }

    if (material.color) {
        // TODO
    }

    if (material.normal) {
        // TODO
    }

    return true;
}

bool implementation::metal::graphics::compile(type::object& object) {
    for (auto &child : object.children) {
        compile(child);
    }
    if (object.compile() == false || object.vertices.size() == 0) {
        return false;
    }

    // TODO
    
    compile(object.texture);

    return true;
}

bool implementation::metal::graphics::compile(type::font& font) {
    if (font.compile() == false) {
        return false;
    }

    for (auto& glyph : font.glyphs) {
        if (glyph.identifier) {
            compile(glyph.quad);
        }
    }

    return true;
}

bool implementation::metal::graphics::compile(type::entity& entity) {
    entity.bake();

    // TODO

    if (entity.compile() == false) {
        return false;
    }

    for (auto& animation : entity.animations) {
        for (auto& frame : animation.second.frames) {
            compile(frame);
        }
    }

    return true;
}

bool implementation::metal::graphics::compile(platform::assets* assets) {
    for (auto program : assets->get<type::program>()) {
        compile(*program);
    }
    for (auto material : assets->get<type::material>()) {
        compile(*material);
    }
    for (auto object : assets->get<type::object>()) {
        compile(*object);
    }
    for (auto entity : assets->get<type::entity>()) {
        compile(*entity);
    }
    for (auto font : assets->get<type::font>()) {
        compile(*font);
    }
    return true;
}

void implementation::metal::graphics::draw(type::object& object, type::program& shader, const spatial::matrix& projection, const spatial::matrix& view, const spatial::matrix& model, const spatial::matrix& lighting, unsigned int options) {
    // TODO
}

void implementation::metal::graphics::draw(std::string text, type::font& font, type::program& shader, const spatial::matrix& projection, const spatial::matrix& view, const spatial::matrix& model, const spatial::matrix& lighting, unsigned int options) {
    int prior = 0;
    spatial::matrix position = model;
    for (unsigned int i = 0; i < text.length(); i++) {
        position.translate(spatial::vector((float)font.kern(prior, text[i]), 0.0f, 0.0f));
        spatial::matrix relative = position;

        auto& glyph = font.glyphs[text[i]];

        relative.translate(spatial::vector((float)glyph.xoffset, (float)(font.point() - glyph.height - glyph.yoffset), 0.0f));

        draw(glyph.quad, shader, projection, view, relative);
        position.translate(spatial::vector((float)glyph.xadvance, 0.0f, 0.0f));
        prior = text[i];
    }
}


void implementation::metal::graphics::ontarget(type::object& object) {
    // TODO
}

void implementation::metal::graphics::untarget() {
    // TODO
}

void implementation::metal::graphics::oninvert() {
    // TODO
}

void implementation::metal::graphics::uninvert() {
    // TODO
}


#endif


