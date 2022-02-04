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

        int u_Clipping;

        int u_ProjectionMatrix;
        int u_ViewMatrix;
        int u_ModelMatrix;
        int u_LightingMatrix;

        int u_SurfaceTextureUnit;
        int u_ShadowTextureUnit;

        int u_AmbientLightPosition;
        int u_AmbientLightColor;

        int u_RelativeParticles;

        std::string type() {
            return "type::program";
        }

        bool empty() {
            return fragment.empty() || vertex.empty();
        }
    };

}