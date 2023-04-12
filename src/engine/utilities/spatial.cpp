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

//dot = x1 * x2 + y1 * y2 + z1 * z2
//lenSq1 = x1 * x1 + y1 * y1 + z1 * z1
//lenSq2 = x2 * x2 + y2 * y2 + z2 * z2
//angle = acos(dot / sqrt(lenSq1 * lenSq2))

spatial::vector::vector() {
    this->x = 0.0f;
    this->y = 0.0f;
    this->z = 0.0f;
    this->w = 1.0f;
}

spatial::vector::vector(const type_t& v) {
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

spatial::vector::vector(const type_t& x, const type_t& y, const type_t& z, const type_t& w) {
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
spatial::vector spatial::vector::operator + (const vector& operand) const {
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

spatial::vector& spatial::vector::operator() (const type_t& x, const type_t& y, const type_t& z, const type_t& w) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
    return(*this);
}

bool spatial::vector::operator == (const vector& operand) const {
    return this->x == operand.x &&
           this->y == operand.y &&
           this->z == operand.z &&
           this->w == operand.w;
}

bool spatial::vector::operator != (const vector& operand) const {
    return (*this == operand) == false;
}

#if defined _USE_GLM
spatial::vector::operator glm::vec3() const {
    return glm::vec3(x, y, z);
}
#endif

spatial::vector& spatial::vector::rotate(const spatial::vector& axis, type_t rad) {
    return *this = spatial::matrix().rotate(axis, rad).interpolate(*this);
}

spatial::vector& spatial::vector::rotate_x(type_t rad) {
    vector result;
    result.x = (x * 1) + (y * 0) + (z * 0);
    result.y = (x * 0) + (y * cosf(rad)) + (z * sinf(rad));
    result.z = (x * 0) + (y * -sinf(rad)) + (z * cosf(rad));
    return (*this = result);
}

spatial::vector& spatial::vector::rotate_y(type_t rad) {
    vector result;
    result.x = (x * cosf(rad)) + (y * 0) + (z * sinf(rad));
    result.y = (x * 0) + (y * 1) + (z * 0);
    result.z = (x * -sinf(rad)) + (y * 0) + (z * cosf(rad));
    return (*this = result);
}

spatial::vector& spatial::vector::rotate_z(type_t rad) {
    vector result;
    result.x = (x * cosf(rad)) + (y * sinf(rad)) + (z * 0);
    result.y = (x * -sinf(rad)) + (y * cosf(rad)) + (z * 0);
    result.z = (x * 0) + (y * 0) + (z * 1);
    return (*this = result);
}

spatial::vector::type_t spatial::vector::dot(const spatial::vector& operand) const {
    return(x * operand.x + y * operand.y + z * operand.z);
}

spatial::vector::type_t spatial::vector::dot() const {
    return(x * x + y * y + z * z);
}

spatial::vector::type_t spatial::vector::length() const {
    return((type_t)sqrt(dot()));
}

spatial::vector spatial::vector::unit() const {
    return(*this / (vector)length());
}

spatial::vector &spatial::vector::normalize() {
    return(*this = unit());
}

spatial::vector::type_t spatial::vector::distance(const vector& v) const {
    return sqrt(pow(x - v.x, 2) + pow(y - v.y, 2) + pow(z - v.z, 2));
}

spatial::vector spatial::vector::project(const matrix& projection, const matrix& view, const matrix& model) {
    matrix mvp = projection * view * model;
    return mvp.interpolate(*this);
}

spatial::vector spatial::vector::unproject(const matrix& projection, const matrix& view, int width, int height) const {
#if defined _USE_GLM    
    double x0 = 2.0 * x / width - 1;
    double y0 = 2.0 * y / height - 1;

    glm::vec4 screenPos = glm::vec4(x0, -y0, z, 1.0f);

    glm::mat4 proj;
    memcpy(glm::value_ptr(proj), projection.data(), sizeof(type_t) * 16);

    glm::mat4 vw;
    memcpy(glm::value_ptr(vw), view.data(), sizeof(type_t) * 16);

    glm::mat4 ProjectView = proj * vw;
    glm::mat4 viewProjectionInverse = inverse(ProjectView);
    glm::vec4 worldPos = viewProjectionInverse * screenPos;

    return spatial::vector(worldPos.x, worldPos.y, worldPos.z, worldPos.w);
#else
    return (projection * view).invert() * spatial::vector((x / (type_t)(width)) * 2.0f - 1.0f, 1.0f - ((y / (type_t)(height)) * 2.0f), z, w);
#endif
}

float sign_xz(const spatial::vector& v0, const spatial::vector& v1, const spatial::vector& v2)
{
    return (v0.x - v2.x) * (v1.z - v2.z) - (v1.x - v2.x) * (v0.z - v2.z);
}

bool spatial::vector::encompassed_xz(const vector& v0, const vector& v1, const vector& v2) const {
    type_t d1, d2, d3;
    bool has_neg, has_pos;

    d1 = sign_xz(*this, v0, v1);
    d2 = sign_xz(*this, v1, v2);
    d3 = sign_xz(*this, v2, v0);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}

spatial::vector spatial::vector::lerp(const vector& to, const type_t t)
{
    if (t >= 1.0) {
        return *this;
    }
    if (t <= 0.0) {
        return to;
    }

    vector result;
    type_t s = 1.0 - t;
    result.x = s * x + t * to.x;
    result.y = s * y + t * to.y;
    result.z = s * z + t * to.z;
    result.w = s * w + t * to.w;
    return result;
}

spatial::vector spatial::vector::slerp(const vector& to, const type_t t)
{
    if (t >= 1.0) {
        return *this;
    }
    if (t <= 0.0) {
        return to;
    }

    vector result;
    type_t dot = utilities::clamp(this->unit().dot(to.unit()), -1.0, 1.0);
    type_t theta = acos(dot);
    type_t sn = sin(theta);
    type_t s = sin((1.0 - t) * theta) / sn;
    type_t r = sin(t * theta) / sn;
    result.x = s * x + r * to.x;
    result.y = s * y + r * to.y;
    result.z = s * z + r * to.z;
    result.w = s * w + r * to.w;
    return result;
}

spatial::matrix::matrix() {
    identity();
}
spatial::matrix::matrix(const matrix& m) {
    r = m.r;
}
spatial::matrix::matrix(const vector& c0, const vector& c1, const vector& c2, const vector& c3) {
    r[0][0] = c0.x;
    r[0][1] = c1.x;
    r[0][2] = c2.x;
    r[0][3] = c3.x;

    r[1][0] = c0.y;
    r[1][1] = c1.y;
    r[1][2] = c2.y;
    r[1][3] = c3.y;

    r[2][0] = c0.z;
    r[2][1] = c1.z;
    r[2][2] = c2.z;
    r[2][3] = c3.z;

    r[3][0] = c0.w;
    r[3][1] = c1.w;
    r[3][2] = c2.w;
    r[3][3] = c3.w;
}

spatial::matrix& spatial::matrix::identity() {
    r[0][0] = 1.0f;
    r[0][1] = 0.0f;
    r[0][2] = 0.0f;
    r[0][3] = 0.0f;

    r[1][0] = 0.0f;
    r[1][1] = 1.0f;
    r[1][2] = 0.0f;
    r[1][3] = 0.0f;

    r[2][0] = 0.0f;
    r[2][1] = 0.0f;
    r[2][2] = 1.0f;
    r[2][3] = 0.0f;

    r[3][0] = 0.0f;
    r[3][1] = 0.0f;
    r[3][2] = 0.0f;
    r[3][3] = 1.0f;

    return *this;
}

spatial::vector spatial::matrix::interpolate(const spatial::vector& v) const {
    return (*this * v) + spatial::vector(r[3][0], r[3][1], r[3][2], r[3][3]);
}

spatial::geometry spatial::matrix::interpolate(const spatial::geometry& g) const {
    spatial::geometry results = g;
    results.vertices.clear();
    for (auto v : g.vertices) {
        results.vertices.push_back(*this * v);
    }
    return results;
}


spatial::matrix& spatial::matrix::operator *= (const matrix& operand) {
    return (*this = (*this) * operand);
}
spatial::matrix spatial::matrix::operator * (const matrix& operand) const {
    matrix result;
    int k, r, c;
    for (c = 0; c < 4; ++c) for (r = 0; r < 4; ++r) {
        result.r[c][r] = 0.f;
        for (k = 0; k < 4; ++k) {
            result.r[c][r] += this->r[k][r] * operand.r[c][k];
        }
    }
    return result;
}

spatial::matrix& spatial::matrix::scale(const type_t& operand) {
    r[0][0] *= operand;
    r[1][1] *= operand;
    r[2][2] *= operand;
    return *this;
}

spatial::matrix& spatial::matrix::scale_x(const type_t& operand) {
    r[0][0] *= operand;
    return *this;
}

spatial::matrix& spatial::matrix::scale_y(const type_t& operand) {
    r[1][1] *= operand;
    return *this;
}

spatial::matrix& spatial::matrix::scale_z(const type_t& operand) {
    r[2][2] *= operand;
    return *this;
}


spatial::vector spatial::matrix::operator * (const vector& operand) const {
    spatial::vector result;

    matrix current = *this;

    result.x = (current.r[0][0] * operand.x) + (current.r[1][0] * operand.y) + (current.r[2][0] * operand.z) + (current.r[3][0] * operand.w);
    result.y = (current.r[0][1] * operand.x) + (current.r[1][1] * operand.y) + (current.r[2][1] * operand.z) + (current.r[3][1] * operand.w);
    result.z = (current.r[0][2] * operand.x) + (current.r[1][2] * operand.y) + (current.r[2][2] * operand.z) + (current.r[3][2] * operand.w);
    result.w = (current.r[0][3] * operand.x) + (current.r[1][3] * operand.y) + (current.r[2][3] * operand.z) + (current.r[3][3] * operand.w);

    return result;
}

spatial::matrix spatial::matrix::operator * (const matrix::type_t& operand) const {
    matrix result = *this;
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            result[c][r] *= operand;
        }
    }
    return result;
}

spatial::geometry spatial::matrix::operator * (const spatial::geometry& g) const {
    spatial::geometry results = g;
    results.vertices.clear();
    for (auto v : g.vertices) {
        results.vertices.push_back(*this * v);
    }
    return results;
}


// http://fastgraph.com/makegames/3drotation/
spatial::matrix& spatial::matrix::rotate(const vector& axis, type_t rad) {

    type_t c = cosf(rad);
    type_t s = sinf(rad);
    type_t t = 1 - c;

    matrix rotation;

    rotation.r[0][0] = (t * pow(axis.x, 2)) + c;
    rotation.r[0][1] = (t * axis.x * axis.y) + (s * axis.z);
    rotation.r[0][2] = (t * axis.x * axis.z) + (s * axis.y);
    rotation.r[0][3] = 0.0f;

    rotation.r[1][0] = (t * axis.x * axis.y) + (s * axis.z);
    rotation.r[1][1] = (t * pow(axis.y, 2)) + c;
    rotation.r[1][2] = (t * axis.y * axis.z) + (s * axis.x);
    rotation.r[1][3] = 0.0f;

    rotation.r[2][0] = (t * axis.x * axis.z) + (s * axis.y);
    rotation.r[2][1] = (t * axis.y * axis.z) + (s * axis.x);
    rotation.r[2][2] = (t * pow(axis.z, 2)) + c;
    rotation.r[2][3] = 0.0f;

    rotation.r[3][0] = 0.0f;
    rotation.r[3][1] = 0.0f;
    rotation.r[3][2] = 0.0f;
    rotation.r[3][3] = 1.0f;

    return(*this *= rotation);
}
spatial::matrix& spatial::matrix::rotate_x(type_t rad) {
    matrix rotation;

    rotation.r[0][0] = 1.0f;
    rotation.r[0][1] = 0.0f;
    rotation.r[0][2] = 0.0f;
    rotation.r[0][3] = 0.0f;

    rotation.r[1][0] = 0.0f;
    rotation.r[1][1] = cosf(rad);
    rotation.r[1][2] = sinf(rad);
    rotation.r[1][3] = 0.0f;

    rotation.r[2][0] = 0.0f;
    rotation.r[2][1] = -sinf(rad);
    rotation.r[2][2] = cosf(rad);
    rotation.r[2][3] = 0.0f;

    rotation.r[3][0] = 0.0f;
    rotation.r[3][1] = 0.0f;
    rotation.r[3][2] = 0.0f;
    rotation.r[3][3] = 1.0f;

    return(*this *= rotation);
}
spatial::matrix& spatial::matrix::rotate_y(type_t rad) {
    matrix rotation;

    rotation.r[0][0] = cosf(rad);
    rotation.r[0][1] = 0.0f;
    rotation.r[0][2] = -sinf(rad);
    rotation.r[0][3] = 0.0f;

    rotation.r[1][0] = 0.0f;
    rotation.r[1][1] = 1.0f;
    rotation.r[1][2] = 0.0f;
    rotation.r[1][3] = 0.0f;

    rotation.r[2][0] = sinf(rad);
    rotation.r[2][1] = 0.0f;
    rotation.r[2][2] = cosf(rad);
    rotation.r[2][3] = 0.0f;

    rotation.r[3][0] = 0.0f;
    rotation.r[3][1] = 0.0f;
    rotation.r[3][2] = 0.0f;
    rotation.r[3][3] = 1.0f;

    return(*this *= rotation);
}
spatial::matrix& spatial::matrix::rotate_z(type_t rad) {
    matrix rotation;

    rotation.r[0][0] = cosf(rad);
    rotation.r[0][1] = sinf(rad);
    rotation.r[0][2] = 0.0f;
    rotation.r[0][3] = 0.0f;

    rotation.r[1][0] = -sinf(rad);
    rotation.r[1][1] = cosf(rad);
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

spatial::matrix& spatial::matrix::position(const vector& v) {
    return(this->position(v.x, v.y, v.z, v.w));
}
spatial::matrix& spatial::matrix::position(const type_t& x, const type_t& y, const type_t& z, const type_t& w) {
    r[3][0] = x;
    r[3][1] = y;
    r[3][2] = z;
    return *this;
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
#if defined _USE_GLM
    auto rad = fov * (float)M_PI / 180.0f;
    glm::mat4 projection = glm::perspective(rad, aspect, n, f);
    memcpy(r.x, glm::value_ptr(projection), sizeof(type_t) * 16);
    return *this;
#else
    auto rad = fov * (float)M_PI / 180.0f;

    type_t const a = 1.0f / (type_t)tan(rad / 2.0f);

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
#endif
}

spatial::matrix& spatial::matrix::ortho(type_t left, type_t right, type_t bottom, type_t top, type_t n, type_t f) {
    r[0][0] = 2.0f / (right - left);
    r[0][1] = 0.0f;
    r[0][2] = 0.0f;
    r[0][3] = 0.0f;

    r[1][0] = 0.0f;
    r[1][1] = 2.0f / (top - bottom);
    r[1][2] = 0.0f;
    r[1][3] = 0.0f;

    r[2][0] = 0.0f;
    r[2][1] = 0.0f;
    r[2][2] = -2.0f / (f - n);
    r[2][3] = 0.0f;

    r[3][0] = -(right + left) / (right - left);
    r[3][1] = -(top + bottom) / (top - bottom);
    r[3][2] = -(f + n) / (f - n);
    r[3][3] = 1.0f;

    return *this;
}

spatial::matrix& spatial::matrix::lookat(const vector& eye, const vector& focus, const vector& up) {
#if defined _USE_GLM
    glm::vec3 e(eye.x, eye.y, eye.z);
    glm::vec3 c(focus.x, focus.y, focus.z);
    glm::vec3 u(up.x, up.y, up.z);

    glm::mat4 lat = glm::lookAt(e, c, u);

    memcpy(r.x, glm::value_ptr(lat), sizeof(type_t) * 16);

    return *this;
#else
    vector f = (focus - eye).unit();
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
#endif
}

spatial::matrix& spatial::matrix::translate(const vector& eye, const vector& focus, const vector& up) {
    *this = spatial::quaternion().translate(eye, focus, up);
    return position(eye);
}

spatial::matrix& spatial::matrix::invert() {
#if defined _USE_GLM
    glm::mat4 current;
    memcpy(glm::value_ptr(current), r.x, sizeof(type_t) * 16);
    auto result = glm::inverse(current);
    memcpy(r.x, glm::value_ptr(result), sizeof(type_t) * 16);

    return *this;
#else
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

    // assumes it is invertible
    type_t i = 1.0f / (s[0] * c[5] - s[1] * c[4] + s[2] * c[3] + s[3] * c[2] - s[4] * c[1] + s[5] * c[0]);

    matrix result;

    result.r[0][0] = (r[1][1] * c[5] - r[1][2] * c[4] + r[1][3] * c[3]) * i;
    result.r[0][1] = (-r[0][1] * c[5] + r[0][2] * c[4] - r[0][3] * c[3]) * i;
    result.r[0][2] = (r[3][1] * s[5] - r[3][2] * s[4] + r[3][3] * s[3]) * i;
    result.r[0][3] = (-r[2][1] * s[5] + r[2][2] * s[4] - r[2][3] * s[3]) * i;

    result.r[1][0] = (-r[1][0] * c[5] + r[1][2] * c[2] - r[1][3] * c[1]) * i;
    result.r[1][1] = (r[0][0] * c[5] - r[0][2] * c[2] + r[0][3] * c[1]) * i;
    result.r[1][2] = (-r[3][0] * s[5] + r[3][2] * s[2] - r[3][3] * s[1]) * i;
    result.r[1][3] = (r[2][0] * s[5] - r[2][2] * s[2] + r[2][3] * s[1]) * i;

    result.r[2][0] = (r[1][0] * c[4] - r[1][1] * c[2] + r[1][3] * c[0]) * i;
    result.r[2][1] = (-r[0][0] * c[4] + r[0][1] * c[2] - r[0][3] * c[0]) * i;
    result.r[2][2] = (r[3][0] * s[4] - r[3][1] * s[2] + r[3][3] * s[0]) * i;
    result.r[2][3] = (-r[2][0] * s[4] + r[2][1] * s[2] - r[2][3] * s[0]) * i;

    result.r[3][0] = (-r[1][0] * c[3] + r[1][1] * c[1] - r[1][2] * c[0]) * i;
    result.r[3][1] = (r[0][0] * c[3] - r[0][1] * c[1] + r[0][2] * c[0]) * i;
    result.r[3][2] = (-r[3][0] * s[3] + r[3][1] * s[1] - r[3][2] * s[0]) * i;
    result.r[3][3] = (r[2][0] * s[3] - r[2][1] * s[1] + r[2][2] * s[0]) * i;

    return *this = result;
#endif
}

spatial::matrix& spatial::matrix::pivot() {
    type_t t;
    int c, r;
    for(c=0; c<4; c++) for(r=0; r<4; r++) {
        t = this->r[r][c];
        this->r[r][c] = this->r[c][r];
        this->r[c][r] = t;
    }
    return *this;
}


spatial::matrix& spatial::matrix::set(int row, int col, type_t value) {
    r[row][col] = value;
    return *this;
}


spatial::quaternion::quaternion() {

}

spatial::quaternion::quaternion(type_t v[]) {
    this->x = v[0];
    this->y = v[1];
    this->z = v[2];
    this->w = v[3];
    normalize();
}

spatial::quaternion::quaternion(const type_t& x, const type_t& y, const type_t& z, const type_t& w) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
    normalize();
}

spatial::quaternion::quaternion(const vector& v) {
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
    normalize();
}

spatial::quaternion::quaternion(const matrix& m) {
    type_t tr, s, q[4];
    int i, j, k;
    int nxt[3] = { 1, 2, 0 };
    tr = m[0][0] + m[1][1] + m[2][2];
    
    if (tr > 0.0) {
        s = sqrt(tr + 1.0);
        w = s / 2.0;
        s = 0.5 / s;
        x = (m[1][2] - m[2][1]) * s;
        y = (m[2][0] - m[0][2]) * s;
        z = (m[0][1] - m[1][0]) * s;
    }
    else {
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

// https://stackoverflow.com/questions/52413464/look-at-quaternion-using-up-vector
spatial::quaternion& spatial::quaternion::translate(const vector& eye, const vector& center, const vector& up) {
    vector f = (eye - center).unit();
    vector s = (up % f).unit();
    vector t = f % s;

    double tr = s.x + t.y + f.z;
    if (tr > 0.0) {
        double l = 0.5 / sqrt(tr + 1.0);
        w = 0.25 / l;
        x = (t.z - f.y) * l;
        y = (f.x - s.z) * l;
        z = (s.y - t.x) * l;
    }
    else {
        if (s.x > t.y && s.x > f.z) {
            double l = 2.0 * sqrt(1.0 + s.x - t.y - f.z);
            w = (t.z - f.y) / l;
            x = 0.25 * l;
            y = (t.x + s.y) / l;
            z = (f.x + s.z) / l;
        }
        else if (t.y > f.z) {
            double l = 2.0 * sqrt(1.0 + t.y - s.x - f.z);
            w = (f.x - s.z) / l;
            x = (t.x + s.y) / l;
            y = 0.25 * l;
            z = (f.y + t.z) / l;
        }
        else {
            double l = 2.0 * sqrt(1.0 + f.z - s.x - t.y);
            w = (s.y - t.x) / l;
            x = (f.x + s.z) / l;
            y = (f.y + t.z) / l;
            z = 0.25 * l;
        }
    }
    return *this;
}

// https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
spatial::quaternion& spatial::quaternion::euler(const type_t& roll, const type_t& pitch, const type_t& yaw, const type_t& degrees)
{
    type_t cy = cosf(yaw * 0.5);
    type_t sy = sinf(yaw * 0.5);
    type_t cp = cosf(pitch * 0.5);
    type_t sp = sinf(pitch * 0.5);
    type_t cr = cosf(roll * 0.5);
    type_t sr = sinf(roll * 0.5);

    w = cr * cp * cy + sr * sp * sy;
    x = sr * cp * cy - cr * sp * sy;
    y = cr * sp * cy + sr * cp * sy;
    z = cr * cp * sy - sr * sp * cy;

    return *this;
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

spatial::quaternion::operator spatial::matrix() {
    unit();

    matrix result;
    
    //result[0][0] = 1.0f;
    result[0][0] = 1.0f - 2.0f * (y * y + z * z);
    result[0][1] = 2.0f * (x * y + w * z);
    result[0][2] = 2.0f * (x * z - w * y);
    result[0][3] = 0.0f;

    result[1][0] = 2.0f * (x * y - w * z);
    //result[1][1] = 1.0f;
    result[1][1] = 1.0f - 2.0f * (x * x + z * z);
    result[1][2] = 2.0f * (y * z + w * x);
    result[1][3] = 0.0f;

    result[2][0] = 2.0f * (x * z + w * y);
    result[2][1] = 2.0f * (y * z - w * x);
    result[2][2] = 1.0f - 2.0f * (x * x + y * y);
    result[2][3] = 0.0f;

    result[3][0] = 0.0f;
    result[3][1] = 0.0f;
    result[3][2] = 0.0f;
    result[3][3] = 1.0f;

    return(result);
}

spatial::quaternion spatial::quaternion::lerp(const quaternion& q, const type_t t)
{
    return this->vector::lerp(q, t).unit();
}

spatial::quaternion spatial::quaternion::slerp(const quaternion& q, const type_t t)
{
    return this->vector::slerp(q, t).unit();
}


spatial::position::position(void) {
    identity();
}

spatial::position::position(const position& ref) {
    *this = ref;
    this->modify();
}

spatial::position::position(const spatial::vector& pos) {
    identity();
    reposition(pos);
}

spatial::position::operator spatial::matrix() {
    matrix result;
    result.translate(eye, focus, up);
    return result; //  * spatial::matrix().scale(translation.scale);
}

spatial::position& spatial::position::scale(type_t value) {
    matrix result;
    result.translate(eye, focus, up);
    translation.scale += value;
    //return result * spatial::matrix().scale(translation.scale);
    return this->modify();
}

spatial::matrix& spatial::position::serialize() {
    if (dirty) {
		state.identity();
		state.translate(eye, focus, up);
		state *= spatial::matrix().scale(translation.scale);
		dirty = false;
	}
    return state;
}

void spatial::position::identity(void) {
    eye.x = 0.0f;
    eye.y = 0.0f;
    eye.z = 0.0f;

    focus.x = 0.0f;
    focus.y = 0.0f;
    focus.z = -1.0f;

    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
}

void spatial::position::viewable(bool toggle) {
    view = toggle;
}

spatial::position& spatial::position::surge(type_t t) {
    store scope(*this);

    vector diff = (eye - focus).unit() * t;

    focus += diff;
    eye += diff;

    return this->modify();
}

spatial::position& spatial::position::sway(type_t t) {
    store scope(*this);

    vector normal = eye - focus;
    vector cross = (normal % up) + focus;
    vector diff = (cross - focus) * t;

    focus += diff;
    eye += diff;

    return this->modify();
}

spatial::position& spatial::position::heave(type_t t) {
    store scope(*this);

    vector diff = up * t;

    focus += diff;
    eye += diff;

    return this->modify();
}

spatial::position& spatial::position::pitch(type_t angle, bool apply) {
    translation.pitch = apply ? translation.pitch + angle : angle;
    return apply ? rotate() : this->modify();
}

spatial::position& spatial::position::yaw(type_t angle, bool apply) {
    translation.yaw = apply ? translation.yaw + angle : angle;
    return apply ? rotate() : this->modify();
}

spatial::position& spatial::position::roll(type_t angle, bool apply) {
    translation.roll = apply ? translation.roll + angle : angle;
    return apply ? rotate() : this->modify();
}

spatial::position& spatial::position::spin(type_t angle, bool apply) {
    translation.spin = apply ? translation.spin + angle : angle;
    return apply ? rotate() : this->modify();
}

spatial::position& spatial::position::rotate() {
    store scope(*this);

    vector offset = eye;

    identity();

    type_t radx, rady, radz;

    radx = static_cast<type_t>(translation.pitch * (3.1415927 / 180));
    focus.rotate_x(radx);
    up.rotate_x(radx);

    rady = static_cast<type_t>(translation.spin * (3.1415927 / 180));
    focus.rotate_y(rady);
    up.rotate_y(rady);

    radz = static_cast<type_t>(translation.roll * (3.1415927 / 180));
    focus.rotate_z(radz);
    up.rotate_z(radz);

    eye += offset;
    focus += offset;

    return this->modify();
}

void spatial::position::project(const vector& offset, const vector& projection) {
    store scope(*this);

    vector normal = eye - focus;

    vector cross = (normal % up) + focus;

    vector diff = (cross - focus) * offset;

    diff.x = (cross.x - focus.x) * offset.x;
    diff.y = (cross.y - focus.y) * offset.y;
    diff.z = (cross.z - focus.z) * offset.z;
}

spatial::position& spatial::position::orientation(const spatial::position& reference) {
    memcpy(&this->translation, &reference.translation, sizeof(reference.translation));
    this->rotate();
    return this->modify();
}

spatial::position& spatial::position::reposition(const vector& offset) {
    store scope(*this);

    focus += offset - eye;
    eye = offset;

    return this->modify();
}

spatial::position& spatial::position::move(const vector& offset) {
    store scope(*this);

    reposition(eye + offset);

    return this->modify();
}
spatial::position& spatial::position::lookat(const vector& offset) {
    store scope(*this);

    auto forward = (offset - eye).unit();
    auto right = forward == vector(0, -1, 0) ? vector(-1, 0, 0) : vector(0, 1, 0) % forward;

    focus = forward + eye;
    up = (forward % right).unit();

    translation.pitch = 0;
    translation.roll = 0;
    translation.spin = 0;
    translation.yaw = 0;

    return this->modify();
}

spatial::position& spatial::position::rotate(const spatial::position& axis) {
    store scope(*this);

    auto offset = eye - axis.eye;
    reposition(offset);

    type_t radx, rady, radz;

    radx = static_cast<type_t>(axis.translation.pitch * (3.1415927 / 180));
    focus.rotate_x(radx);
    up.rotate_x(radx);
    eye.rotate_x(radx);

    rady = static_cast<type_t>(axis.translation.spin * (3.1415927 / 180));
    focus.rotate_y(rady);
    up.rotate_y(rady);
    eye.rotate_y(rady);

    radz = static_cast<type_t>(axis.translation.roll * (3.1415927 / 180));
    focus.rotate_z(radz);
    up.rotate_z(radz);
    eye.rotate_z(radz);

    eye += axis.eye;
    focus += axis.eye;

    return this->modify();
}

spatial::position& spatial::position::opacity(float a) {
    alpha = a;
    return this->modify();
}

void spatial::position::constrain(bool x, bool y, bool z) {
    constraint.x = x;
    constraint.y = y;
    constraint.z = z;
}

void spatial::position::apply(const spatial::position& reference) {
    if (constraint.x) {
        this->eye.x = reference.eye.x;
        this->focus.x = reference.focus.x;
        this->up.x = reference.up.x;
    }
    if (constraint.y) {
        this->eye.y = reference.eye.y;
        this->focus.y = reference.focus.y;
        this->up.y = reference.up.y;
    }
    if (constraint.z) {
        this->eye.z = reference.eye.z;
        this->focus.z = reference.focus.z;
        this->up.z = reference.up.z;
    }
    this->modify();
}

spatial::vector spatial::position::x(type_t magnitude) {
    return eye - spatial::vector(magnitude, 0, 0);
}
spatial::vector spatial::position::y(type_t magnitude) {
    return eye - spatial::vector(0, magnitude, 0);
}
spatial::vector spatial::position::z(type_t magnitude) {
    return eye - spatial::vector(0, 0, magnitude);
}

spatial::vector spatial::position::forward() {
    return (eye - focus).unit();
}

spatial::vector spatial::position::down() {
    return y();
}

spatial::vector spatial::position::tanget() {
    vector normal = eye - focus;
    return (normal % up) + focus;
}

spatial::position& spatial::position::operator=(const spatial::position& ref) {
    this->eye = ref.eye;
    this->focus = ref.focus;
    this->up = ref.up;

    this->constraint = ref.constraint;
    this->translation = ref.translation;
    
    this->rate = ref.rate;
    this->alpha = ref.alpha;
    this->view = ref.view;
    
    this->dirty = true;
    
    return *this;
}

spatial::sphere::sphere(int horizontal, int vertical) : sphere() {
    interpolate(horizontal, vertical);
}

// http://www.songho.ca/opengl/gl_sphere.html
spatial::sphere &spatial::sphere::interpolate(int horizontal, int vertical) {
    float x, y, z, xy;

    float hs = 2 * M_PI / horizontal;
    float vs = M_PI / vertical;
    float ha, va;

    // Calculate the vertices based on (0, 0, 0)
    std::vector<spatial::vector> prior;
    for (int i = 0; i <= vertical; ++i)
    {
        va = M_PI / 2 - i * vs;
        xy = radius * cosf(va);
        z = radius * sinf(va);

        std::vector<spatial::vector> current;
        for (int j = 0; j <= horizontal; ++j)
        {
            ha = j * hs;

            x = xy * cosf(ha);
            y = xy * sinf(ha);

            current.push_back({ x, z, y });
            if (prior.size() && j > 0) {
                int left = j - 1;
                int right = j;

                vertices.push_back(prior[left]);
                vertices.push_back(current[left]);
                vertices.push_back(current[left]);
                vertices.push_back(prior[right]);
                vertices.push_back(prior[right]);
                vertices.push_back(prior[left]);

                vertices.push_back(current[left]);
                vertices.push_back(current[right]);
                vertices.push_back(current[right]);
                vertices.push_back(prior[right]);
                vertices.push_back(prior[right]);
                vertices.push_back(current[left]);
            }
        }
        prior.clear();
        std::copy(current.begin(), current.end(), std::back_inserter(prior));
    }

    // Transpose to the center position
    for (auto& v : vertices) {
        v += center;
    }

    return *this;
}

spatial::quad::quad(int width, int height) : quad() {
    spatial::quad::interpolate(width, height);
}

spatial::quad &spatial::quad::interpolate(int width, int height) {
    //this->width = width;
    //this->height = height;

    int factor = 1;

    vertices.resize(6 * factor * factor);

    type_t x = 0.0f;
    type_t y = 0.0f;

    type_t dx = width / factor;
    type_t dy = height / factor;

    int index = 0;

    for (int i = 0; i < factor; i++) {
        x = dx * i;
        for (int j = 0; j < factor; j++) {
            y = dy * j;

            vertices[index++](x + dx, y + dy);
            vertices[index++](x, y + dy);
            vertices[index++](x, y);

            vertices[index++](x + dx, y + dy);
            vertices[index++](x, y);
            vertices[index++](x + dx, y);
        }
    }

    return *this;
}

spatial::quad& spatial::quad::project(const matrix& model, const matrix& view, const matrix& projection) {
    matrix mvp = projection * view * model;
    for (auto& v : vertices) {
        v = mvp.interpolate(v);
    }
    return *this;
}

spatial::geometry spatial::quad::edges(int width, int height) {
    spatial::geometry rays;

    rays.vertices.push_back(spatial::vector(0.0f, 0.0f));
    rays.vertices.push_back(spatial::vector(width, 0.0f));

    rays.vertices.push_back(spatial::vector(width, 0.0f));
    rays.vertices.push_back(spatial::vector(width, height));

    rays.vertices.push_back(spatial::vector(width, height));
    rays.vertices.push_back(spatial::vector(0.0f, height));

    rays.vertices.push_back(spatial::vector(0.0f, height));
    rays.vertices.push_back(spatial::vector(0.0f, 0.0f));

    return rays;
}

spatial::ray::ray(const vector& point, const vector& normal) : ray() {
    vertices.resize(2);
    vertices[0] = point;
    vertices[1] = normal;
}

spatial::ray::ray(const vector& point, const matrix& perspective, const matrix& view, const int& w, const int& h) {
    projection(point, perspective, view, w, h);
}

spatial::ray& spatial::ray::interpolate(const vector& origin, const vector& terminus) {
    vertices.resize(2);
    vertices[0] = origin;
    vertices[1] = (terminus - origin).unit(); // Set as direction vector
    return *this;
}

spatial::ray& spatial::ray::projection(const vector& point, const matrix& perspective, const matrix& view, const int& w, const int& h) {
    vertices.resize(2);
    vertices[0] = spatial::vector({ point.x, point.y }).unproject(perspective, view, w, h);
    vertices[1] = spatial::vector({ point.x, point.y, 1.0f }).unproject(perspective, view, w, h);
    return *this;
}

spatial::ray::type_t spatial::ray::distance(const spatial::ray& r) {
    // TODO: needs implementation
    return 0.0f;
}

spatial::ray::type_t spatial::ray::distance(const spatial::vector& v) const {
    auto translated = vertices[0] + vertices[1];

    auto p1 = v - vertices[0];
    auto p2 = v - translated;

    auto c = p1 % p2;

    return c.length() / vertices[1].length();
}

// TODO: make this smarter... and share the implementation with the intersection method
bool spatial::ray::intersects(const spatial::geometry& bounds) {
    int vertices = bounds.vertices.size();
    int triangles = vertices / 3;
    for (int i = 0, index = 0; i < triangles; i++, index += 3) {
        spatial::triangle tri(bounds.vertices[index], bounds.vertices[index + 1], bounds.vertices[index + 2]);
        if (intersects(tri)) {
            return true;
        }
    }
    return false;
}

bool spatial::ray::intersects(const spatial::triangle& triangle) {

#if defined _USE_GLM
    glm::vec3 r0 = vertices[0];
    glm::vec3 r1 = vertices[1];

    glm::vec3 dir = glm::normalize(r1 - r0);

    glm::vec3 v0 = triangle.vertices[0];
    glm::vec3 v1 = triangle.vertices[1];
    glm::vec3 v2 = triangle.vertices[2];

    glm::vec3 pos;  // no current use for the barycentric coordinates

    return glm::intersectLineTriangle(r0, dir, v0, v1, v2, pos);
#else
    // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm

    auto org = vertices[0];
    auto dir = (vertices[1] - vertices[0]).unit();

    auto e1 = triangle.vertices[1] - triangle.vertices[0];
    auto e2 = triangle.vertices[2] - triangle.vertices[0];

    auto h = dir % e2;

    auto a = e1.dot(h);
    if (a > -0.000001f && a < 0.000001f) {
        return false;
    }

    auto f = 1.0 / a;
    auto s = org - triangle.vertices[0];
    auto u = f * s.dot(h);
    if (u < 0.0f || u > 1.0f) {
        return false;
    }

    auto q = s.cross(e1);
    auto v = f * dir.dot(q);
    if (v < 0.0 || u + v > 1.0) {
        return false;
    }

    auto t = f * e2.dot(q);
    if (t > 0.000001f) {
        auto v = org + dir * t;
        return true;
    }

    return false;
#endif
}

bool spatial::ray::intersects(const spatial::plane& plane) {
    auto d = vertices[1] - vertices[0];
    auto w = vertices[0] - plane.point;

    auto a = -plane.normal.dot(w);
    auto b = plane.normal.dot(d);

    if (fabs(b) < 0.00001f) {
        return false;
    }

    return true;
}


bool spatial::ray::intersects(const spatial::sphere& sphere) {
    return distance(sphere.center) < sphere.radius;
}

spatial::vector spatial::ray::intersection(const spatial::geometry& bounds) {
    int vertices = bounds.vertices.size();
    int triangles = vertices / 3;
    for (int i = 0, index = 0; i < triangles; i++, index += 3) {
        spatial::triangle tri(bounds.vertices[index], bounds.vertices[index+1], bounds.vertices[index+2]);
        if (intersects(tri)) {
            return intersection(tri);
        }
    }
    return spatial::vector();
}

spatial::vector spatial::ray::intersection(const spatial::triangle& triangle) {

    // For now assuming callers check intersection first

    spatial::plane plane;

    plane.point = triangle.vertices[0];
    plane.normal = triangle.normal();

    return intersection(plane);
}

// https://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection
spatial::vector spatial::ray::intersection(const spatial::plane& p) {
    auto d = vertices[1].unit().dot(p.normal.unit());
    if (abs(d) > 0.00001f) {
        auto t = (p.point - vertices[0]).dot(p.normal.unit()) / d;
        return vertices[0] + vertices[1].unit() * t;
    }
    return spatial::vector();
}

void spatial::ray::extend(const spatial::ray::type_t& d) {
//    auto slope = vertices[1] / vertices[0];
//    vertices[1] = vertices[0] * d;
}

spatial::vector spatial::triangle::normal() const {
    auto e1 = vertices[1] - vertices[0];
    auto e2 = vertices[2] - vertices[0];
    return (e1 % e2).unit();
}

spatial::triangle& spatial::triangle::project(const matrix& model, const matrix& view, const matrix& projection) {
    matrix mvp = projection * view * model;
    for (auto& v : vertices) {
        v = mvp.interpolate(v);
    }
    return *this;
}
