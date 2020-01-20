#include "engine.hpp"

#if defined __PLATFORM_SUPPORTS_OPENGL

void implementation::opengl::graphics::init(void) {
    //glDisable(GL_DEPTH_TEST);

    glDepthFunc(GL_LEQUAL);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void implementation::opengl::graphics::clear(void) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void implementation::opengl::graphics::compile(type::shader& shader) {
    switch (shader.type()) {
    case(type::format::FORMAT_VERT):
        shader.context = glCreateShader(GL_VERTEX_SHADER);
        break;
    case(type::format::FORMAT_FRAG):
        shader.context = glCreateShader(GL_FRAGMENT_SHADER);
        break;
    }
    if (!shader.context) {
        return;
    }

    GLchar* text = (GLchar*)shader.text.c_str();
    GLint length = strlen(text);
    glShaderSource(shader.context, 1, (const GLchar**)&text, &length);

    glCompileShader(shader.context);

    GLint compiled = GL_FALSE;
    glGetShaderiv(shader.context, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE) {
        glGetShaderiv(shader.context, GL_INFO_LOG_LENGTH, &length);
        if (length) {
            char* info = (char*)malloc(length);
            glGetShaderInfoLog(shader.context, length, NULL, info);

            free(info);
            glDeleteShader(shader.context);
            shader.context = 0;
        }
    }
}

void implementation::opengl::graphics::compile(type::program& program) {
    compile(program.vertex);
    compile(program.fragment);

    program.context = glCreateProgram();
    if (!program.context) {
        return;
    }

    glAttachShader(program.context, program.vertex.context);

    glAttachShader(program.context, program.fragment.context);

    glLinkProgram(program.context);

    GLint linked = GL_FALSE;
    glGetProgramiv(program.context, GL_LINK_STATUS, &linked);
    if (linked != GL_TRUE) {
        GLint length = 0;
        glGetProgramiv(program.context, GL_INFO_LOG_LENGTH, &length);
        if (length) {
            char* info = (char*)malloc(length);
            glGetProgramInfoLog(program.context, length, NULL, info);
            free(info);
        }
        glDeleteProgram(program.context);
        program.context = 0;
    }

    program.a_Vertex = glGetAttribLocation(program.context, "a_Vertex");
    program.a_Texture = glGetAttribLocation(program.context, "a_Texture");
    program.a_Normal = glGetAttribLocation(program.context, "a_Normal");

    program.u_ModelMatrix = glGetUniformLocation(program.context, "u_ModelMatrix");
    program.u_ViewMatrix = glGetUniformLocation(program.context, "u_ViewMatrix");
    program.u_ProjectionMatrix = glGetUniformLocation(program.context, "u_ProjectionMatrix");

    program.u_SurfaceTextureUnit = glGetUniformLocation(program.context, "u_SurfaceTextureUnit");

    program.u_AmbientLight = glGetUniformLocation(program.context, "u_AmbientLight");
    program.u_DirectionalLight = glGetUniformLocation(program.context, "u_DirectionalLight");
    program.u_DirectionalLightPosition = glGetUniformLocation(program.context, "u_DirectionalLightPosition");
}

void implementation::opengl::graphics::compile(type::object& object) {
    glGenBuffers(1, &object.context);
    glBindBuffer(GL_ARRAY_BUFFER, object.context);
    glBufferData(GL_ARRAY_BUFFER, sizeof(type::object::vertex) * object.vertices.size(), object.vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenTextures(1, &object.texture.context);
    glBindTexture(GL_TEXTURE_2D, object.texture.context);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, object.texture.map.properties.width, object.texture.map.properties.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, object.texture.map.raster.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void implementation::opengl::graphics::compile(type::font& font) {
    for (auto glyph : font.glyphs) {
        if (glyph.identifier) {
            compile(glyph.quad);
        }
    }
}


void implementation::opengl::graphics::draw(type::object& object, type::program& shader, const spatial::matrix& model, const spatial::matrix& view, const spatial::matrix& projection) {
    glUseProgram(shader.context);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, object.texture.context);

    glUniformMatrix4fv(shader.u_ModelMatrix, 1, GL_FALSE, (GLfloat*)model.r);
    glUniformMatrix4fv(shader.u_ViewMatrix, 1, GL_FALSE, (GLfloat*)view.r);
    glUniformMatrix4fv(shader.u_ProjectionMatrix, 1, GL_FALSE, (GLfloat*)projection.r);
    glUniform1i(shader.u_SurfaceTextureUnit, 0);

    glUniform4f(shader.u_AmbientLight, 0.0f, 0.0f, 0.0f, 1.0f);

    glUniform4f(shader.u_DirectionalLight, 0.0f, 0.0f, 0.0f, 1.0f);
    glUniform4f(shader.u_DirectionalLightPosition, 1.0f, 1.0f, 0.1f, 1.0f);

    glBindBuffer(GL_ARRAY_BUFFER, object.context);
    glVertexAttribPointer(shader.a_Vertex, 4, GL_FLOAT, GL_FALSE, sizeof(type::object::vertex), BUFFER_OFFSET(0));
    glVertexAttribPointer(shader.a_Texture, 4, GL_FLOAT, GL_FALSE, sizeof(type::object::vertex), BUFFER_OFFSET(4 * sizeof(GL_FLOAT)));
    glVertexAttribPointer(shader.a_Normal, 4, GL_FLOAT, GL_TRUE, sizeof(type::object::vertex), BUFFER_OFFSET(8 * sizeof(GL_FLOAT)));

    glEnableVertexAttribArray(shader.a_Vertex);
    glEnableVertexAttribArray(shader.a_Texture);
    glEnableVertexAttribArray(shader.a_Normal);

    //glDrawArrays(GL_TRIANGLE_FAN, 0, object.vertices.size());
    glDrawArrays(GL_TRIANGLES, 0, object.vertices.size());

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void implementation::opengl::graphics::draw(std::string text, type::font& font, type::program& shader, const spatial::matrix& model, const spatial::matrix& view, const spatial::matrix& projection) {
    int prior = 0;
    spatial::matrix position = model;
    for (unsigned int i = 0; i < text.length(); i++) {
        position.translate(spatial::vector((float)font.kern(prior, text[i]), 0.0f, 1.0f));
        spatial::matrix relative = position;
        relative.translate(spatial::vector((float)font.glyphs[text[i]].xoffset, 0.0f, 1.0f));
        draw(font.glyphs[text[i]].quad, shader, relative, view, projection);
        position.translate(spatial::vector((float)font.glyphs[text[i]].xadvance, 0.0f, 1.0f));
        prior = text[i];
    }
}

#endif