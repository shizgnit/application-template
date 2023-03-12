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

std::vector<int> utilities::range(int elements) {
    std::vector<int> results(elements);
    for (int i = 0; i < elements; i++) {
        results[i] = i;
    }
    return results;
}
std::vector<int> utilities::range(int start, int end) {
    std::vector<int> results(end - start + 1);
    for (int i = start; i <= end; i++) {
        results[i-start] = i;
    }
    return results;
}


std::string utilities::rtrim(std::string in) {
    size_t back = in.length() - 1;
    while (back != -1 && (in[back] == 0x09 || in[back] == 0x20 || in[back] == 0x0A || in[back] == 0x0D)) {
        back -= 1;
    }
    if (back < in.length()) {
        return(in.substr(0, back + 1));
    }
    return(in);
}

std::string utilities::ltrim(std::string in) {
    size_t front = 0;
    while (front < in.length() && (in[front] == 0x09 || in[front] == 0x20 || in[front] == 0x0A || in[front] == 0x0D)) {
        front += 1;
    }
    if (front > 0) {
        return(in.substr(front, in.length() - front));
    }
    return(in);
}

std::string utilities::trim(std::string in) {
    size_t front = 0;
    while (front < in.length() && (in[front] == 0x09 || in[front] == 0x20 || in[front] == 0x0A || in[front] == 0x0D)) {
        front += 1;
    }
    size_t back = in.length() - 1;
    while (back != -1 && (in[back] == 0x09 || in[back] == 0x20 || in[back] == 0x0A || in[back] == 0x0D)) {
        back -= 1;
    }
    if (front < back) {
        return(in.substr(front, back - front + 1));
    }
    return(in);
}

std::string utilities::lc(std::string in) {
    std::vector<char> translated(in.length());
    for (size_t i = 0; i < in.length(); i++) {
        translated[i] = std::tolower(in[i]);
    }
    return(std::string(translated.data(), translated.size()));
}

std::string utilities::uc(std::string in) {
    std::vector<char> translated(in.length());
    for (size_t i = 0; i < in.length(); i++) {
        translated[i] = toupper(in[i]);
    }
    return(std::string(translated.data(), translated.size()));
}

bool utilities::numeric(std::string in) {
    return in.find_first_not_of("-0123456789.") == std::string::npos;
}

double utilities::clamp(double value, double min, double max) {
    if (value < min) {
        return min;
    }
    if (value > max) {
        return max;
    }
    return value;
}

std::string utilities::extension(const std::string& in) {
    std::string ext;
    auto dot = in.find_last_of(".");
    if (dot != std::string::npos) {
        ext = in.substr(dot + 1, in.length() - dot - 1);
    }
    return ext;
}


/*
std::string utilities::format(std::string str, ...) {
    std::string result;

    va_list args;
    va_start(args, str);

    char buffer[1024];
    memset(buffer, 0, 1024);

    sprintf(buffer, str.c_str(), args);

    va_end(args);

    result = buffer;

    return(result);
}
*/

std::string utilities::dirname(const std::string& in) {
    auto pos = in.find_last_of("\\/");
    if (pos != std::string::npos) {
        return(in.substr(0, pos));
    }
    return("");
}
std::string utilities::basename(const std::string& in) {
    auto pos = in.find_last_of("\\/");
    if (pos != std::string::npos) {
        return(in.substr(pos + 1, in.length() - pos - 1));
    }
    return(in);
}


std::string utilities::join(std::string delimiter, std::vector<std::string> arguments) {
    std::string result;

    std::vector<std::string>::iterator it = arguments.begin();
    while (it != arguments.end()) {
        result.append(*it);
        if (++it != arguments.end()) {
            result.append(delimiter);
        }
    }

    return(result);
}

/*
std::vector<std::string> utilities::split(std::string expression, std::string input, int limit) {
    std::vector<std::string> results;
    
    std::regex regex(expression);

    int iteration = 1;

    std::string buffer = input;

    do {
        vector< pair<int, int> > match = regex.detailed_match(buffer);

        if (!match.size()) { break; }

        int match_start = match[0].first;
        int match_end = match[0].second;
        int match_length = match[0].second - match[0].first;

        results.push(buffer.substr(0, match_start));

        buffer = buffer.substr(match_end);

    } while (++iteration != limit);

    results.push(buffer);

    return(results);
}
*/

std::vector<std::string> utilities::tokenize(std::string input, std::string delimiter, int tokens) {
    std::vector<std::string> results;

    bool newlines = false;
    if (delimiter.empty()) {
        delimiter = "\n";
        newlines = true;
    }

    size_t start = 0, end;
    while ((tokens == 0 || results.size() < tokens) && (end = input.find(delimiter, start)) != std::string::npos) {
        if (newlines && input[end - 1] == '\r') {
            results.push_back(input.substr(start, end - 1 - start));
        }
        else {
            results.push_back(input.substr(start, end - start));
        }
        start = end + delimiter.length();
    }
    results.push_back(input.substr(start, input.length() - start));

    return(results);
}

std::string utilities::replace(std::string input, std::string label, std::string value) {
    std::string results = input;
    size_t pos;
    while ((pos = results.find(label)) != std::string::npos) {
        results.replace(pos, label.length(), value);
    }
    return results;
}


/*
std::string utilities::substitute(std::string input, std::string expression, std::string replacement, bool global) {
    size_t length = replacement.length();

    std::string buffer = input;

    std::regex regex(expression);

    size_t offset = 0;

    do {
        if (offset > buffer.length()) {
            break;
        }
        std::vector< std::pair<int, int> > match = regex.detailed_match(buffer.substr(offset));

        if (!match.size()) { break; }

        int match_start = match[0].first + offset;
        int match_end = match[0].second + offset;
        int match_length = match[0].second - match[0].first;

        if (match_length <= 0) { break; }

        std::string temporary = join("", (buffer.substr(0, match_start),
            replacement,
            buffer.substr(match_end)));

        buffer = temporary;

        offset = match_start + length;
    } while (global);

    return(buffer);
}
*/

/*
std::string utilities::substitute(std::string input, std::vector<std::string>& replacement) {
    std::string buffer = input;

    std::string expression = "%";
    for (int i = 0; i < (int)replacement.size(); i++) {
        //std::string key = join("", (expression, utilities::type_cast<std::string>(i)));
        //buffer = utilities::substitute(buffer, key, replacement[i], true);
    }

    return(buffer);
}
*/

std::string utilities::base(std::string in, int target_ordinal, int current_ordinal) {
    static const char digits[63] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    if (target_ordinal == 64 && current_ordinal == 10) {
        return(base64(in));
    }

    std::string value;

    if (current_ordinal < 2 || current_ordinal > 62) {
        //throw(Exception(L"BASE_INVALID_ORDINAL", __FILE__, __LINE__));
    }
    if (target_ordinal < 2 || target_ordinal > 62) {
        //throw(Exception(L"BASE_INVALID_TARGET_ORDINAL", __FILE__, __LINE__));
    }

    if (current_ordinal == 10) {
        unsigned long decimal = utilities::type_cast<unsigned long>(in);
        char buffer[256];
        char* index = buffer;
        while (decimal > 0) {
            *index++ = digits[decimal % target_ordinal];
            decimal /= target_ordinal;
        }
        *index = '\0';
        if (index != buffer) {
            char reverse[256];
            index = reverse;
            size_t i = strlen(buffer);
            while (1) {
                *index++ = buffer[--i];
                if (i == 0) {
                    break;
                }
            }
            *index = '\0';
            value = reverse;
        }
        else {
            value = "0";
        }
        return(value);
    }
    else {
        const char* source = in.c_str();
        unsigned long pos = (unsigned long)pow((float)current_ordinal, (int)(strlen(source) - 1));

        unsigned long decimal = 0;

        while (*source) {
            char* index = (char*)digits;
            while (*index) {
                if (*index++ == *source) {
                    break;
                }
            }
            decimal += (index - digits - 1) * pos;
            source++;
            pos /= current_ordinal;
        }

        if (target_ordinal != 10) {
            value = base(type_cast<std::string>(decimal), target_ordinal);
        }
        else {
            value = type_cast<std::string>(decimal);
        }
        return(value);
    }
}

std::string utilities::base64(std::string in) {
    static const char base64digits[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    size_t size = in.length();

    unsigned char* output = (unsigned char*)malloc((size / 3) * 4);
    unsigned char* ptr = output;

    for (unsigned int i = 0; i < size; i += 3) {
        unsigned long value = 0;
        value += (in[i] << 16);
        if (i + 1 < size) value += (in[i + 1] << 8);
        if (i + 2 < size) value += (in[i + 2] + 2);

        *ptr++ = base64digits[(value >> 18) & 0x3f];
        *ptr++ = base64digits[(value >> 12) & 0x3f];
        *ptr++ = base64digits[(value >> 6) & 0x3f];
        *ptr++ = base64digits[value & 0x3f];
    }

    *ptr = '\0';

    std::string results = reinterpret_cast<char*>(output);
    free(output);

    return(results);
}

#if defined __PLATFORM_WINDOWS

std::string utilities::uuid() {
    uuid_t uuid;
    std::string output;

    ::UuidCreate(&uuid);

    CHAR* szUuid = NULL;
    if (::UuidToStringA(&uuid, (RPC_CSTR*)&szUuid) == RPC_S_OK)
    {
        output = szUuid;
        ::RpcStringFreeA((RPC_CSTR*)&szUuid);
    }

    return(output);
}

void utilities::sleep(::time_t milliseconds) {
    Sleep(milliseconds);
}

#endif

#if defined __PLATFORM_POSIX

std::string utilities::uuid() {
    // TODO: pull from /proc/sys/kernel/random/uuid

    return "";
}

void utilities::sleep(::time_t milliseconds) {
    ::sleep(milliseconds/1000);
}

#endif

template<> std::string utilities::read<std::string>(std::istream& input, size_t bytes) {
    std::vector<char> buffer(bytes + 1, 0);
    input.read(&buffer[0], bytes);
    buffer[bytes] = '\0';
    return std::string(&buffer[0]);
}


// https://en.wikipedia.org/wiki/Perlin_noise

#include <math.h>

/* Function to linearly interpolate between a0 and a1
 * Weight w should be in the range [0.0, 1.0]
 */
float interpolate(float a0, float a1, float w) {
    /* // You may want clamping by inserting:
     * if (0.0 > w) return a0;
     * if (1.0 < w) return a1;
     */
    return (a1 - a0) * w + a0;
    /* // Use this cubic interpolation [[Smoothstep]] instead, for a smooth appearance:
     * return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
     *
     * // Use [[Smootherstep]] for an even smoother result with a second derivative equal to zero on boundaries:
     * return (a1 - a0) * ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w) + a0;
     */
}

typedef struct {
    float x, y;
} vector2;

/* Create pseudorandom direction vector
 */
vector2 randomGradient(int ix, int iy) {
    // No precomputed gradients mean this works for any number of grid coordinates
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2; // rotation width
    unsigned a = ix, b = iy;
    a *= 3284157443; b ^= a << s | a >> w-s;
    b *= 1911520717; a ^= b << s | b >> w-s;
    a *= 2048419325;
    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]
    vector2 v;
    v.x = cos(random); v.y = sin(random);
    return v;
}

// Computes the dot product of the distance and gradient vectors.
float dotGridGradient(int ix, int iy, float x, float y) {
    // Get gradient from integer coordinates
    vector2 gradient = randomGradient(ix, iy);

    // Compute the distance vector
    float dx = x - (float)ix;
    float dy = y - (float)iy;

    // Compute the dot-product
    return (dx*gradient.x + dy*gradient.y);
}

// Compute Perlin noise at coordinates x, y
float utilities::perlin(float x, float y) {
    // Determine grid cell coordinates
    int x0 = (int)floor(x);
    int x1 = x0 + 1;
    int y0 = (int)floor(y);
    int y1 = y0 + 1;

    // Determine interpolation weights
    // Could also use higher order polynomial/s-curve here
    float sx = x - (float)x0;
    float sy = y - (float)y0;

    // Interpolate between grid point gradients
    float n0, n1, ix0, ix1, value;

    n0 = dotGridGradient(x0, y0, x, y);
    n1 = dotGridGradient(x1, y0, x, y);
    ix0 = interpolate(n0, n1, sx);

    n0 = dotGridGradient(x0, y1, x, y);
    n1 = dotGridGradient(x1, y1, x, y);
    ix1 = interpolate(n0, n1, sx);

    return ix1;
    //value = interpolate(ix0, ix1, sy);
    //return value; // Will return in range -1 to 1. To make it in range 0 to 1, multiply by 0.5 and add 0.5
}
