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

    class program : virtual public type::info {
    public:
        type::shader fragment;
        type::shader vertex;
        
        type::shader unified;

        int a_Vertex;
        int a_Texture;
        int a_Normal;

        int a_ModelMatrix;

        int a_Identifier;
        int a_Flags;

        int u_Clipping;

        int u_ProjectionMatrix;
        int u_ViewMatrix;
        int u_ModelMatrix;
        int u_LightingMatrix;

        int u_SurfaceTextureUnit;
        int u_ShadowTextureUnit;
        int u_NormalTextureUnit;
        int u_DepthTextureUnit;
        int u_BlurTextureUnit;
        int u_PickingTextureUnit;

        int u_TextureSize;
        
        int u_AmbientLightPosition;
        int u_AmbientLightColor;
        int u_AmbientLightBias;
        int u_AmbientLightStrength;

        int u_RelativeParticles;

        int u_Flags;
        int u_Parameters;

        std::string type() {
            return "type::program";
        }

        bool empty() {
            return fragment.empty() || vertex.empty();
        }
    };

}
