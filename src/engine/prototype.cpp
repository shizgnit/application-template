#include "engine.hpp"

inline type::audio sound;

inline type::object icon;

inline spatial::position camera;

inline type::program shader;

inline spatial::matrix ortho;


bool init = false;

void application::on_startup() {

    graphics->init();
    audio->init();

    assets->retrieve("raw/glados.wav") >> format::parser::wav >> sound;

    /// Load up the shaders
    assets->retrieve("shaders/shader_basic.vert") >> format::parser::vert >> shader.vertex;
    assets->retrieve("shaders/shader_basic.frag") >> format::parser::frag >> shader.fragment;
    graphics->compile(shader);

    /// Get the icon ready for drawing
    assets->retrieve("drawable/marvin.png") >> format::parser::png >> icon.texture.map;
    icon.quad(295, 281);
    icon.xy_projection(0, 0, 295, 281);
    graphics->compile(icon);

    camera.move(4.0f);

    audio->compile(sound);

    init = true;
}

void application::on_resize() {
    glViewport(0, 0, width, height);
    ortho.ortho(0, width, 0, height);
}

void application::on_draw() {
    graphics->clear();

    spatial::matrix frame;
    frame.identity();
    frame.translate(10, 10, 0);

    graphics->draw(icon, shader, frame, spatial::matrix(), ortho);

    graphics->flush();
}

void application::on_proc() {}

void application::on_touch_press(float x, float y) {}
void application::on_touch_release(float x, float y) {
    audio->play(sound);
}
void application::on_touch_drag(float x, float y) {}
void application::on_touch_scale(float x, float y, float z) {}
void application::on_touch_zoom_in() {}
void application::on_touch_zoom_out() {}

void application::on_key_down(int key) {}
void application::on_key_up(int key) {}

void application::on_mouse_move(long int x, long int y) {}
