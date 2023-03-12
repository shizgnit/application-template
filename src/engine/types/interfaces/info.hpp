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

namespace type {

    enum format {
        FORMAT_FNT   = 0x01,
        FORMAT_FRAG  = 0x02,
        FORMAT_MTL   = 0x03,
        FORMAT_OBJ   = 0x04,
        FORMAT_PNG   = 0x05,
        FORMAT_TGA   = 0x06,
        FORMAT_VERT  = 0x07,
        FORMAT_WAV   = 0x08,
        FORMAT_FBX   = 0x09,
        FORMAT_METAL = 0X0A
    };

    class info {
    protected:
        virtual void define() {
            // Just leave the info blank for now
        };
        info() {
            define();
        }

        std::vector<std::string> _extensions;
        format _format;

        bool _compiled = false;

        std::string _id;

        info& operator=(const info& ref) {
            _extensions = ref._extensions;
            _format = ref._format;
            _compiled = ref._compiled;
            _id = ref._id;
            
            resource = ref.resource;
            
            return *this;
        }
        
    public:
        class opaque_t;
        opaque_t *resource = nullptr;

        info(std::vector<std::string> extensions, format spec) {
            _extensions = extensions;
            _format = spec;
        }
        virtual ~info() {}

        virtual std::string id(std::string id = "") {
            if (id.empty() == false) {
                _id = id;
            }
            return _id;
        }

        virtual std::string type() {
            return "type::undefined";
        }

        virtual type::format format() {
            return _format;
        }

        bool compiled() {
            return _compiled;
        }

        virtual bool compiled(bool flag) {
            return _compiled = flag;
        }

        virtual bool empty() {
            return true;
        };
    };

}
