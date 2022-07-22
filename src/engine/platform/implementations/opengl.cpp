#include "engine.hpp"

#if defined __PLATFORM_SUPPORTS_OPENGL

bool implementation::opengl::fbo::init(type::object& object, platform::graphics *ref, bool depth, unsigned char *collector) {
    if (allocation == 0) {
        allocation = object.texture.depth ? GL_DEPTH_ATTACHMENT : attachments().allocate();
        if (allocation == 0 || object.texture.color == NULL) {
            return false;
        }
    }

    collect = collector;

    parent = ref;

    if (context.frame) {
        glDeleteFramebuffers(1, &context.frame);
    }
    if (context.render) {
        glDeleteRenderbuffers(1, &context.render);
    }

    glGenFramebuffers(1, &context.frame);
    glBindFramebuffer(GL_FRAMEBUFFER, context.frame);
    glFramebufferTexture2D(GL_FRAMEBUFFER, allocation, GL_TEXTURE_2D, object.texture.color->context, 0);

    glGenRenderbuffers(1, &context.render);
    glBindRenderbuffer(GL_RENDERBUFFER, context.render);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, object.texture.color->properties.width, object.texture.color->properties.height);
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
    glBindRenderbuffer(GL_RENDERBUFFER, context.render);
    glViewport(0, 0, target->texture.color->properties.width, target->texture.color->properties.height);
    glClear(clear ? GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT : GL_DEPTH_BUFFER_BIT);
}

void implementation::opengl::fbo::disable() {
    if (target == NULL || target->texture.color == NULL) {
        return;
    }

    // Currently all rendering is done to the mipmap level 0... copy it out after every render
    if (collect != NULL) {
        glReadPixels(0, 0, target->texture.color->properties.width, target->texture.color->properties.height, GL_RGBA, GL_UNSIGNED_BYTE, collect);
    }
    if (!target->texture.depth) {
        glBindTexture(GL_TEXTURE_2D, target->texture.color->context);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, parent->width(), parent->height());
}

void implementation::opengl::graphics::projection(int fov) {
    ortho = spatial::matrix().ortho(0, display_width, 0, display_height);
    perspective = spatial::matrix().perspective(fov, (float)display_width / (float)display_height, 0.0f, 100.0f);
}

void implementation::opengl::graphics::dimensions(int width, int height, float scale) {
    bool init = (display_width == 0 && display_height == 0);

    display_width = width;
    display_height = height;

    glViewport(0, 0, display_width, display_height);

    // Setup the render buffer
    color = spatial::quad(display_width, display_height);
    color.texture.color = &assets->get<type::image>("color");
    color.texture.color->create(display_width, display_height, 0, 0, 0, 0);
    color.xy_projection(0, 0, display_width, display_height, false, true);
    compile(color);
    fbos[color.instance].init(color, this);

    // Setup the scene depth buffer
    depth = spatial::quad(display_width, display_height);
    depth.texture.color = &assets->get<type::image>("depth");
    depth.texture.color->create(display_width, display_height, 0, 0, 0, 0);
    //depth.texture.depth = true;
    depth.xy_projection(0, 0, display_width, display_height, false, true);
    compile(depth);
    fbos[depth.instance].init(depth, this);

    // Setup the post process blur buffer
    blur = spatial::quad(display_width * scale, display_height * scale);
    blur.texture.color = &assets->get<type::image>("blur");
    blur.texture.color->create(display_width * scale, display_height * scale, 0, 0, 0, 0);
    blur.xy_projection(0, 0, display_width * scale, display_height * scale, false, true);
    compile(blur);
    fbos[blur.instance].init(blur, this);

    // Setup the picking buffer
    picking = spatial::quad(display_width, display_height);
    picking.texture.color = &assets->get<type::image>("picking");
    picking.texture.color->create(display_width, display_height, 0, 0, 0, 0);
    picking.xy_projection(0, 0, display_width, display_height, false, true);
    compile(picking);
    pixels.resize(display_width * display_height * 4, 0);
    fbos[picking.instance].init(picking, this, false, pixels.data());
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
    ray.texture.color = &assets->get<type::image>("ray");
    ray.texture.color->create(1, 1, 255, 255, 255, 255);
    ray.xy_projection(0, 0, 1, 1);
    compile(ray);

    // Setup the shadow depth map
    shadow = spatial::quad(256, 256);
    shadow.texture.color = &assets->get<type::image>("shadowmap");
    shadow.texture.color->create(2048, 2048, 0, 0, 0, 0);
    //shadow.texture.depth = true;
    shadow.xy_projection(0, 0, shadow.texture.color->properties.width, shadow.texture.color->properties.height);
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

    program.a_ModelMatrix = glGetAttribLocation(program.context, "a_ModelMatrix");

    program.a_Identifier = glGetAttribLocation(program.context, "a_Identifier");
    program.a_Flags = glGetAttribLocation(program.context, "a_Flags");

    program.a_Vertex = glGetAttribLocation(program.context, "a_Vertex");
    program.a_Texture = glGetAttribLocation(program.context, "a_Texture");
    program.a_Normal = glGetAttribLocation(program.context, "a_Normal");

    program.u_ProjectionMatrix = glGetUniformLocation(program.context, "u_ProjectionMatrix");
    program.u_ViewMatrix = glGetUniformLocation(program.context, "u_ViewMatrix");
    program.u_ModelMatrix = glGetUniformLocation(program.context, "u_ModelMatrix");
    program.u_LightingMatrix = glGetUniformLocation(program.context, "u_LightingMatrix");

    program.u_Clipping = glGetUniformLocation(program.context, "u_Clipping");

    program.u_AmbientLightPosition = glGetUniformLocation(program.context, "u_AmbientLightPosition");
    program.u_AmbientLightColor = glGetUniformLocation(program.context, "u_AmbientLightColor");
    program.u_AmbientLightBias = glGetUniformLocation(program.context, "u_AmbientLightBias");
    program.u_AmbientLightStrength = glGetUniformLocation(program.context, "u_AmbientLightStrength");

    program.u_Flags = glGetUniformLocation(program.context, "u_Flags");
    program.u_Parameters = glGetUniformLocation(program.context, "u_Parameters");

    program.u_SurfaceTextureUnit = glGetUniformLocation(program.context, "u_SurfaceTextureUnit");
    program.u_NormalTextureUnit = glGetUniformLocation(program.context, "u_NormalTextureUnit");
    program.u_ShadowTextureUnit = glGetUniformLocation(program.context, "u_ShadowTextureUnit");
    program.u_DepthTextureUnit = glGetUniformLocation(program.context, "u_DepthTextureUnit");
    program.u_BlurTextureUnit = glGetUniformLocation(program.context, "u_BlurTextureUnit");
    program.u_PickingTextureUnit = glGetUniformLocation(program.context, "u_PickingTextureUnit");

    return true;
}

bool implementation::opengl::graphics::compile(type::material& material) {
    if (material.compile() == false) {
        return false;
    }
    if (material.color == NULL && material.normal == NULL) {
        return false;
    }

    if (material.color) {
        if (material.color->context) {
            glDeleteTextures(1, &material.color->context);
        }
        if (material.depth) {
            glGenTextures(1, &material.color->context);
            glBindTexture(GL_TEXTURE_2D, material.color->context);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, material.color->properties.width, material.color->properties.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        else {
            glGenTextures(1, &material.color->context);
            glBindTexture(GL_TEXTURE_2D, material.color->context);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, material.color->properties.width, material.color->properties.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, material.color->raster.data());
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    }

    if (material.normal) {
        if (material.normal->context) {
            glDeleteTextures(1, &material.normal->context);
        }
        glGenTextures(1, &material.normal->context);
        glBindTexture(GL_TEXTURE_2D, material.normal->context);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, material.normal->properties.width, material.normal->properties.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, material.normal->raster.data());
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

    if (object.context) {
        glBindBuffer(GL_ARRAY_BUFFER, object.context);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(spatial::vertex) * object.vertices.size(), object.vertices.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    else {
        glGenBuffers(1, &object.context);
        glBindBuffer(GL_ARRAY_BUFFER, object.context);
        glBufferData(GL_ARRAY_BUFFER, sizeof(spatial::vertex) * object.vertices.size(), object.vertices.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

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
    entity.bake();

    if (entity.identifiers.context) {
        glBindBuffer(GL_ARRAY_BUFFER, entity.identifiers.context);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(unsigned int) * entity.identifiers.content.size(), entity.identifiers.content.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    else if (entity.identifiers.content.size()) {
        glGenBuffers(1, &entity.identifiers.context);
        glBindBuffer(GL_ARRAY_BUFFER, entity.identifiers.context);
        glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * 1024, NULL, GL_STREAM_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(unsigned int) * entity.identifiers.content.size(), entity.identifiers.content.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (entity.flags.context) {
        glBindBuffer(GL_ARRAY_BUFFER, entity.flags.context);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(unsigned int) * entity.flags.content.size(), entity.flags.content.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    else if (entity.flags.content.size()) {
        glGenBuffers(1, &entity.flags.context);
        glBindBuffer(GL_ARRAY_BUFFER, entity.flags.context);
        glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * 1024, NULL, GL_STREAM_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(unsigned int) * entity.flags.content.size(), entity.flags.content.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (entity.positions.context) {
        glBindBuffer(GL_ARRAY_BUFFER, entity.positions.context);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(spatial::matrix) * entity.positions.content.size(), entity.positions.content.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    else if (entity.positions.content.size()) {
        glGenBuffers(1, &entity.positions.context);
        glBindBuffer(GL_ARRAY_BUFFER, entity.positions.context);
        glBufferData(GL_ARRAY_BUFFER, sizeof(spatial::matrix) * 1024, NULL, GL_STREAM_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(spatial::matrix) * entity.positions.content.size(), entity.positions.content.data());
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

void implementation::opengl::graphics::draw(type::object& object, type::program& shader, const spatial::matrix& projection, const spatial::matrix& view, const spatial::matrix& model, const spatial::matrix& lighting, unsigned int options) {
    // Look for the first object with vertices, just at the top level for now
    auto &target = object;
    if (target.visible == false) {
        return;
    }
    if (target.vertices.size() == 0) {
        return;
    }

    if (target.compiled() == false) {
        compile(target);
    }

    glUseProgram(shader.context);

    if (target.texture.color && target.texture.color->context) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, target.texture.color->context);
    }
    if (target.texture.normal && target.texture.normal->context) {
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, target.texture.normal->context);
    }
    if (shadow.texture.color && shadow.texture.color->context) {
        glActiveTexture(GL_TEXTURE0 + 2);
        glBindTexture(GL_TEXTURE_2D, shadow.texture.color->context);
    }
    if (depth.texture.color && depth.texture.color->context) {
        glActiveTexture(GL_TEXTURE0 + 3);
        glBindTexture(GL_TEXTURE_2D, depth.texture.color->context);
    }
    if (blur.texture.color && blur.texture.color->context) {
        glActiveTexture(GL_TEXTURE0 + 4);
        glBindTexture(GL_TEXTURE_2D, blur.texture.color->context);
    }

    glUniformMatrix4fv(shader.u_ProjectionMatrix, 1, GL_FALSE, (GLfloat*)projection.data());
    glUniformMatrix4fv(shader.u_ViewMatrix, 1, GL_FALSE, (GLfloat*)view.data());
    glUniformMatrix4fv(shader.u_ModelMatrix, 1, GL_FALSE, (GLfloat*)model.data());
    glUniformMatrix4fv(shader.u_LightingMatrix, 1, GL_FALSE, (GLfloat*)lighting.data());

    glUniform1i(shader.u_SurfaceTextureUnit, 0);
    glUniform1i(shader.u_NormalTextureUnit, 1);
    glUniform1i(shader.u_ShadowTextureUnit, 2);
    glUniform1i(shader.u_DepthTextureUnit, 3);
    glUniform1i(shader.u_BlurTextureUnit, 4);

    glUniform4f(shader.u_AmbientLightPosition, ambient.position.eye.x, ambient.position.eye.y, ambient.position.eye.z, ambient.position.eye.w);
    glUniform4f(shader.u_AmbientLightColor, ambient.color.x, ambient.color.y, ambient.color.z, ambient.color.w);

    glUniform1f(shader.u_AmbientLightBias, ambient.bias);
    glUniform1f(shader.u_AmbientLightStrength, ambient.strength);

    glUniform4f(shader.u_Clipping, clip_top[clip_top.size()-1], clip_bottom[clip_bottom.size() - 1], clip_left[clip_left.size() - 1], clip_right[clip_right.size() - 1]);

    glUniform1ui(shader.u_Flags, object.flags);
    glUniformMatrix4fv(shader.u_Parameters, 1, GL_FALSE, (GLfloat*)parameters.data());

    glBindBuffer(GL_ARRAY_BUFFER, target.context);

    glVertexAttribPointer(shader.a_Vertex, 4, GL_FLOAT, GL_FALSE, sizeof(spatial::vertex), BUFFER_OFFSET(offset_vector));
    glVertexAttribPointer(shader.a_Texture, 4, GL_FLOAT, GL_FALSE, sizeof(spatial::vertex), BUFFER_OFFSET(offset_vector + sizeof(spatial::vector)));
    glVertexAttribPointer(shader.a_Normal, 4, GL_FLOAT, GL_TRUE, sizeof(spatial::vertex), BUFFER_OFFSET(offset_vector + (sizeof(spatial::vector) * 2)));

    glEnableVertexAttribArray(shader.a_Vertex);
    glEnableVertexAttribArray(shader.a_Texture);
    glEnableVertexAttribArray(shader.a_Normal);

    int instances = 1;
    if (object.emitter) {
        if (shader.a_ModelMatrix >= 0 && object.emitter->positions.context) {
            glBindBuffer(GL_ARRAY_BUFFER, object.emitter->positions.context);

            glVertexAttribPointer(shader.a_ModelMatrix + 0, 4, GL_FLOAT, GL_FALSE, sizeof(spatial::matrix), BUFFER_OFFSET(offset_matrix + sizeof(float) * 0));
            glVertexAttribPointer(shader.a_ModelMatrix + 1, 4, GL_FLOAT, GL_FALSE, sizeof(spatial::matrix), BUFFER_OFFSET(offset_matrix + sizeof(float) * 4));
            glVertexAttribPointer(shader.a_ModelMatrix + 2, 4, GL_FLOAT, GL_FALSE, sizeof(spatial::matrix), BUFFER_OFFSET(offset_matrix + sizeof(float) * 8));
            glVertexAttribPointer(shader.a_ModelMatrix + 3, 4, GL_FLOAT, GL_FALSE, sizeof(spatial::matrix), BUFFER_OFFSET(offset_matrix + sizeof(float) * 12));

            glEnableVertexAttribArray(shader.a_ModelMatrix + 0);
            glEnableVertexAttribArray(shader.a_ModelMatrix + 1);
            glEnableVertexAttribArray(shader.a_ModelMatrix + 2);
            glEnableVertexAttribArray(shader.a_ModelMatrix + 3);

            glVertexAttribDivisor(shader.a_ModelMatrix + 0, 1);
            glVertexAttribDivisor(shader.a_ModelMatrix + 1, 1);
            glVertexAttribDivisor(shader.a_ModelMatrix + 2, 1);
            glVertexAttribDivisor(shader.a_ModelMatrix + 3, 1);
        }

        if (shader.a_Identifier >= 0 && object.emitter->identifiers.context) {
            glBindBuffer(GL_ARRAY_BUFFER, object.emitter->identifiers.context);

            glVertexAttribPointer(shader.a_Identifier, 1, GL_FLOAT, GL_FALSE, sizeof(unsigned int), BUFFER_OFFSET(0));
            glEnableVertexAttribArray(shader.a_Identifier);
            glVertexAttribDivisor(shader.a_Identifier, 1);
        }

        if (shader.a_Flags >= 0 && object.emitter->flags.context) {
            glBindBuffer(GL_ARRAY_BUFFER, object.emitter->flags.context);

            glVertexAttribPointer(shader.a_Flags, 1, GL_FLOAT, GL_FALSE, sizeof(unsigned int), BUFFER_OFFSET(0));
            glEnableVertexAttribArray(shader.a_Flags);
            glVertexAttribDivisor(shader.a_Flags, 1);
        }

        instances = object.emitter->baked;
    }
    else {
        if (shader.a_Identifier >= 0) {
            glDisableVertexAttribArray(shader.a_Identifier);
        }
        if (shader.a_Flags >= 0) {
            glDisableVertexAttribArray(shader.a_Flags);
        }
    }

    // Draw either the solids or wireframes
    if (target.vertices.size() == 2 || options & render::WIREFRAME) {
        //glDrawArrays(GL_LINES, 0, target.vertices.size());
        glDrawArraysInstanced(GL_LINE_LOOP, 0, target.vertices.size(), instances);
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
            compile(ray);
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


void implementation::opengl::graphics::ontarget(type::object& object) {
    if (fbos.find(object.instance) == fbos.end()) {
        fbos[object.instance].init(object, this, object.depth, object.pixels.size() ? object.pixels.data() : NULL);
    }
    fbos[object.instance].enable();
    target.push_back(&object);
}

void implementation::opengl::graphics::untarget() {
    if (target.size() == 0) {
        return;
    }
    fbos[target.back()->instance].disable();
    target.pop_back();
}

void implementation::opengl::graphics::oninvert() {
    glCullFace(GL_FRONT);
}

void implementation::opengl::graphics::uninvert() {
    glCullFace(GL_BACK);
}

#endif