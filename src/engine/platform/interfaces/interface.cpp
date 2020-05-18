#include "engine.hpp"

void interface::button::draw() {
    graphics->clip(graphics->height() - y, -((graphics->height() - background.height()) - y), -x, x + background.width());

    spatial::matrix position;
    position.identity();
    position.translate(x, (graphics->height() - background.height()) - y, 0);

    graphics->draw(background, manager.get().shader, position, spatial::matrix(), manager.get().projection);
}

void interface::textbox::draw() {
    graphics->clip(graphics->height() - y, -((graphics->height() - background.height()) - y), -x, x + background.width());

    spatial::matrix position;
    position.identity();
    position.translate(x, (graphics->height() - background.height()) - y, 0);

    graphics->draw(background, manager.get().shader, position, spatial::matrix(), manager.get().projection);

    int line_x = x;
    int line_y = y;

    auto lines = content.get();
    int line = y + (manager.get().font.leading() * lines.size());
    for (auto text : lines) {
        line_y += manager.get().font.leading();

        spatial::matrix position;
        position.identity();
        position.translate(line_x, (graphics->height() - manager.get().font.height()) - line_y, 0);

        graphics->draw(text, manager.get().font, manager.get().shader, position, spatial::matrix(), manager.get().projection);
    }

    graphics->noclip();
}

int interface::tabbed::add(interface::widget& button, interface::widget& content) {
    children.push_back(button);
    crossreference[" "] = children.size();
    children.push_back(content);

    return 0;
}

void interface::tabbed::select(const std::string label) {
    if (crossreference.find(label) != crossreference.end()) {
        selected = crossreference[label];
    }
}

void interface::tabbed::draw() {
    // Draw the buttons
    for (int i = 0; i < children.size(); i += 2) {
        children[selected].get().draw();
    }
    // Nothing selected... just bail
    if (selected <= 0) {
        return;
    }
    // Draw the selection
    children[selected].get().draw();
}
