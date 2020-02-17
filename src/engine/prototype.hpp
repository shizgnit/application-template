#pragma once

class application {
public:
    void on_startup();
    void on_resize();
    void on_draw();

    void on_proc();

    void on_press(float x, float y);
    void on_release(float x, float y);
    void on_drag(float x, float y);
    void on_scale(float x, float y, float z);
    void on_zoom_in();
    void on_zoom_out();

    void on_key_down(int key);
    void on_key_up(int key);

    void on_move(long int x, long int y);

    application* dimensions(int width, int height) {
        this->width = width;
        this->height = height;
        return(this);
    }
    
    bool started = false;

private:
    char* resources;

    int width;
    int height;
};


