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

/// GLM - Trying it out to potentially replace custom implementation
#if defined _USE_GLM
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif

namespace spatial {

    inline double radians(double angle) {
        return angle * (double)M_PI / 180.0f;
    }

    class matrix;

    class vector {
    public:
        typedef float type_t;

        vector();
        vector(type_t v[]);
        vector(const type_t& v);
        vector(const type_t& x, const type_t& y, const type_t& z = 0.0f, const type_t& w = 1.0f);

        vector(const vector& ref);
        virtual vector& operator = (const vector& operand);

        virtual vector& operator += (const vector& operand);
        virtual vector operator + (const vector& operand) const;

        virtual vector& operator -= (const vector& operand);
        virtual vector operator - (const vector& operand) const;

        virtual vector& operator *= (const vector& operand);
        virtual vector operator * (const vector& operand) const;

        virtual vector& operator /= (const vector& operand);
        virtual vector operator / (const vector& operand) const;

        virtual vector& operator %= (const vector& operand);
        virtual vector operator % (const vector& operand) const;

        virtual bool operator == (const vector& operand) const;
        virtual bool operator != (const vector& operand) const;

        vector& cross(const vector& operand) {
            return(*this %= operand);
        }

        virtual vector& operator() (const type_t& x, const type_t& y, const type_t& z = 0.0f, const type_t& w = 1.0f);

        vector& rotate(const vector& axis, type_t rad);
        vector& rotate_x(type_t rad);
        vector& rotate_y(type_t rad);
        vector& rotate_z(type_t rad);

        type_t inner(const vector& operand) {
            return this->dot(operand);
        };

        type_t dot(const vector& operand) const;
        type_t dot() const;

        type_t length() const;
        vector unit() const; // TODO: make the interface consistent for behavior
        vector& normalize();

        type_t distance(const vector& v) const;

        vector project(const matrix& projection, const matrix& view, const matrix& model);
        vector unproject(const matrix& projection, const matrix& view, int width, int height) const;

        bool encompassed_xz(const vector& v0, const vector& v1, const vector& v2) const;

#if defined _USE_GLM
        operator glm::vec3() const;
#endif
        
        virtual bool hasValue() const {
            return (x == 0 && y == 0 && z == 0 && w == 1) == false;
        }

        operator std::string() const {
            std::stringstream ss;
            ss << "{" << x << "," << y << "," << z << "," << w << "}";
            return ss.str();
        }
        
        vector lerp(const vector& to, const type_t t);
        vector slerp(const vector& to, const type_t t);
        
    public:

#if defined _VECTOR_PADDING
        float padding[_VECTOR_PADDING];
#endif

        union {
            struct {
                type_t x;
                type_t y;
                type_t z;
                type_t w;
            };
            type_t l[4];
        };
    };

    class vertex {
    public:
        vertex() {}
        vertex(const vector& position) {
            *this = position;
        }

        vector coordinate;
        vector texture;
        vector normal;

        // Assume that a vector assignment is for the coordinate portion
        const vertex& operator=(const vector& position) {
            coordinate = position;
            return *this;
        }
    };

    class geometry {
    public:
        geometry() {}
        geometry(size_t elements) : vertices(elements) {}
        geometry(const std::vector<spatial::vertex>& vertices) {
            *this = vertices;
        }

        geometry& operator = (const spatial::geometry& ref) {
            this->vertices = ref.vertices;
            return *this;
        }

        geometry& operator = (const std::vector<spatial::vertex>& vertices) {
            this->vertices.resize(vertices.size());
            for (auto& i : utilities::indices(vertices)) {
                this->vertices[i] = vertices[i].coordinate;
            }
            return *this;
        }

        std::vector<spatial::vector> vertices;
    };

    class matrix {
    public:
        typedef vector::type_t type_t;
        typedef struct matrix_t { type_t x[4][4]; type_t* const operator [] (int i) const { return const_cast<type_t *>(x[i]); } } matrix_t;

        matrix();
        matrix(const matrix& m);
        matrix(const vector& c0, const vector& c1, const vector& c2, const vector& c3);
        matrix(std::initializer_list<std::vector<type_t>> ref) {
            int i = 0;
            for (auto row : ref) {
                for (int col = 0; col < row.size(); col++) {
                    r[i][col] = row[col];
                }
                i += 1;
            }
        }

        type_t* operator [] (int i) const {
            return r[i];
        }

        type_t* data() const {
            return const_cast<type_t *>((type_t *)(r.x));
        }

        operator std::string() const {
            std::stringstream ss;            
            ss << "{";
            for (int row = 0; row < 4; row++) {
                ss << "{";
                for (int col = 0; col < 4; col++) {
                    ss << r[row][col] << (col < 3 ? "," : "");
                }
                ss << "}" << (row < 3 ? ",\n" : "");
            }
            ss << "}";
            return ss.str();
        }
        
        operator bool() const {
            spatial::matrix ident;
            type_t *ref = ident.r[0];
            type_t *ptr = r[0];
            for(int i=0; i<16; i++) {
                if(ref[i] != ptr[i]) {
                    return false;
                }
            }
            return true;
        }


    private:
#if defined _MATRIX_PADDING
        float padding[_MATRIX_PADDING];
#endif
        matrix_t r;

    public:
        matrix& identity();

        virtual matrix& operator *= (const matrix& operand);

        matrix operator * (const matrix& operand) const;
        matrix operator * (const type_t& operand) const;

        matrix& scale(const type_t& operand);
        matrix& scale_x(const type_t& operand);
        matrix& scale_y(const type_t& operand);
        matrix& scale_z(const type_t& operand);

        virtual vector operator * (const vector& v) const;
        virtual geometry operator * (const geometry& v) const;

        vector interpolate(const vector& v) const;
        geometry interpolate(const geometry& g) const;

        matrix& rotate(const vector& axis, type_t rad);
        matrix& rotate_x(type_t rad);
        matrix& rotate_y(type_t rad);
        matrix& rotate_z(type_t rad);

        matrix& position(const vector& v);
        matrix& position(const type_t& x, const type_t& y, const type_t& z, const type_t& w = 1.0f);

        matrix& translate(const vector& v);
        matrix& translate(const type_t& x, const type_t& y, const type_t& z, const type_t& w = 1.0f);
        matrix& translate(const vector& eye, const vector& focus, const vector& up);

        matrix& perspective(type_t fov, type_t aspect, type_t near=0.0f, type_t far=10.0f);
        matrix& ortho(type_t left, type_t right, type_t bottom, type_t top, type_t near=0.0f, type_t far=10.0f);

        matrix& lookat(const vector& eye, const vector& focus, const vector& up);

        matrix& invert();
        matrix& pivot();

        matrix& set(int row, int col, type_t value);
    };

    class quaternion : public vector {
    public:
        typedef vector::type_t type_t;

        quaternion();
        quaternion(type_t v[]);
        quaternion(const type_t& x, const type_t& y, const type_t& z = 0.0f, const type_t& w = 1.0f);

        quaternion(const vector& v);
        quaternion(const matrix& m);

        ~quaternion();

        operator matrix();

        quaternion& translate(const vector& eye, const vector& center, const vector& up);
        quaternion& euler(const type_t& x, const type_t& y, const type_t& z, const type_t& degrees);

        quaternion operator *(const quaternion& operand);

        quaternion lerp(const quaternion& q, const type_t t);
        quaternion slerp(const quaternion& q, const type_t t);
    };

    /// <summary>
    /// 
    ///     up (+y)
    ///      ^
    ///      |
    ///      |
    ///   eye . ---> focus (lookat -z)
    /// 
    /// </summary>
    class position {
    public:
        typedef vector::type_t type_t;

        position(void);
        position(const position& ref);

        position(const spatial::vector& pos);

        operator matrix();

        void identity(void);

        void viewable(bool toggle);

        position& scale(type_t value=0.0f);

        position& surge(type_t t);
        position& sway(type_t t);
        position& heave(type_t t);

        position& pitch(type_t angle);
        position& yaw(type_t angle);
        position& roll(type_t angle);
        position& spin(type_t angle);

        void project(const vector& offset, const vector& projection);

        position& orientation(const position& reference);
        position& reposition(const vector& offset);
        position& lookat(const vector& offset);

        void constrain(bool x, bool y, bool z);

        vector x(type_t magnitude = -1.0);
        vector y(type_t magnitude = 1.0);
        vector z(type_t magnitude = 1.0);

        vector forward();
        vector down();
        vector tanget();

        position& operator=(const position& ref);
        
    public:
        struct {
            bool x = false;
            bool y = false;
            bool z = false;
        } constraint;

        struct {
            type_t pitch = 0.0f;
            type_t yaw = 0.0f;
            type_t roll = 0.0f;
            type_t spin = 0.0f;
            type_t scale = 1.0f;
        } translation;

        vector eye;
        vector focus;
        vector up;

        void apply(const position& reference);

        double rate = 1.0f;

        bool modified() {
            return dirty;
        }

        spatial::matrix& serialize();
        
    protected:
        bool view = false;
        bool dirty = true;
        
        spatial::matrix state;
        
        position& rotate();
        
        position& modify() {
            dirty = true;
            return *this;
        }
    };

    class store {
        position storage;
        position* source = NULL;
        store() {} // hidden
    public:
        store(position& instance) {
            if (instance.constraint.x || instance.constraint.y || instance.constraint.z) {
                storage = instance;
                source = &instance;
            }
        }
        ~store() {
            if (source) {
                source->apply(storage);
            }
        }
    };

    class plane : public geometry {
    public:
        plane() : geometry() {}
        plane(vector p, vector n) {
            point = p;
            normal = n;
        }
        plane(spatial::position& pos) {
            point = pos.eye;
            normal = (pos.focus - pos.eye).unit();
        }

        vector point;
        vector normal;
    };

    class sphere : public geometry {
    public:
        sphere() : geometry() {}
        sphere(int horizontal, int vertical);

        typedef vector::type_t type_t;

        vector center = { 0.0f, 0.0f, 0.0f };
        type_t radius = 1.0f;

        sphere &interpolate(int horizontal, int vertical);
    };

    class triangle : public geometry {
    public:
        triangle() : geometry(3) {}
        triangle(const spatial::vector& v0, const spatial::vector& v1, const spatial::vector& v2) : triangle() {
            vertices[0] = v0;
            vertices[1] = v1;
            vertices[2] = v2;
        }

        typedef vector::type_t type_t;

        spatial::vector normal() const;

        triangle& project(const matrix& model, const matrix& view, const matrix& projection);
    };

    class quad : public geometry {
    public:
        quad() : geometry() {}
        quad(int width, int height);
        quad(const std::vector<spatial::vector>& vertices) : quad() {
            this->vertices = vertices;
        }
        quad(const std::vector<spatial::vertex>& vertices) : quad() {
            this->vertices.resize(vertices.size());
            for (auto& i : utilities::indices(vertices)) {
                this->vertices[i] = vertices[i].coordinate;
            }
        }

        quad &interpolate(int width, int height);

        quad& operator =(const std::vector<spatial::vector>& vertices) {
            this->vertices = vertices;
            return *this;
        }

        typedef vector::type_t type_t;

        // TODO: might use this
        quad& project(const matrix& model, const matrix& view, const matrix& projection);

        operator geometry& () {
            return *this;
        }

        static spatial::geometry edges(int width, int height);
    };

    class ray : public geometry {
    public:
        ray() : geometry() {}
        ray(const vector& point, const vector& normal);

        ray(const vector& point, const matrix& perspective, const matrix& view, const int& w, const int& h);

        ray &interpolate(const vector& origin, const vector& terminus);
        ray &projection(const vector& point, const matrix& perspective, const matrix& view, const int& w, const int& h);

        typedef vector::type_t type_t;

        type_t distance(const vector& v) const;
        type_t distance(const ray& r);

        bool intersects(const geometry& t);
        bool intersects(const triangle& t);
        bool intersects(const plane& t);
        bool intersects(const sphere& t);

        vector intersection(const geometry& t);
        vector intersection(const triangle& t);
        vector intersection(const plane& t);

        void extend(const type_t& d);
    };

}

