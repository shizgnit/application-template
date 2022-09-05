#pragma once

class application {
public:
    virtual void on_startup() {};
    virtual void on_resize() {};
    virtual void on_draw() {};
    virtual void on_interval() {};

    application* dimensions(int width, int height) {
        this->width = width;
        this->height = height;
        this->on_resize();
        return(this);
    }

    bool started = false;

    int getWidth() const {
        return width;
    }
    int getHeight() const {
        return height;
    }

protected:

    char* resources;

    int width;
    int height;
};
