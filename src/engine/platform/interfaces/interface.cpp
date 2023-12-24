/*
================================================================================
  Copyright (c) 2023, Dee E. Abbott
  All rights reserved.
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  * Neither the name of the organization nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
================================================================================
*/

#include "engine.hpp"

void platform::interface::button::draw() {
    graphics->clip(graphics->height() - y, -((graphics->height() - background.height()) - y), -x, x + background.width());

    spatial::matrix position;
    position.identity();
    position.translate(x, (graphics->height() - background.height()) - y, 0);

    graphics->draw(background, manager.get().shader, position, spatial::matrix(), manager.get().projection);
}

void platform::interface::textbox::draw() {
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

int platform::interface::tabbed::add(interface::widget& button, interface::widget& content) {
    children.push_back(button);
    crossreference[" "] = children.size();
    children.push_back(content);

    return 0;
}

void platform::interface::tabbed::select(const std::string label) {
    if (crossreference.find(label) != crossreference.end()) {
        selected = crossreference[label];
    }
}

void platform::interface::tabbed::draw() {
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
