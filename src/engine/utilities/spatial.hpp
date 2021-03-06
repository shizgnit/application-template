#pragma once

/// GLM - Trying it out to potentially replace custom implementation
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtx/intersect.hpp>


namespace spatial {

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

        vector& cross(const vector& operand) {
            return(*this %= operand);
        }

        virtual vector& operator() (const type_t& x, const type_t& y, const type_t& z = 0.0f, const type_t& w = 1.0f);

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

        type_t distance(const vector& v) const;

        vector project(const matrix& model, const matrix& view, const matrix& projection);
        vector unproject(const matrix& view, const matrix& projection, int width, int height) const;

        operator glm::vec3() const;

    public:

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

    class matrix {
    public:
        typedef vector::type_t type_t;
        typedef struct matrix_t { type_t x[4][4]; type_t* const operator [] (int i) const { return const_cast<type_t *>(x[i]); } } matrix_t;

        matrix();
        matrix(const matrix& m);
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

    private:
        matrix_t r;

    public:
        matrix& identity();

        virtual matrix& operator *= (const matrix& operand);
        matrix operator * (const matrix& operand) const;

        matrix& scale(const type_t& operand);
        
        
        //matrix& operator *= (const type_t& operand);
        //matrix operator * (const type_t& operand) const;

        virtual vector operator * (const vector& v) const;

        vector interpolate(const vector& v) const;

        matrix& rotate_x(type_t angle);
        matrix& rotate_y(type_t angle);
        matrix& rotate_z(type_t angle);

        matrix& translate(const vector& v);
        matrix& translate(const type_t& x, const type_t& y, const type_t& z, const type_t& w = 1.0f);
        matrix& translate(const vector& eye, const vector& center, const vector& up);

        matrix& perspective(type_t fov, type_t aspect, type_t n, type_t f);
        matrix& ortho(type_t left, type_t right, type_t bottom, type_t top);

        matrix& lookat(const vector& eye, const vector& center, const vector& up);

        matrix& invert();
    };

    class position {
    public:
        typedef vector::type_t type_t;

        position(void);
        position(const position& ref);

        operator matrix();

        void identity(void);

        void viewable(bool toggle);

        void move(type_t t);
        void vertical(type_t t);

        void strafe(type_t t);

        void rotate(type_t x, type_t y, type_t z = 0.0f);

        void project(const vector& offset, const vector& projection);

    public:
        bool view;

        vector rotation;

        vector eye;
        vector center;
        vector up;
    };

    class quaternion : public vector {
    public:
        typedef vector::type_t type_t;

        quaternion();
        quaternion(const matrix& m);

        ~quaternion();

        operator matrix();
        void euler(const type_t& x, const type_t& y, const type_t& z, const type_t& degrees);

        quaternion operator *(const quaternion& operand);
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
        enum primitive {
            UNDEFINED = 0x00,
            POINT = 0x01,
            LINE = 0x02,
            POLYGON = 0x04
        } type;

        geometry(primitive t) : type(t) {}
        geometry(primitive t, size_t elements) : type(t), vertices(elements) {}

        geometry& operator = (const spatial::geometry& ref) {
            this->vertices = ref.vertices;
            return *this;
        }

        std::vector<spatial::vector> vertices;
    };

    // TODO: currently planes and spheres are specifications only and do not represent visualized geometry, eventually these need to auto generate geometry
    class plane {
    public:
        vector point;
        vector normal;
    };
    class sphere {
    public:
        typedef vector::type_t type_t;

        vector center;
        type_t radius;
    };

    class triangle : public geometry {
    public:
        triangle() : geometry(geometry::POLYGON, 3) {}
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
        quad() : geometry(geometry::POLYGON) {}
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

        void size(int width, int height);

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
    };

    class ray : public geometry {
    public:
        ray() : geometry(geometry::LINE) {}
        ray(const vector& origin, const vector& terminus);

        void endpoints(const vector& origin, const vector& terminus);

        typedef vector::type_t type_t;

        type_t distance(const vector& v) const;
        type_t distance(const ray& r);

        bool intersects(const quad& t);
        bool intersects(const triangle& t);
        bool intersects(const plane& t);
        bool intersects(const sphere& t);

        vector intersection(const quad& t);
        vector intersection(const triangle& t);
        vector intersection(const plane& t);
    };

}

