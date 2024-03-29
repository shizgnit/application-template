/*
================================================================================
  Copyright (c) 2023, Pandemos
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

#undef min
#undef max

namespace type {

    class entity;

    class object : virtual public type::info {
    public:
        unsigned int allocate() {
            static unsigned int i = 0;
            return ++i;
        }

        unsigned int instance = allocate();

        program* renderer = NULL;
        entity* emitter = NULL;

        object() : renderer(NULL), emitter(NULL) {}
        object(const spatial::geometry& ref) {
            *this = ref;
        }

        typedef spatial::vector::type_t type_t;

        spatial::geometry& interpolate(const spatial::matrix& model) {
            if (interpolated.vertices.size() == 0) {
                interpolated.vertices.reserve(vertices.size());
            }
            for (int i = 0; i < vertices.size(); i++) {
                interpolated.vertices[i] = model * vertices[i].coordinate;
            }
            return interpolated;
        }

        void uv_projection(bool wrap=true, bool spherical=false) {
            auto top = max();
            auto bottom = min();
            for (auto& vertex : vertices) {
                if (vertex.normal.z < -0.1) {
                    vertex.texture.z = 1.0 - (vertex.coordinate.x - bottom.x) / (top.x - bottom.x);
                }
                else if (vertex.normal.z > 0.1) {
                    vertex.texture.z = (vertex.coordinate.x - bottom.x) / (top.x - bottom.x);
                }
                else if (vertex.normal.x > 0.1) {
                    vertex.texture.z = 1.0 - (vertex.coordinate.z - bottom.z) / (top.z - bottom.z);
                }
                else {
                    vertex.texture.z = (vertex.coordinate.z - bottom.z) / (top.z - bottom.z);
                }
                vertex.texture.w = 1.0 - ((vertex.coordinate.y - bottom.y) / (top.y - bottom.y));
            }
        }

        void xy_projection(unsigned int x, unsigned int y, unsigned int width, unsigned int height, bool horizontal=false, bool vertical=false) {
            if (texture.color == NULL) {
                return;
            }

            type_t max_x = this->width();
            type_t max_y = this->height();

            type_t texture_dx = 1 / (type_t)texture.color->properties.width;
            type_t texture_dy = 1 / (type_t)texture.color->properties.height;

            for (unsigned int i = 0; i < vertices.size(); i++) {
                type_t dx = vertices[i].coordinate.x / max_x;
                type_t dy = (max_y - vertices[i].coordinate.y) / max_y;

                type_t tx = (width * dx + x) * texture_dx;
                type_t ty = (height * dy + y) * texture_dy;
                //float ty = 1.0f - ((height * dy + y) * texture_dy);

                if (horizontal) {
                    tx = 1.0 - tx;
                }
                if (vertical) {
                    ty = 1.0 - ty;
                }

                vertices[i].texture(tx, ty, 0.0f, 0.0f);
            }

            texture.compiled(false);
        }

        object* flat_normals() {
            int triangles = vertices.size() / 3;

            for (unsigned int i = 0; i < vertices.size() / 3; i++) {
                int index = i * 3;

                type_t u[3];
                type_t v[3];

                u[0] = vertices[index + 1].coordinate.x - vertices[index].coordinate.x;
                u[1] = vertices[index + 1].coordinate.y - vertices[index].coordinate.y;
                u[2] = vertices[index + 1].coordinate.z - vertices[index].coordinate.z;

                v[0] = vertices[index + 2].coordinate.x - vertices[index].coordinate.x;
                v[1] = vertices[index + 2].coordinate.y - vertices[index].coordinate.y;
                v[2] = vertices[index + 2].coordinate.z - vertices[index].coordinate.z;

                for (int j = index; j < index + 3; j++) {
                    vertices[j].normal.x = u[1] * v[2] - u[2] * v[1];
                    vertices[j].normal.y = u[2] * v[0] - u[0] * v[2];
                    vertices[j].normal.z = u[0] * v[1] - u[1] * v[0];
                    vertices[j].normal.w = 0.0f;
                }
            }

            return(NULL);
        }

        spatial::vector::type_t vertical_distance(const spatial::vector &v) {
            spatial::ray projection(v, v.y + 1.0f);
            return projection.intersection(vertices).length();
        }

        float width() {
            if (constraint.calculated == false) {
                calculate_constraints();
            }
            return constraint.max.x - constraint.min.x;
        }

        float height() {
            if (constraint.calculated == false) {
                calculate_constraints();
            }
            return constraint.max.y - constraint.min.y;
        }

        float length() {
            if (constraint.calculated == false) {
                calculate_constraints();
            }
            return constraint.max.z - constraint.min.z;
        }

        spatial::vector& min() {
            if (constraint.calculated == false) {
                calculate_constraints();
            }
            return constraint.min;
        }

        spatial::vector& max() {
            if (constraint.calculated == false) {
                calculate_constraints();
            }
            return constraint.max;
        }

        spatial::vector& center() {
            if (constraint.calculated == false) {
                calculate_constraints();
            }
            return constraint.center;
        }

        spatial::vector& dimensions() {
            if (constraint.calculated == false) {
                calculate_constraints();
            }
            return constraint.dimensions;
        }

        std::vector<spatial::vertex> vertices;
        
        spatial::geometry interpolated;

        type::material texture;

        unsigned int instances = 0;

        unsigned int flags = 0;

        bool visible = true;

        object& operator += (const object& ref) {
            for (auto vertex : ref.vertices) {
                vertices.push_back(vertex);
            }
            return *this;
        }

        object& operator = (const spatial::geometry& ref) {
            this->vertices.clear();
            std::copy(ref.vertices.begin(), ref.vertices.end(), std::back_inserter(this->vertices));
            constraint.calculated = false;
            compiled(false);
            if (ref.vertices.size() == 6 && texture.color) {
                xy_projection(0, 0, texture.color->properties.width, texture.color->properties.height);
            }
            return *this;
        }

        friend type::object& operator >> (type::object& input, type::object& instance) {
            // Only copy over a single object
            if (input.vertices.size()) {
                instance = input;
            }
            else {
                for (auto child : input.children) {
                    instance = child;
                    break;
                }
            }
            return instance;
        }

        friend std::vector<type::object>& operator >> (type::object& input, std::vector<type::object>& instance) {
            instance = input.children;
            return instance;
        }

        std::vector<object> children;
        type::object *icon = NULL;

        std::string type() {
            return "type::object";
        }

        bool empty() {
            return vertices.empty() && children.empty();
        }

        object& scale(spatial::matrix transform) {
            for (auto& vertex : vertices) {
                vertex.coordinate = transform * vertex.coordinate;
            }
            calculate_constraints();
            return *this;
        }

        object& reposition(spatial::vector position) {
            for (auto &vertex : vertices) {
                vertex.coordinate += position;
            }
            calculate_constraints();
            return *this;
        }

        bool depth = false;
        std::vector<unsigned char> pixels;

//    protected:
        void calculate_constraints() {
            if (vertices.size() == 0) {
                return;
            }

            constraint.min = vertices.front().coordinate;
            constraint.max = vertices.front().coordinate;

            for (auto vertex : vertices) {
                if (constraint.max.x < vertex.coordinate.x) {
                    constraint.max.x = vertex.coordinate.x;
                }
                if (constraint.max.y < vertex.coordinate.y) {
                    constraint.max.y = vertex.coordinate.y;
                }
                if (constraint.max.z < vertex.coordinate.z) {
                    constraint.max.z = vertex.coordinate.z;
                }
                if (constraint.min.x > vertex.coordinate.x) {
                    constraint.min.x = vertex.coordinate.x;
                }
                if (constraint.min.y > vertex.coordinate.y) {
                    constraint.min.y = vertex.coordinate.y;
                }
                if (constraint.min.z > vertex.coordinate.z) {
                    constraint.min.z = vertex.coordinate.z;
                }
            }
            constraint.dimensions.x = constraint.max.x - constraint.min.x;
            constraint.dimensions.y = constraint.max.y - constraint.min.y;
            constraint.dimensions.z = constraint.max.z - constraint.min.z;

            constraint.center.x = constraint.min.x + (constraint.dimensions.x / 2);
            constraint.center.y = constraint.min.y + (constraint.dimensions.y / 2);
            constraint.center.z = constraint.min.z + (constraint.dimensions.z / 2);

            constraint.calculated = true;
        }

        struct {
            bool calculated = false;
            spatial::vector min;
            spatial::vector max;
            spatial::vector center;
            spatial::vector dimensions;
        } constraint;
    };

}
