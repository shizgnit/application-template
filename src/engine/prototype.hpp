#pragma once

class prototype: public application {
public:
    void on_startup();
    void on_resize();
    void on_draw();
    void on_interval();

public:
    void setup_input();
};
