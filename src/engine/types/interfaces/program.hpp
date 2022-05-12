#pragma once

namespace type {

    class program : virtual public type::info {
    public:
        type::shader fragment;
        type::shader vertex;

        unsigned int context;

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