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

namespace type {

    class rig : virtual public type::info, public properties {
    public:
        typedef spatial::vector::type_t type_t;

        class bone {
        public:
            bone(rig* ref, spatial::position* association) {
                parent = ref;
                position = association;
            }

            spatial::position* position=NULL;

            bone& operator=(const bone& ref) {
                this->id = ref.id;
                return *this;
            }

            bool operator==(const bone& ref) {
                return id == ref.id;
            }

            void adjust(const spatial::position& rotation, const spatial::vector& movement) {
                parent->adjust(*this, rotation, movement);
            }

            float max() {
                return parent && parent->has("height") ? std::get<double>(parent->get("height")) : 0.0;
            }

        protected:
            int assign() {
                static int increment = 0;
                return ++increment;
            }

            int id = assign();
            rig* parent = NULL;
        };

        bone& add(spatial::position* association) {
            bones.push_back(bone(this, association));
            return bones.back();
        }

        void adjust(bone& ref, const spatial::position& rotation, const spatial::vector& movement) {
            std::list<bone>::iterator it = std::find(bones.begin(), bones.end(), ref);
            if (it != bones.end()) {
                adjust(ref, it, rotation, movement);
            }
        }

    protected:
        std::list<bone> bones;
        void adjust(bone &ref, std::list<bone>::iterator &it, const spatial::position& rotation, const spatial::vector& movement) {
            auto reference = it->position->eye;
            it->position->reposition(movement);
            it->position->rotate(rotation);
            ++it;
            if (it != bones.end()) {
                adjust(ref, it, rotation, it->position->eye + (movement - reference));
            }
        }
    };

}
