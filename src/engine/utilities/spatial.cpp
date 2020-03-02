#include "engine.hpp"

spatial::vector::vector() {
    this->x = 0.0f;
    this->y = 0.0f;
    this->z = 0.0f;
    this->w = 1.0f;
}

spatial::vector::vector(type_t v) {
    this->x = v;
    this->y = v;
    this->z = v;
    this->w = v;
}

spatial::vector::vector(type_t v[]) {
    this->x = v[0];
    this->y = v[1];
    this->z = v[2];
    this->w = v[3];
}

spatial::vector::vector(type_t x, type_t y, type_t z, type_t w) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

spatial::vector::vector(const vector& ref) {
    this->x = ref.x;
    this->y = ref.y;
    this->z = ref.z;
    this->w = ref.w;
}

spatial::vector& spatial::vector::operator = (const vector& operand) {
    this->x = operand.x;
    this->y = operand.y;
    this->z = operand.z;
    this->w = operand.w;
    return(*this);
}

spatial::vector& spatial::vector::operator += (const vector& operand) {
    return (*this = (*this) + operand);
}
spatial::vector spatial::vector::operator + (const vector& operand) {
    vector result;
    result.x = x + operand.x;
    result.y = y + operand.y;
    result.z = z + operand.z;
    return result;
}

spatial::vector& spatial::vector::operator -= (const vector& operand) {
    return (*this = (*this) - operand);
}
spatial::vector spatial::vector::operator - (const vector& operand) const {
    vector result;
    result.x = x - operand.x;
    result.y = y - operand.y;
    result.z = z - operand.z;
    return result;
}

spatial::vector& spatial::vector::operator *= (const vector& operand) {
    return (*this = (*this) * operand);
}
spatial::vector spatial::vector::operator * (const vector& operand) const {
    vector result;
    result.x = x * operand.x;
    result.y = y * operand.y;
    result.z = z * operand.z;
    return result;
}

spatial::vector& spatial::vector::operator /= (const vector& operand) {
    return (*this = (*this) / operand);
}
spatial::vector spatial::vector::operator / (const vector& operand) const {
    vector result;
    result.x = x / (operand.x ? operand.x : 1.0f);
    result.y = y / (operand.y ? operand.y : 1.0f);
    result.z = z / (operand.z ? operand.z : 1.0f);
    return result;
}

spatial::vector& spatial::vector::operator %= (const vector& operand) {
    return (*this = (*this) % operand);
}
spatial::vector spatial::vector::operator % (const vector& operand) const {
    vector result;
    result.x = (y * operand.z) - (z * operand.y);
    result.y = (z * operand.x) - (x * operand.z);
    result.z = (x * operand.y) - (y * operand.x);
    return result;
}

spatial::vector& spatial::vector::rotate_x(type_t rad) {
    vector result;
    result.x = (x * 1) + (y * 0) + (z * 0);
    result.y = (x * 0) + (y * cosf(rad)) + (z * sinf(rad));
    result.z = (x * 0) + (y * -sinf(rad)) + (z * cosf(rad));
    return (*this = result);
}

spatial::vector spatial::vector::rotate_y(type_t rad) {
    vector result;
    result.x = (x * cosf(rad)) + (y * 0) + (z * sinf(rad));
    result.y = (x * 0) + (y * 1) + (z * 0);
    result.z = (x * -sinf(rad)) + (y * 0) + (z * cosf(rad));
    return (*this = result);
}

spatial::vector spatial::vector::rotate_z(type_t rad) {
    vector result;
    result.x = (x * cosf(rad)) + (y * sinf(rad)) + (z * 0);
    result.y = (x * -sinf(rad)) + (y * cosf(rad)) + (z * 0);
    result.z = (x * 0) + (y * 0) + (z * 1);
    return (*this = result);
}

spatial::vector::type_t spatial::vector::dot() {
    return(x * x + y * y + z * z);
}

spatial::vector::type_t spatial::vector::length() {
    return((type_t)sqrt(dot()));
}

spatial::vector spatial::vector::unit() {
    return(*this / (vector)length());
}

spatial::vector::type_t spatial::vector::distance(const vector& v) {
    return sqrt(pow(x - v.x, 2) + pow(y - v.y, 2) + pow(z - v.z, 2));
}

void spatial::vector::unproject(vector mouse, const matrix& model, const matrix& projection, int width, int height) {
    matrix inverted = model * projection;
    inverted.invert();

    mouse.x /= width;
    mouse.y /= height;

    mouse.x = (mouse.x / width) * 2 - 1;
    mouse.y = (mouse.y / height) * 2 - 1;
    mouse.z = mouse.z * 2 - 1;

    // don't have a matrix vector multiply yet
    //*this = inverted * mouse;

    mouse /= mouse.z;
}

spatial::matrix::matrix() {
    identity();
}
spatial::matrix::matrix(const matrix& m) {
    r = m.r;
}

spatial::matrix& spatial::matrix::identity() {
    *this = { { 1.0f, 0.0f, 0.0f, 0.0f },
              { 0.0f, 1.0f, 0.0f, 0.0f },
              { 0.0f, 0.0f, 1.0f, 0.0f },
              { 0.0f, 0.0f, 0.0f, 1.0f } };
    return *this;
}

spatial::matrix& spatial::matrix::operator *= (const matrix& operand) {
    return (*this = (*this) * operand);
}
spatial::matrix spatial::matrix::operator * (const matrix& operand) const {
    //matrix result = *this;

    /*
    result.r[0][0] = r[0][0] * operand.r[0][0] + r[1][0] * operand.r[0][1] + r[2][0] * operand.r[0][2] + r[3][0] * operand.r[0][3];
    result.r[0][1] = r[0][1] * operand.r[0][0] + r[1][1] * operand.r[0][1] + r[2][1] * operand.r[0][2] + r[3][1] * operand.r[0][3];
    result.r[0][2] = r[0][2] * operand.r[0][0] + r[1][2] * operand.r[0][1] + r[2][2] * operand.r[0][2] + r[3][2] * operand.r[0][3];
    result.r[0][3] = r[0][3] * operand.r[0][0] + r[1][3] * operand.r[0][1] + r[2][3] * operand.r[0][2] + r[3][3] * operand.r[0][3];

    result.r[1][0] = r[0][0] * operand.r[1][0] + r[1][0] * operand.r[1][1] + r[2][0] * operand.r[1][2] + r[3][0] * operand.r[1][3];
    result.r[1][1] = r[0][1] * operand.r[1][0] + r[1][1] * operand.r[1][1] + r[2][1] * operand.r[1][2] + r[3][1] * operand.r[1][3];
    result.r[1][2] = r[0][2] * operand.r[1][0] + r[1][2] * operand.r[1][1] + r[2][2] * operand.r[1][2] + r[3][2] * operand.r[1][3];
    result.r[1][3] = r[0][3] * operand.r[1][0] + r[1][3] * operand.r[1][1] + r[2][3] * operand.r[1][2] + r[3][3] * operand.r[1][3];

    result.r[2][0] = r[0][0] * operand.r[2][0] + r[1][0] * operand.r[2][1] + r[2][0] * operand.r[2][2] + r[3][0] * operand.r[2][3];
    result.r[2][1] = r[0][1] * operand.r[2][0] + r[1][1] * operand.r[2][1] + r[2][1] * operand.r[2][2] + r[3][1] * operand.r[2][3];
    result.r[2][2] = r[0][2] * operand.r[2][0] + r[1][2] * operand.r[2][1] + r[2][2] * operand.r[2][2] + r[3][2] * operand.r[2][3];
    result.r[2][3] = r[0][3] * operand.r[2][0] + r[1][3] * operand.r[2][1] + r[2][3] * operand.r[2][2] + r[3][3] * operand.r[2][3];

    result.r[3][0] = r[0][0] * operand.r[3][0] + r[1][0] * operand.r[3][1] + r[2][0] * operand.r[3][2] + r[3][0] * operand.r[3][3];
    result.r[3][1] = r[0][1] * operand.r[3][0] + r[1][1] * operand.r[3][1] + r[2][1] * operand.r[3][2] + r[3][1] * operand.r[3][3];
    result.r[3][2] = r[0][2] * operand.r[3][0] + r[1][2] * operand.r[3][1] + r[2][2] * operand.r[3][2] + r[3][2] * operand.r[3][3];

    //result.r[3][3] = r[0][3] * operand.r[3][0] + r[1][3] * operand.r[3][1] + r[2][3] * operand.r[3][2] + r[3][3] * operand.r[3][3];

    */

    matrix result;
    int k, r, c;
    for (c = 0; c < 4; ++c) for (r = 0; r < 4; ++r) {
        result.r[c][r] = 0.f;
        for (k = 0; k < 4; ++k)
            result.r[c][r] += this->r[k][r] * operand.r[c][k];
    }

    return result;
}

spatial::matrix& spatial::matrix::scale(const type_t& operand) {
    return(*this *= operand);
}
spatial::matrix& spatial::matrix::operator *= (const type_t& operand) {
    return (*this = (*this) * operand);
}
spatial::matrix spatial::matrix::operator * (const type_t& operand) const {
    matrix result = *this;

    result.r[0][0] *= operand;
    result.r[0][1] *= operand;
    result.r[0][2] *= operand;
    result.r[0][3] *= operand;

    result.r[1][0] *= operand;
    result.r[1][1] *= operand;
    result.r[1][2] *= operand;
    result.r[1][3] *= operand;

    result.r[2][0] *= operand;
    result.r[2][1] *= operand;
    result.r[2][2] *= operand;
    result.r[2][3] *= operand;

    result.r[3][0] *= operand;
    result.r[3][1] *= operand;
    result.r[3][2] *= operand;

    // result.r[3][3] *= operand;

    return result;
}

spatial::matrix& spatial::matrix::rotate_x(type_t angle) {
    matrix rotation;

    rotation.r[0][0] = 1.0f;
    rotation.r[0][1] = 0.0f;
    rotation.r[0][2] = 0.0f;
    rotation.r[0][3] = 0.0f;

    rotation.r[1][0] = 0.0f;
    rotation.r[1][1] = cosf(angle);
    rotation.r[1][2] = sinf(angle);
    rotation.r[1][3] = 0.0f;

    rotation.r[2][0] = 0.0f;
    rotation.r[2][1] = -sinf(angle);
    rotation.r[2][2] = cosf(angle);
    rotation.r[2][3] = 0.0f;

    rotation.r[3][0] = 0.0f;
    rotation.r[3][1] = 0.0f;
    rotation.r[3][2] = 0.0f;
    rotation.r[3][3] = 1.0f;

    return(*this *= rotation);
}
spatial::matrix& spatial::matrix::rotate_y(type_t angle) {
    matrix rotation;

    rotation.r[0][0] = cosf(angle);
    rotation.r[0][1] = 0.0f;
    rotation.r[0][2] = -sinf(angle);
    rotation.r[0][3] = 0.0f;

    rotation.r[1][0] = 0.0f;
    rotation.r[1][1] = 1.0f;
    rotation.r[1][2] = 0.0f;
    rotation.r[1][3] = 0.0f;

    rotation.r[2][0] = sinf(angle);
    rotation.r[2][1] = 0.0f;
    rotation.r[2][2] = cosf(angle);
    rotation.r[2][3] = 0.0f;

    rotation.r[3][0] = 0.0f;
    rotation.r[3][1] = 0.0f;
    rotation.r[3][2] = 0.0f;
    rotation.r[3][3] = 1.0f;

    return(*this *= rotation);
}
spatial::matrix& spatial::matrix::rotate_z(type_t angle) {
    matrix rotation;

    rotation.r[0][0] = cosf(angle);
    rotation.r[0][1] = sinf(angle);
    rotation.r[0][2] = 0.0f;
    rotation.r[0][3] = 0.0f;

    rotation.r[1][0] = -sinf(angle);
    rotation.r[1][1] = cosf(angle);
    rotation.r[1][2] = 0.0f;
    rotation.r[1][3] = 0.0f;

    rotation.r[2][0] = 0.0f;
    rotation.r[2][1] = 0.0f;
    rotation.r[2][2] = 1.0f;
    rotation.r[2][3] = 0.0f;

    rotation.r[3][0] = 0.0f;
    rotation.r[3][1] = 0.0f;
    rotation.r[3][2] = 0.0f;
    rotation.r[3][3] = 1.0f;

    return(*this *= rotation);
}

spatial::matrix& spatial::matrix::translate(const vector& v) {
    return(this->translate(v.x, v.y, v.z, v.w));
}
spatial::matrix& spatial::matrix::translate(const type_t& x, const type_t& y, const type_t& z, const type_t& w) {
    matrix current = *this;

    r[3][0] = x * current.r[0][0] + y * current.r[1][0] + z * current.r[2][0] + w * current.r[3][0];
    r[3][1] = x * current.r[0][1] + y * current.r[1][1] + z * current.r[2][1] + w * current.r[3][1];
    r[3][2] = x * current.r[0][2] + y * current.r[1][2] + z * current.r[2][2] + w * current.r[3][2];
    r[3][3] = x * current.r[0][3] + y * current.r[1][3] + z * current.r[2][3] + w * current.r[3][3];

    return *this;
}

spatial::matrix& spatial::matrix::perspective(type_t fov, type_t aspect, type_t n, type_t f) {
    type_t const a = 1.0f / (type_t)tan(fov / 2.0f);

    r[0][0] = a / aspect;
    r[0][1] = 0.0f;
    r[0][2] = 0.0f;
    r[0][3] = 0.0f;

    r[1][0] = 0.0f;
    r[1][1] = a;
    r[1][2] = 0.0f;
    r[1][3] = 0.0f;

    r[2][0] = 0.0f;
    r[2][1] = 0.0f;
    r[2][2] = -((f + n) / (f - n));
    r[2][3] = -1.0f;

    r[3][0] = 0.0f;
    r[3][1] = 0.0f;
    r[3][2] = -((2.0f * f * n) / (f - n));
    r[3][3] = 1.0f;

    return *this;
}

spatial::matrix& spatial::matrix::ortho(type_t left, type_t right, type_t bottom, type_t top) {
    r[0][0] = 2.0f / (right - left);
    r[1][1] = 2.f / (top - bottom);
    r[2][2] = -1.0f;
    r[3][0] = -(right + left) / (right - left);
    r[3][1] = -(top + bottom) / (top - bottom);
    r[3][3] = 1.0f;
    return *this;
}

spatial::matrix& spatial::matrix::lookat(const vector& eye, const vector& center, const vector& up) {
    vector f = (center - eye).unit();
    vector s = (f % up).unit();
    vector t = s % f;

    r[0][0] = s.x;
    r[1][0] = s.y;
    r[2][0] = s.z;

    r[0][1] = t.x;
    r[1][1] = t.y;
    r[2][1] = t.z;

    r[0][2] = -f.x;
    r[1][2] = -f.y;
    r[2][2] = -f.z;

    return this->translate(eye * -1.0f);
}

spatial::matrix& spatial::matrix::translate(const vector& eye, const vector& center, const vector& up) {

    vector f = (center - eye).unit();
    vector s = (f % up).unit();
    vector t = s % f;

    r[0][0] = s.x;
    r[1][0] = s.y;
    r[2][0] = s.z;

    r[0][1] = t.x;
    r[1][1] = t.y;
    r[2][1] = t.z;

    r[0][2] = -f.x;
    r[1][2] = -f.y;
    r[2][2] = -f.z;

    return this->translate(center);
}

spatial::matrix& spatial::matrix::invert() {
    type_t s[6];
    type_t c[6];

    s[0] = r[0][0] * r[1][1] - r[1][0] * r[0][1];
    s[1] = r[0][0] * r[1][2] - r[1][0] * r[0][2];
    s[2] = r[0][0] * r[1][3] - r[1][0] * r[0][3];
    s[3] = r[0][1] * r[1][2] - r[1][1] * r[0][2];
    s[4] = r[0][1] * r[1][3] - r[1][1] * r[0][3];
    s[5] = r[0][2] * r[1][3] - r[1][2] * r[0][3];

    c[0] = r[2][0] * r[3][1] - r[3][0] * r[2][1];
    c[1] = r[2][0] * r[3][2] - r[3][0] * r[2][2];
    c[2] = r[2][0] * r[3][3] - r[3][0] * r[2][3];
    c[3] = r[2][1] * r[3][2] - r[3][1] * r[2][2];
    c[4] = r[2][1] * r[3][3] - r[3][1] * r[2][3];
    c[5] = r[2][2] * r[3][3] - r[3][2] * r[2][3];

    /* Assumes it is invertible */
    type_t idet = 1.0f / (s[0] * c[5] - s[1] * c[4] + s[2] * c[3] + s[3] * c[2] - s[4] * c[1] + s[5] * c[0]);

    matrix result;

    result.r[0][0] = (r[1][1] * c[5] - r[1][2] * c[4] + r[1][3] * c[3]) * idet;
    result.r[0][1] = (-r[0][1] * c[5] + r[0][2] * c[4] - r[0][3] * c[3]) * idet;
    result.r[0][2] = (r[3][1] * s[5] - r[3][2] * s[4] + r[3][3] * s[3]) * idet;
    result.r[0][3] = (-r[2][1] * s[5] + r[2][2] * s[4] - r[2][3] * s[3]) * idet;

    result.r[1][0] = (-r[1][0] * c[5] + r[1][2] * c[2] - r[1][3] * c[1]) * idet;
    result.r[1][1] = (r[0][0] * c[5] - r[0][2] * c[2] + r[0][3] * c[1]) * idet;
    result.r[1][2] = (-r[3][0] * s[5] + r[3][2] * s[2] - r[3][3] * s[1]) * idet;
    result.r[1][3] = (r[2][0] * s[5] - r[2][2] * s[2] + r[2][3] * s[1]) * idet;

    result.r[2][0] = (r[1][0] * c[4] - r[1][1] * c[2] + r[1][3] * c[0]) * idet;
    result.r[2][1] = (-r[0][0] * c[4] + r[0][1] * c[2] - r[0][3] * c[0]) * idet;
    result.r[2][2] = (r[3][0] * s[4] - r[3][1] * s[2] + r[3][3] * s[0]) * idet;
    result.r[2][3] = (-r[2][0] * s[4] + r[2][1] * s[2] - r[2][3] * s[0]) * idet;

    result.r[3][0] = (-r[1][0] * c[3] + r[1][1] * c[1] - r[1][2] * c[0]) * idet;
    result.r[3][1] = (r[0][0] * c[3] - r[0][1] * c[1] + r[0][2] * c[0]) * idet;
    result.r[3][2] = (-r[3][0] * s[3] + r[3][1] * s[1] - r[3][2] * s[0]) * idet;
    result.r[3][3] = (r[2][0] * s[3] - r[2][1] * s[1] + r[2][2] * s[0]) * idet;

    return *this = result;
}


spatial::position::position(void) {
    view = false;
    identity();
}

spatial::position::position(const position& ref) {
    view = ref.view;

    rotation = ref.rotation;

    eye = ref.eye;
    center = ref.center;
    up = ref.up;
}

spatial::position::operator spatial::matrix() {
    matrix result;
    result.translate(eye, center, up);
    return(result);
}

void spatial::position::identity(void) {
    eye.x = 0.0f;
    eye.y = 0.0f;
    eye.z = 1.0f;

    center.x = 0.0f;
    center.y = 0.0f;
    center.z = 0.0f;

    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
}

void spatial::position::viewable(bool toggle) {
    view = toggle;
}

void spatial::position::move(type_t t) {
    vector diff = (eye - center).unit() * t;

    center += diff;
    eye += diff;
}

void spatial::position::vertical(type_t t) {
    vector diff = up * t;

    center += diff;
    eye += diff;
}

void spatial::position::strafe(type_t t) {
    vector normal = eye - center;
    vector cross = (normal % up) + center;
    vector diff = (cross - center) * t;

    center += diff;
    eye += diff;
}

void spatial::position::rotate(type_t x, type_t y, type_t z) {
    rotation.x += x;
    rotation.y += y;
    rotation.z += z;

    vector offset = center;

    identity();

    type_t radx = static_cast<type_t>(rotation.x * (3.1415927 / 180));
    eye.rotate_x(radx);
    up.rotate_x(radx);

    type_t rady = static_cast<type_t>(rotation.y * (3.1415927 / 180));
    eye.rotate_y(rady);
    up.rotate_y(rady);

    eye += offset;
    center += offset;
}

void spatial::position::project(const vector& offset, const vector& projection) {
    vector normal = eye - center;

    vector cross = (normal % up) + center;

    vector diff = (cross - center) * offset;

    diff.x = (cross.x - center.x) * offset.x;
    diff.y = (cross.y - center.y) * offset.y;
    diff.z = (cross.z - center.z) * offset.z;
}

spatial::quaternion::quaternion() {

}

spatial::quaternion::quaternion(const matrix& m) {
    type_t tr, s, q[4];
    int i, j, k;
    int nxt[3] = { 1, 2, 0 };
    tr = m[0][0] + m[1][1] + m[2][2];
    // check the diagonal
    if (tr > 0.0) {
        s = sqrt(tr + 1.0);
        w = s / 2.0;
        s = 0.5 / s;
        x = (m[1][2] - m[2][1]) * s;
        y = (m[2][0] - m[0][2]) * s;
        z = (m[0][1] - m[1][0]) * s;
    }
    else {
        // diagonal is negative
        i = 0;
        if (m[1][1] > m[0][0]) i = 1;
        if (m[2][2] > m[i][i]) i = 2;
        j = nxt[i];
        k = nxt[j];
        s = sqrt((m[i][i] - (m[j][j] + m[k][k])) + 1.0);
        q[i] = s * 0.5;
        if (s != 0.0) s = 0.5 / s;
        q[3] = (m[j][k] - m[k][j]) * s;
        q[j] = (m[i][j] + m[j][i]) * s;
        q[k] = (m[i][k] + m[k][i]) * s;
        x = q[0];
        y = q[1];
        z = q[2];
        w = q[3];
    }
}

spatial::quaternion::~quaternion() {

}

spatial::quaternion spatial::quaternion::operator *(const quaternion& operand) {
    quaternion result;

    result.w = w * operand.w - x * operand.x - y * operand.y - z * operand.z;
    result.x = w * operand.x + x * operand.w + y * operand.z - z * operand.y;
    result.y = w * operand.y + y * operand.w + z * operand.x - x * operand.z;
    result.z = w * operand.z + z * operand.w + x * operand.y - y * operand.x;

    return(result);
}

/*
    void quaternion::euler(const type_t &x, const type_t &y, const type_t &z, const type_t &degrees) {
    type_t angle = type_t((degrees / 180.0f) * M_PI);
    type_t result = type_t(sin(angle / 2.0f));
    this->w = type_t(cos(angle / 2.0f));

    // Calculate the x, y and z of the quaternion
    this->x = type_t(x * result);
    this->y = type_t(y * result);
    this->z = type_t(z * result);
    }
*/

void spatial::quaternion::euler(const type_t& roll, const type_t& pitch, const type_t& yaw, const type_t& degrees)
{
    type_t cr, cp, cy, sr, sp, sy, cpcy, spsy;
    // calculate trig identities
    cr = cos(roll / 2);
    cp = cos(pitch / 2);
    cy = cos(yaw / 2);
    sr = sin(roll / 2);
    sp = sin(pitch / 2);
    sy = sin(yaw / 2);
    cpcy = cp * cy;
    spsy = sp * sy;
    w = cr * cpcy + sr * spsy;
    x = sr * cpcy - cr * spsy;
    y = cr * sp * cy + sr * cp * sy;
    z = cr * cp * sy - sr * sp * cy;
}

spatial::quaternion::operator spatial::matrix() {
    matrix result = {
        {  1.0f - 2.0f * (y * y + z * z),  2.0f * (x * y + w * z),         2.0f * (x * z - w * y),         0.0f  },
        {  2.0f * (x * y - w * z),         1.0f - 2.0f * (x * x + z * z),  2.0f * (y * z + w * x),         0.0f  },
        {  2.0f * (x * z + w * y),         2.0f * (y * z - w * x),         1.0f - 2.0f * (x * x + y * y),  0.0f  },
        {  0.0f,                           0.0f,                           0.0f,                           1.0f  }
    };

    return(result);
}

/*
Slerp(QUAT * from, QUAT * to, type_t t, QUAT * res)
{
    type_t           to1[4];
    double        omega, cosom, sinom, scale0, scale1;
    // calc cosine
    cosom = from->x * to->x + from->y * to->y + from->z * to->z
    + from->w * to->w;
    // adjust signs (if necessary)
    if (cosom <0.0) {
    cosom = -cosom; to1[0] = -to->x;
    to1[1] = -to->y;
    to1[2] = -to->z;
    to1[3] = -to->w;
    }
    else {
    to1[0] = to->x;
    to1[1] = to->y;
    to1[2] = to->z;
    to1[3] = to->w;
    }
    // calculate coefficients
    if ((1.0 - cosom) > DELTA) {
    // standard case (slerp)
    omega = acos(cosom);
    sinom = sin(omega);
    scale0 = sin((1.0 - t) * omega) / sinom;
    scale1 = sin(t * omega) / sinom;
    }
    else {
    // "from" and "to" quaternions are very close
    //  ... so we can do a linear interpolation
    scale0 = 1.0 - t;
    scale1 = t;
    }
    // calculate final values
    res->x = scale0 * from->x + scale1 * to1[0];
    res->y = scale0 * from->y + scale1 * to1[1];
    res->z = scale0 * from->z + scale1 * to1[2];
    res->w = scale0 * from->w + scale1 * to1[3];
}
*/
