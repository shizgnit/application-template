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

#define __PLATFORM_SUPPORTS_METAL 1

#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>

#include <simd/simd.h>

namespace implementation {

    namespace metal {

        #define BUFFER_OFFSET(i) ((void*)(i))

        static inline float deg_to_radf(float deg) {
            return deg * (float)M_PI / 180.0f;
        }

        class graphics : public platform::graphics {
        public:
            void projection(int fov);
            void dimensions(int width, int height, float scale=1.0);

            void init(void);
            void clear(void);
            void flush(void);

            bool compile(type::shader& shader);
            bool compile(type::program& program);
            bool compile(type::material& material);
            bool compile(type::object& object);
            bool compile(type::font& font);
            bool compile(type::entity& entity);

            bool compile(platform::assets* assets);

            void draw(type::object& object, type::program& shader, const spatial::matrix& projection, const spatial::matrix& view=spatial::matrix(), const spatial::matrix& model=spatial::matrix(), const spatial::matrix& lighting=spatial::matrix(), unsigned int options=0x00);
            void draw(std::string text, type::font& font, type::program& shader, const spatial::matrix& projection, const spatial::matrix& view=spatial::matrix(), const spatial::matrix& model=spatial::matrix(), const spatial::matrix& lighting=spatial::matrix(), unsigned int options = 0x00);

            void ontarget(type::object& object);
            void untarget();

            void oninvert();
            void uninvert();

            void release(type::object* object) {
            }

            std::vector<type::object*> target;

            int offset_vector = 0;
            int offset_matrix = 0;
            
            MTL::Device* _pDevice = NULL;
            MTL::CommandQueue* _pCommandQueue = NULL;
            
            MTL::DepthStencilState* _pDepthStencilState;
            
            MTK::View* _pView = NULL;
            MTL::CommandBuffer* _pCmd = NULL;
            
            struct vertexData
            {
                simd::float4 position;
                simd::float4 texcoord;
                simd::float4 normal;
            };
            
            struct cameraData {
                simd::float4x4 projection;
                simd::float4x4 view;
                simd::float4x4 model;
                simd::float4x4 light;
            };
            
            MTL::Buffer* _pCameraDataBuffer = NULL;
            
            int kNumInstances;
        };

    }

}
