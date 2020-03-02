#pragma once

namespace spatial {

    class matrix;

    class vector {
    public:
        typedef float type_t;

        vector();
        vector(type_t v);
        vector(type_t v[]);
        vector(type_t x, type_t y, type_t z, type_t w = 1.0f);

        vector(const vector& ref);
        virtual vector& operator = (const vector& operand);

        virtual vector& operator += (const vector& operand);
        virtual vector operator + (const vector& operand);

        virtual vector& operator -= (const vector& operand);
        virtual vector operator - (const vector& operand) const;

        virtual vector& operator *= (const vector& operand);
        virtual vector operator * (const vector& operand) const;

        virtual vector& operator /= (const vector& operand);
        virtual vector operator / (const vector& operand) const;

        virtual vector& operator %= (const vector& operand);
        virtual vector operator % (const vector& operand) const;

        vector& rotate_x(type_t rad);

        vector rotate_y(type_t rad);

        vector rotate_z(type_t rad);

        type_t dot();

        type_t length();
        vector unit();

        type_t distance(const vector& v);

        void unproject(vector mouse, const matrix& model, const matrix& projection, int width, int height);

    public:
        type_t x;
        type_t y;
        type_t z;
        type_t w;
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
        matrix& operator *= (const type_t& operand);
        matrix operator * (const type_t& operand) const;

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
}

