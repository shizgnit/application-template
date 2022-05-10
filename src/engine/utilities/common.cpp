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
    while (back != -1 && (in[back] == 0x09 || in[back] == 0x20)) {
        back -= 1;
    }
    if (back < in.length()) {
        return(in.substr(0, back + 1));
    }
    return(in);
}

std::string utilities::ltrim(std::string in) {
    size_t front = 0;
    while (front < in.length() && (in[front] == 0x09 || in[front] == 0x20)) {
        front += 1;
    }
    if (front > 0) {
        return(in.substr(front, in.length() - front));
    }
    return(in);
}

std::string utilities::trim(std::string in) {
    size_t front = 0;
    while (front < in.length() && (in[front] == 0x09 || in[front] == 0x20)) {
        front += 1;
    }
    size_t back = in.length() - 1;
    while (back != -1 && (in[back] == 0x09 || in[back] == 0x20)) {
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
        start = end + 1;
    }
    results.push_back(input.substr(start, input.length() - start));

    return(results);
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
