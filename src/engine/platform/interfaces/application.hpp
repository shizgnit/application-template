#pragma once

class application {
public:
    virtual void on_startup() = 0;
    virtual void on_resize() = 0;
    virtual void on_draw() = 0;
    virtual void on_interval() = 0;

    application* dimensions(int width, int height) {
        this->width = width;
        this->height = height;
        this->on_resize();
        return(this);
    }

    bool started = false;

protected:

    char* resources;

    int width;
    int height;
};
