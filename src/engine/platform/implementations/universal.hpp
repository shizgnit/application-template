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

#pragma once

#define __PLATFORM_UNIVERSAL 1

namespace implementation {

    namespace universal {

        class input : public platform::input {
        public:
            void raise(const event& ev);
            void emit();

            std::string printable(int vkey) {
                return platform::keys[16].pressed || platform::keys[160].pressed ? platform::keys[vkey].meta : platform::keys[vkey].character;
            }

        protected:
            int active_pointer();
            int active_pointer(const event& ev);

            void on_press(const event& ev);
            void on_release(const event& ev);
            void on_move(const event& ev);

            void on_key_up(const event& ev);
            void on_key_down(const event& ev);

            void on_button_up(const event& ev);
            void on_button_down(const event& ev);

            bool drag;

            std::mutex tracking;

            std::vector<spatial::vector> points;
        };

        class interface : public platform::interface {
        public:
            bool raise(const input::event& ev, int x, int y);
            void emit();

            void position();
            widget* reposition(std::vector<widget*>& c);

            void draw();

            widget* create(std::vector<widget*>& c);
            widget* create(widget::spec t, int w, int h, int r, int g, int b, int a);
            widget* create(widget* instance, int w, int h, int r, int g, int b, int a);

            void print(int x, int y, const std::string& text);

        protected:
            void draw(widget& instance);
            void position(widget& instance);
        };

        class assets : public platform::assets::common {
        public:
            std::string load(platform::assets*, const std::string& type, const std::string& resource, const std::string& id);
        };

    }
}
