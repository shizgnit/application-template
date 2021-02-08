#include "engine.hpp"

#if defined __PLATFORM_SUPPORTS_OPENGL

bool implementation::opengl::fbo::init(bool depth) {
    static attachment attachments;
    allocation = attachments.allocate();

    glGenFramebuffers(1, &context.frame);
    glBindFramebuffer(GL_FRAMEBUFFER, context.frame);

    if (0) {
        glGenTextures(1, &texture.context);
        glBindTexture(GL_TEXTURE_2D, texture.context);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.map.properties.width, texture.map.properties.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.context, 0);
    }

    if (depth) {
        glGenRenderbuffers(1, &context.depth);
        glBindRenderbuffer(GL_RENDERBUFFER, context.depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, texture.map.properties.width, texture.map.properties.height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, context.depth);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glGenBuffers(1, &context.render);

    glBindRenderbuffer(GL_RENDERBUFFER, context.render);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, texture.map.properties.width, texture.map.properties.height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, context.render);

    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        throw("failed to initialize frame buffer");
    }
    return true;
}

void implementation::opengl::fbo::enable() {
    glBindFramebuffer(GL_FRAMEBUFFER, context.frame);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.context, 0);
    //glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, texture.map.properties.width, texture.map.properties.height);
    //glDrawBuffer(allocation);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void implementation::opengl::fbo::disable() {
    //glPopAttrib();
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glViewport(0, 0, prior, prior);
}

void implementation::opengl::graphics::geometry(int width, int height) {
    glViewport(0, 0, width, height);
    display_width = width;
    display_height = height;
}

void implementation::opengl::graphics::init(void) {
    //glDisable(GL_DEPTH_TEST);

    glDepthFunc(GL_LEQUAL);

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE); // off for now, but should be toggled based on render options
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto str = utilities::tokenize((const char*)glGetString(GL_EXTENSIONS), " ");
}

void implementation::opengl::graphics::clear(void) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void implementation::opengl::graphics::flush(void) {
    glFlush();
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

    program.u_Clipping = glGetUniformLocation(program.context, "u_Clipping");

    program.u_AmbientLight = glGetUniformLocation(program.context, "u_AmbientLight");
    program.u_DirectionalLight = glGetUniformLocation(program.context, "u_DirectionalLight");
    program.u_DirectionalLightPosition = glGetUniformLocation(program.context, "u_DirectionalLightPosition");
}

void implementation::opengl::graphics::compile(type::object& object) {
    glGenBuffers(1, &object.context);
    glBindBuffer(GL_ARRAY_BUFFER, object.context);
    glBufferData(GL_ARRAY_BUFFER, sizeof(spatial::vertex) * object.vertices.size(), object.vertices.data(), GL_STATIC_DRAW);
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
    for (auto &glyph : font.glyphs) {
        if (glyph.identifier) {
            compile(glyph.quad);
        }
    }
}


void implementation::opengl::graphics::draw(type::object& object, type::program& shader, const spatial::matrix& model, const spatial::matrix& view, const spatial::matrix& projection) {
    glUseProgram(shader.context);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // GL_FILL

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, object.texture.context);

    glUniformMatrix4fv(shader.u_ModelMatrix, 1, GL_FALSE, (GLfloat*)model.data());
    glUniformMatrix4fv(shader.u_ViewMatrix, 1, GL_FALSE, (GLfloat*)view.data());
    glUniformMatrix4fv(shader.u_ProjectionMatrix, 1, GL_FALSE, (GLfloat*)projection.data());

    glUniform1i(shader.u_SurfaceTextureUnit, 0);

    glUniform4f(shader.u_AmbientLight, 0.0f, 0.0f, 0.0f, 1.0f);

    glUniform4f(shader.u_DirectionalLight, 0.0f, 0.0f, 0.0f, 1.0f);
    glUniform4f(shader.u_DirectionalLightPosition, 1.0f, 1.0f, 0.1f, 1.0f);

    glUniform4f(shader.u_Clipping, clip_top[clip_top.size()-1], clip_bottom[clip_bottom.size() - 1], clip_left[clip_left.size() - 1], clip_right[clip_right.size() - 1]);

#ifdef __PLATFORM_64BIT
    size_t offset = 8; // TODO: necessary to offset object overhead, this could/should be dynamic.  if this is enabled for android it will not work.    
#else
    size_t offset = 4;
#endif

    glBindBuffer(GL_ARRAY_BUFFER, object.context);
    glVertexAttribPointer(shader.a_Vertex, 4, GL_FLOAT, GL_FALSE, sizeof(spatial::vertex), BUFFER_OFFSET(offset));
    glVertexAttribPointer(shader.a_Texture, 4, GL_FLOAT, GL_FALSE, sizeof(spatial::vertex), BUFFER_OFFSET(offset+sizeof(spatial::vector)));
    glVertexAttribPointer(shader.a_Normal, 4, GL_FLOAT, GL_TRUE, sizeof(spatial::vertex), BUFFER_OFFSET(offset+(sizeof(spatial::vector) * 2)));

    glEnableVertexAttribArray(shader.a_Vertex);
    glEnableVertexAttribArray(shader.a_Texture);
    glEnableVertexAttribArray(shader.a_Normal);

    //glDrawArrays(GL_TRIANGLE_FAN, 0, object.vertices.size());
    if (object.type == spatial::geometry::primitive::LINE) {
        glDrawArrays(GL_LINES, 0, object.vertices.size());
    }
    else { // TODO: force all geometry to be explicit
    //if (object.type == spatial::geometry::primitive::POLYGON) {
        glDrawArrays(GL_TRIANGLES, 0, object.vertices.size());
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void implementation::opengl::graphics::draw(std::string text, type::font& font, type::program& shader, const spatial::matrix& model, const spatial::matrix& view, const spatial::matrix& projection) {
    int prior = 0;
    spatial::matrix position = model;
    for (unsigned int i = 0; i < text.length(); i++) {
        position.translate(spatial::vector((float)font.kern(prior, text[i]), 0.0f, 0.0f));
        spatial::matrix relative = position;

        auto& glyph = font.glyphs[text[i]];

        relative.translate(spatial::vector((float)glyph.xoffset, (float)(font.point() - glyph.height - glyph.yoffset), 0.0f));

        draw(glyph.quad, shader, relative, view, projection);
        position.translate(spatial::vector((float)glyph.xadvance, 0.0f, 0.0f));
        prior = text[i];
    }
}

#endif