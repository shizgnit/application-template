#include "engine.hpp"

#if defined __PLATFORM_SUPPORTS_OPENGL

bool implementation::opengl::fbo::init(type::object& object, bool depth) {
    allocation = object.texture.depth ? GL_DEPTH_ATTACHMENT : attachments().allocate();
    if (allocation == 0) {
        return false;
    }

    glGenFramebuffers(1, &context.frame);
    glBindFramebuffer(GL_FRAMEBUFFER, context.frame);

    glFramebufferTexture2D(GL_FRAMEBUFFER, allocation, GL_TEXTURE_2D, object.texture.context, 0);

    glGenBuffers(1, &context.render);
    glBindRenderbuffer(GL_RENDERBUFFER, context.render);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, object.texture.map->properties.width, object.texture.map->properties.height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, context.render);

    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        throw("failed to initialize frame buffer");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    target = &object;

    return true;
}

void implementation::opengl::fbo::enable(bool clear) {
    if (target == NULL) {
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, context.frame);
    glViewport(0, 0, target->texture.map->properties.width, target->texture.map->properties.height);
    if (target->texture.depth) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
    }
    else {
        glClear(clear ? GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT : GL_DEPTH_BUFFER_BIT);
    }
}

void implementation::opengl::fbo::disable() {
    if (target == NULL) {
        return;
    }
    // Currently all rendering is done to the mipmap level 0... copy it out after every render
    if (target->texture.depth) {
        glCullFace(GL_BACK);
    }
    else {
        glBindTexture(GL_TEXTURE_2D, target->texture.context);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void implementation::opengl::graphics::geometry(int width, int height) {
    glViewport(0, 0, width, height);
    display_width = width;
    display_height = height;
}

void implementation::opengl::graphics::init(void) {
    //glEnable(GL_SHADE_MODEL, GL_SMOOTH);
    glShadeModel(GL_SMOOTH);

    // Depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);
    glClearDepthf(1.0f);
    glDepthRangef(0.0f, 1.0f);

    // Alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Backface culling, makes GLES rendering of objects easier since they don't need to be drawn back to front
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    auto glsl = glGetString(GL_SHADING_LANGUAGE_VERSION);

    auto str = utilities::tokenize((const char*)glGetString(GL_EXTENSIONS), " ");

    // Setup the ray used for drawing normals
    ray = spatial::ray(spatial::vector(0.0, 0.0, 0.0), spatial::vector(2.0, 0.0, 0.0));
    ray.texture.map = &assets->get<type::image>("ray");
    ray.texture.map->create(1, 1, 255, 255, 255, 255);
    ray.xy_projection(0, 0, 1, 1);
    compile(ray);

    // Setup the shadow depth map
    shadow = spatial::quad(256, 256);
    shadow.texture.map = &assets->get<type::image>("shadowmap");
    shadow.texture.map->create(1024, 1024, 0, 0, 0, 0);
    shadow.texture.depth = true;
    shadow.xy_projection(0, 0, shadow.texture.map->properties.width, shadow.texture.map->properties.height);
    compile(shadow);

    // Calculate the offsets
    spatial::vector vector({ 256.0f });
    unsigned char* vector_ptr = (unsigned char*)&vector;
    for (int i = 0; i < sizeof(spatial::vector); i++) {
        if (*(spatial::vector::type_t*)(vector_ptr + i) == 256.0f) {
            offset_vector = i;
            break;
        }
    }
    spatial::matrix matrix({ {256.0f} });
    unsigned char* matrix_ptr = (unsigned char*)&matrix;
    for (int i = 0; i < sizeof(spatial::matrix); i++) {
        if (*(spatial::matrix::type_t*)(matrix_ptr + i) == 256.0f) {
            offset_matrix = i;
            break;
        }
    }
}

void implementation::opengl::graphics::clear(void) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void implementation::opengl::graphics::flush(void) {
    glFlush();
    frames.push_back(frame);
    time_t now = time(NULL);
    if (timestamp != now) {
        stats total;
        for (auto frame : frames) {
            total.lines += frame.lines;
            total.triangles += frame.triangles;
            total.vertices += frame.vertices;
        }
        total.frames = frames.size();
        activity.push_back(total);
        if (activity.size() > 60) { // TODO: make this configurable
            activity.pop_front();
        }
        frames.clear();
    }
    frame.clear();
}

bool implementation::opengl::graphics::compile(type::shader& shader) {
    if (shader.compile() == false) {
        return false;
    }

    switch (shader.format()) {
    case(type::format::FORMAT_VERT):
        shader.context = glCreateShader(GL_VERTEX_SHADER);
        break;
    case(type::format::FORMAT_FRAG):
        shader.context = glCreateShader(GL_FRAGMENT_SHADER);
        break;
    }
    if (!shader.context) {
        return false;
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
            errors.push_back(info);
            free(info);
            glDeleteShader(shader.context);
            shader.context = 0;
            return false;
        }
    }

    return true;
}

bool implementation::opengl::graphics::compile(type::program& program) {
    if (program.compile() == false) {
        return false;
    }

    if (compile(program.vertex) == false) {
        return false;
    }
    if (compile(program.fragment) == false) {
        return false;
    }

    program.context = glCreateProgram();
    if (!program.context) {
        return false;
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
            errors.push_back(info);
            free(info);
        }
        glDeleteProgram(program.context);
        program.context = 0;
    }

    glUseProgram(program.context);

    program.a_Vertex = glGetAttribLocation(program.context, "a_Vertex");
    program.a_Texture = glGetAttribLocation(program.context, "a_Texture");
    program.a_Normal = glGetAttribLocation(program.context, "a_Normal");

    program.a_ModelMatrix = glGetAttribLocation(program.context, "a_ModelMatrix");

    program.u_ProjectionMatrix = glGetUniformLocation(program.context, "u_ProjectionMatrix");
    program.u_ViewMatrix = glGetUniformLocation(program.context, "u_ViewMatrix");
    program.u_ModelMatrix = glGetUniformLocation(program.context, "u_ModelMatrix");
    program.u_LightingMatrix = glGetUniformLocation(program.context, "u_LightingMatrix");

    program.u_SurfaceTextureUnit = glGetUniformLocation(program.context, "u_SurfaceTextureUnit");
    program.u_ShadowTextureUnit = glGetUniformLocation(program.context, "u_ShadowTextureUnit");

    program.u_Clipping = glGetUniformLocation(program.context, "u_Clipping");

    program.u_AmbientLightPosition = glGetUniformLocation(program.context, "u_AmbientLightPosition");
    program.u_AmbientLightColor = glGetUniformLocation(program.context, "u_AmbientLightColor");

    return true;
}

bool implementation::opengl::graphics::compile(type::material& material) {
    if (material.compile() == false) {
        return false;
    }
    if (material.map == NULL) {
        return false;
    }

    if (material.depth) {
        glGenTextures(1, &material.context);
        glBindTexture(GL_TEXTURE_2D, material.context);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, material.map->properties.width, material.map->properties.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    else {
        glGenTextures(1, &material.context);
        glBindTexture(GL_TEXTURE_2D, material.context);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, material.map->properties.width, material.map->properties.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, material.map->raster.data());
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

bool implementation::opengl::graphics::compile(type::object& object) {
    for (auto &child : object.children) {
        compile(child);
    }
    if (object.compile() == false || object.vertices.size() == 0) {
        return false;
    }

    glGenBuffers(1, &object.context);
    glBindBuffer(GL_ARRAY_BUFFER, object.context);
    glBufferData(GL_ARRAY_BUFFER, sizeof(spatial::vertex) * object.vertices.size(), object.vertices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    compile(object.texture);

    return true;
}

bool implementation::opengl::graphics::compile(type::font& font) {
    if (font.compile() == false) {
        return false;
    }

    for (auto& glyph : font.glyphs) {
        if (glyph.identifier) {
            compile(glyph.quad);
        }
    }

    return true;
}

bool implementation::opengl::graphics::compile(type::entity& entity) {
    if (entity.context == 0 && entity.positions.size()) {
        glGenBuffers(1, &entity.context);
        glBindBuffer(GL_ARRAY_BUFFER, entity.context);
        glBufferData(GL_ARRAY_BUFFER, sizeof(spatial::matrix) * 1024, NULL, GL_STREAM_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(spatial::matrix) * entity.positions.size(), entity.positions.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (entity.compile() == false) {
        return false;
    }

    for (auto& animation : entity.animations) {
        for (auto& frame : animation.second.frames) {
            compile(frame);
        }
    }

    return true;
}

bool implementation::opengl::graphics::compile(platform::assets* assets) {
    for (auto program : assets->get<type::program>()) {
        compile(*program);
    }
    for (auto material : assets->get<type::material>()) {
        compile(*material);
    }
    for (auto object : assets->get<type::object>()) {
        compile(*object);
    }
    for (auto entity : assets->get<type::entity>()) {
        compile(*entity);
    }
    for (auto font : assets->get<type::font>()) {
        compile(*font);
    }
    return true;
}

bool implementation::opengl::graphics::recompile(type::object& object) {
    if (object.context == 0) {
        return false;
    }

    glBindBuffer(GL_ARRAY_BUFFER, object.context);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(spatial::vertex) * object.vertices.size(), object.vertices.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return true;
}

bool implementation::opengl::graphics::recompile(type::entity& entity) {
    if (entity.context == 0) {
        return false;
    }

    glBindBuffer(GL_ARRAY_BUFFER, entity.context);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(spatial::matrix) * entity.positions.size(), entity.positions.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return true;
}

void implementation::opengl::graphics::draw(type::object& object, type::program& shader, const spatial::matrix& projection, const spatial::matrix& view, const spatial::matrix& model, const spatial::matrix& lighting, unsigned int options) {
    // Look for the first object with vertices, just at the top level for now
    auto &target = object;
    if (target.vertices.size() == 0) {
        return;
    }

    //std::string mstr = model;
    //std::string lstr = lighting;

    glUseProgram(shader.context);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, target.texture.context);
    if (shadow.texture.context) {
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, shadow.texture.context);
    }

    glUniformMatrix4fv(shader.u_ProjectionMatrix, 1, GL_FALSE, (GLfloat*)projection.data());
    glUniformMatrix4fv(shader.u_ViewMatrix, 1, GL_FALSE, (GLfloat*)view.data());
    glUniformMatrix4fv(shader.u_ModelMatrix, 1, GL_FALSE, (GLfloat*)model.data());
    glUniformMatrix4fv(shader.u_LightingMatrix, 1, GL_FALSE, (GLfloat*)lighting.data());

    glUniform1i(shader.u_SurfaceTextureUnit, 0);
    glUniform1i(shader.u_ShadowTextureUnit, 1);

    glUniform4f(shader.u_AmbientLightPosition, ambient.position.center.x, ambient.position.center.y, ambient.position.center.z, ambient.position.center.w);
    glUniform4f(shader.u_AmbientLightColor, ambient.color.x, ambient.color.y, ambient.color.z, ambient.color.w);

    glUniform4f(shader.u_Clipping, clip_top[clip_top.size()-1], clip_bottom[clip_bottom.size() - 1], clip_left[clip_left.size() - 1], clip_right[clip_right.size() - 1]);

    glBindBuffer(GL_ARRAY_BUFFER, target.context);
    glVertexAttribPointer(shader.a_Vertex, 4, GL_FLOAT, GL_FALSE, sizeof(spatial::vertex), BUFFER_OFFSET(offset_vector));
    glVertexAttribPointer(shader.a_Texture, 4, GL_FLOAT, GL_FALSE, sizeof(spatial::vertex), BUFFER_OFFSET(offset_vector +sizeof(spatial::vector)));
    glVertexAttribPointer(shader.a_Normal, 4, GL_FLOAT, GL_TRUE, sizeof(spatial::vertex), BUFFER_OFFSET(offset_vector +(sizeof(spatial::vector) * 2)));

    glEnableVertexAttribArray(shader.a_Vertex);
    glEnableVertexAttribArray(shader.a_Texture);
    glEnableVertexAttribArray(shader.a_Normal);

    int instances = 1;
    if (object.emitter && object.emitter->context) {
        glBindBuffer(GL_ARRAY_BUFFER, object.emitter->context);
        glVertexAttribPointer(shader.a_ModelMatrix, 16, GL_FLOAT, GL_FALSE, sizeof(spatial::matrix), BUFFER_OFFSET(offset_matrix));
        glVertexAttribDivisor(shader.a_ModelMatrix, 1);
        instances = object.emitter->positions.size();
    }

    // Draw either the solids or wireframes
    if (target.vertices.size() == 2 || options & render::WIREFRAME) {
        //glDrawArrays(GL_LINES, 0, target.vertices.size());
        glDrawArraysInstanced(GL_LINES, 0, target.vertices.size(), instances);
        frame.lines += target.vertices.size() / 2;
    }
    else {
        //glDrawArrays(GL_TRIANGLES, 0, target.vertices.size());
        glDrawArraysInstanced(GL_TRIANGLES, 0, target.vertices.size(), instances);
        frame.triangles += target.vertices.size() / 3;
    }
    frame.vertices += target.vertices.size();

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Primary object has been drawn, draw out the normals if requested.  Mostly for debugging.
    if (options & render::NORMALS) {
        for (auto vertex : target.vertices) {
            ray = spatial::ray(vertex.coordinate, vertex.normal + vertex.coordinate);
            recompile(ray);
            draw(ray, shader, projection, view, model);
        }
    }
}

void implementation::opengl::graphics::draw(std::string text, type::font& font, type::program& shader, const spatial::matrix& projection, const spatial::matrix& view, const spatial::matrix& model, const spatial::matrix& lighting, unsigned int options) {
    int prior = 0;
    spatial::matrix position = model;
    for (unsigned int i = 0; i < text.length(); i++) {
        position.translate(spatial::vector((float)font.kern(prior, text[i]), 0.0f, 0.0f));
        spatial::matrix relative = position;

        auto& glyph = font.glyphs[text[i]];

        relative.translate(spatial::vector((float)glyph.xoffset, (float)(font.point() - glyph.height - glyph.yoffset), 0.0f));

        draw(glyph.quad, shader, projection, view, relative);
        position.translate(spatial::vector((float)glyph.xadvance, 0.0f, 0.0f));
        prior = text[i];
    }
}

#endif