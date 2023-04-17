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

#include <string>

namespace utilities {

    template <class T> class type_cast {
    public:
        type_cast(const short& in) { m_internal = static_cast<T>(in); }
        type_cast(const int& in) { m_internal = static_cast<T>(in); }
        type_cast(const long& in) { m_internal = static_cast<T>(in); }
        type_cast(const unsigned short& in) { m_internal = static_cast<T>(in); }
        type_cast(const unsigned int& in) { m_internal = static_cast<T>(in); }
        type_cast(const unsigned long& in) { m_internal = static_cast<T>(in); }
        type_cast(const float& in) { m_internal = static_cast<T>(in); }
        type_cast(const double& in) { m_internal = static_cast<T>(in); }
        type_cast(const unsigned char& in) { m_internal = static_cast<T>(in); }
        type_cast(const char& in) { m_internal = static_cast<T>(in); }

        operator T& () { return(m_internal); }

    private:
        T m_internal;
    };

    template <> class type_cast <int> {
    public:
        type_cast(const char* in) { m_internal = atoi(in); }
        type_cast(std::string& in) { m_internal = atoi(in.c_str()); }

        operator int& () { return(m_internal); }

    private:
        int m_internal;
    };

    template <> class type_cast <unsigned long> {
    public:
        type_cast(const char* in) { m_internal = atol(in); }
        type_cast(std::string& in) { m_internal = atol(in.c_str()); }

        operator unsigned long& () { return(m_internal); }

    private:
        unsigned long m_internal;
    };

    template <> class type_cast <double> {
    public:
        type_cast(const char* in) { m_internal = atof(in); }
        type_cast(std::string& in) { m_internal = atof(in.c_str()); }

        operator double& () { return(m_internal); }

    private:
        double m_internal;
    };

    template <> class type_cast <std::string> {
    public:

#if defined __PLATFORM_WINDOWS
        type_cast(const short& in) { sprintf_s(m_buffer, "%d", in); m_data = m_buffer; }
        type_cast(const int& in) { sprintf_s(m_buffer, "%d", in); m_data = m_buffer; }
        type_cast(const long& in) { sprintf_s(m_buffer, "%ld", in); m_data = m_buffer; }
        type_cast(const unsigned short& in) { sprintf_s(m_buffer, "%u", in); m_data = m_buffer; }
        type_cast(const unsigned int& in) { sprintf_s(m_buffer, "%u", in); m_data = m_buffer; }
        type_cast(const unsigned long& in) { sprintf_s(m_buffer, "%lu", in); m_data = m_buffer; }
        type_cast(const float& in) { sprintf_s(m_buffer, "%f", in); m_data = m_buffer; }
        type_cast(const double& in) { sprintf_s(m_buffer, "%f", in); m_data = m_buffer; }
        type_cast(const unsigned char& in) { sprintf_s(m_buffer, "%c", in); m_data = m_buffer; }
        type_cast(const char& in) { sprintf_s(m_buffer, "%c", in); m_data = m_buffer; }
        type_cast(const time_t& in) { sprintf_s(m_buffer, "%llu", in); m_data = m_buffer; }
#else
        type_cast(const short& in) { sprintf(m_buffer, "%d", in); m_data = m_buffer; }
        type_cast(const int& in) { sprintf(m_buffer, "%d", in); m_data = m_buffer; }
        type_cast(const long& in) { sprintf(m_buffer, "%ld", in); m_data = m_buffer; }
        type_cast(const unsigned short& in) { sprintf(m_buffer, "%u", in); m_data = m_buffer; }
        type_cast(const unsigned int& in) { sprintf(m_buffer, "%u", in); m_data = m_buffer; }
        type_cast(const unsigned long& in) { sprintf(m_buffer, "%lu", in); m_data = m_buffer; }
        type_cast(const float& in) { sprintf(m_buffer, "%f", in); m_data = m_buffer; }
        type_cast(const double& in) { sprintf(m_buffer, "%f", in); m_data = m_buffer; }
        type_cast(const unsigned char& in) { sprintf(m_buffer, "%c", in); m_data = m_buffer; }
        type_cast(const char& in) { sprintf(m_buffer, "%c", in); m_data = m_buffer; }
#endif

        type_cast(const char* in) {
            m_data = in;
        }

        type_cast(const wchar_t* in) {
            auto tmp = std::wstring(in);
            m_data = std::string(tmp.begin(), tmp.end());
        }

        type_cast(std::wstring& in) {
            m_data = std::string(in.begin(), in.end());
        }

        operator std::string& () { return(m_data); }

        const char* c_str() { return(m_data.c_str()); }

    private:
        char m_buffer[50];
        std::string m_data;
    };

    template <> class type_cast <std::wstring> {
    public:

#if defined __PLATFORM_WINDOWS
        type_cast(DWORD& in) {
            //wsprintf(m_buffer, L"%u", in); 
            m_data = m_buffer;
        }
#endif

        type_cast(std::string& in) {
            m_data = std::wstring(in.begin(), in.end());
        }

        operator std::wstring& () { return(m_data); }

        const wchar_t* c_str() { return(m_data.c_str()); }

    private:
        wchar_t m_buffer[50];
        std::wstring m_data;
    };

}