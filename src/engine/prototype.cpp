#include "engine.hpp"

inline format::wav sound;

inline type::object icon;

void application::on_startup() {

    graphics->init();
    audio->init();

    assets->retrieve("raw/glados.wav") >> sound;

    assets->retrieve("raw/marvin.png") >> format::parser::png >> icon.texture.map;

    audio->compile(sound);
}

void application::on_resize() {
    glViewport(0, 0, width, height);
}

const GLfloat triangleVertices[] = {
        0.0f, 1.0f,
        -1.0f, -1.0f,
        1.0f, -1.0f
};

void application::on_draw() {
    glClear(GL_COLOR_BUFFER_BIT);


    glFlush();
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


/// Just some tests for newer syntax on the different compilers
auto test_return() {
    return std::pair<int, int>({ 2, 10 });
}

void test_capabilities() {
    int something = 2;
    auto [foo, bar] = test_return();

    auto data = std::map<int, int>({ { 103, 103 } });

    for (const auto& [left, right] : data) {
        int x = left + right;
    }
}
