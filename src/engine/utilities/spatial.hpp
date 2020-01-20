#pragma once

namespace spatial {

    class matrix;

    class vector {
    public:
        vector();
        vector(float v);
        vector(float v[]);
        vector(float x, float y, float z, float w = 1.0f);

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

        vector& rotate_x(float rad);

        vector rotate_y(float rad);

        vector rotate_z(float rad);

        float dot();

        float length();
        vector unit();

        void unproject(vector mouse, const matrix& model, const matrix& projection, int width, int height);

    public:
        float x;
        float y;
        float z;
        float w;
    };

    struct row { float x[4]; };

    class matrix {
    public:
        matrix();
        matrix(const matrix& m);
        matrix(float d[]);

        matrix(row a, row b, row c, row d) {}

        matrix& identity();

        virtual matrix& operator *= (const matrix& operand);
        matrix operator * (const matrix& operand) const;
        matrix& scale(const float& operand);
        matrix& operator *= (const float& operand);
        matrix operator * (const float& operand) const;

        matrix& rotate_x(float angle);
        matrix& rotate_y(float angle);
        matrix& rotate_z(float angle);

        matrix& translate(const vector& v);
        matrix& translate(const float& x, const float& y, const float& z, const float& w = 1.0f);
        matrix& translate(const vector& eye, const vector& center, const vector& up);

        matrix& perspective(float fov, float aspect, float n, float f);
        matrix& ortho(float left, float right, float bottom, float top);

        matrix& lookat(const vector& eye, const vector& center, const vector& up);

        matrix& invert();

    public:
        float r[4][4];
    };

    class position {
    public:
        position(void);
        position(const position& ref);

        operator matrix();

        void identity(void);

        void viewable(bool toggle);

        void move(float t);
        void vertical(float t);

        void strafe(float t);

        void rotate(float x, float y, float z = 0.0f);

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

        quaternion();
        quaternion(const matrix& m);

        ~quaternion();

        operator matrix();
        void euler(const float& x, const float& y, const float& z, const float& degrees);

        quaternion operator *(const quaternion& operand);
    };
}

