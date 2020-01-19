#pragma once

namespace type {

    class program {
    public:
        type::shader fragment;
        type::shader vertex;

        unsigned int context;

        int a_Vertex;
        int a_Texture;
        int a_Normal;

        int u_ModelMatrix;
        int u_ViewMatrix;
        int u_ProjectionMatrix;

        int u_SurfaceTextureUnit;

        int u_AmbientLight;
        int u_DirectionalLight;
        int u_DirectionalLightPosition;
    };

}