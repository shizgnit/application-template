#include "engine.hpp"

#if defined __PLATFORM_SUPPORTS_OPENGL

std::string glGetErrorString(GLenum err) {
    std::stringstream str;
    str << err;
    switch (err) {
        case GL_NO_ERROR: return "GL_NO_ERROR(" + str.str() + ")";
        case GL_INVALID_ENUM: return "GL_INVALID_ENUM(" + str.str() + ")";
        case GL_INVALID_VALUE: return "GL_INVALID_VALUE(" + str.str() + ")";
        case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION(" + str.str() + ")";
        case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY(" + str.str() + ")";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION(" + str.str() + ")";
        case GL_FRAMEBUFFER_UNDEFINED: return "GL_FRAMEBUFFER_UNDEFINED(" + str.str() + ")";
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT(" + str.str() + ")";
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT(" + str.str() + ")";
        case GL_FRAMEBUFFER_UNSUPPORTED: return "GL_FRAMEBUFFER_UNSUPPORTED(" + str.str() + ")";
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE(" + str.str() + ")";
        default:
            return "GL_ERROR(" + str.str() + ")";
    }
}


static GLint defaultFramebuffer = 0;

bool implementation::opengl::fbo::deinit() {
    if (context.render) {
        GL_TEST(glBindRenderbuffer(GL_RENDERBUFFER, context.render));
        GL_TEST(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0));
        GL_TEST(glDeleteRenderbuffers(1, &context.render));
    }
    if (context.frame) {
        GL_TEST(glBindFramebuffer(GL_FRAMEBUFFER, context.frame));
        GL_TEST(glFramebufferTexture2D(GL_FRAMEBUFFER, allocation, GL_TEXTURE_2D, 0, 0));
        GL_TEST(glDeleteFramebuffers(1, &context.frame));
    }
    return true;
}

bool implementation::opengl::fbo::init(type::object& object, platform::graphics *ref, bool depth, unsigned char *collector) {
    if (allocation == 0) {
        allocation = object.texture.depth ? GL_DEPTH_ATTACHMENT : attachments().allocate();
        if (allocation == 0 || object.texture.color == NULL) {
            return false;
        }
    }

    collect = collector;

    parent = ref;

    GL_TEST(glGenFramebuffers(1, &context.frame));
    GL_TEST(glBindFramebuffer(GL_FRAMEBUFFER, context.frame));
    GL_TEST(glFramebufferTexture2D(GL_FRAMEBUFFER, allocation, GL_TEXTURE_2D, object.texture.color->resource->context, 0));

    GL_TEST(glGenRenderbuffers(1, &context.render));
    GL_TEST(glBindRenderbuffer(GL_RENDERBUFFER, context.render));
    GL_TEST(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, object.texture.color->properties.width, object.texture.color->properties.height));
    GL_TEST(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, context.render));

    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        GL_REPORT_ERROR("glCheckFramebufferStatus", glGetErrorString(status));
    }

    GL_TEST(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    GL_TEST(glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer));

    target = &object;

    return true;
}

void implementation::opengl::fbo::enable(bool clear) {
    if (target == NULL) {
        return;
    }
    GL_TEST(glBindFramebuffer(GL_FRAMEBUFFER, context.frame));
    GL_TEST(glBindRenderbuffer(GL_RENDERBUFFER, context.render));

    //if(parent->width() != target->texture.color->properties.width || parent->height() != target->texture.color->properties.height) {
        GL_TEST(glViewport(0, 0, target->texture.color->properties.width, target->texture.color->properties.height));
    //}
    GL_TEST(glClear(clear ? GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT : GL_DEPTH_BUFFER_BIT));
}

void implementation::opengl::fbo::disable() {
    if (target == NULL || target->texture.color == NULL) {
        return;
    }

    // Currently all rendering is done to the mipmap level 0... copy it out after every render
    if (collect != NULL) {
        GL_TEST(glReadPixels(0, 0, target->texture.color->properties.width, target->texture.color->properties.height, GL_RGBA, GL_UNSIGNED_BYTE, collect));
    }
    if (!target->texture.depth) {
        GL_TEST(glBindTexture(GL_TEXTURE_2D, target->texture.color->resource->context));
        GL_TEST(glGenerateMipmap(GL_TEXTURE_2D));
        GL_TEST(glBindTexture(GL_TEXTURE_2D, 0));
    }
    GL_TEST(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    GL_TEST(glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer));

    //if(parent->width() != target->texture.color->properties.width || parent->height() != target->texture.color->properties.height) {
        GL_TEST(glViewport(0, 0, parent->width(), parent->height()));
    //}
    
    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        GL_REPORT_ERROR("glCheckFramebufferStatus", glGetErrorString(status));
    }
}

void implementation::opengl::graphics::projection(int fov) {
    ortho = spatial::matrix().ortho(0, display_width, 0, display_height);
    perspective = spatial::matrix().perspective(fov, (float)display_width / (float)display_height, 0.0f, 10.0f);
}

void implementation::opengl::graphics::dimensions(int width, int height, float scale) {
    if(width == 0 || height == 0) {
        return;
    }
    
    //if(display_width == 0 && display_height == 0) {
    //    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFramebuffer);
        
    //    GLint dims[4] = {0};
    //    glGetIntegerv(GL_VIEWPORT, dims);
        
    //    display_width = dims[2];
    //    display_height = dims[3];
    //}
    //else {
        display_width = width;
        display_height = height;
    //}
    
    event(utilities::string() << "glViewport(" << display_width << "x" << display_height << ")");
    
    GL_TEST(glViewport(0, 0, display_width, display_height));

    // Setup the render buffer
    fbos[color.instance].deinit();
    color = spatial::quad(display_width, display_height);
    color.texture.color = &assets->get<type::image>("color");
    color.texture.color->create(display_width, display_height, 0, 0, 0, 0);
    color.xy_projection(0, 0, display_width, display_height, false, true);
    compile(color);
    fbos[color.instance].init(color, this);

    // Setup the scene depth buffer
    fbos[depth.instance].deinit();
    depth = spatial::quad(display_width, display_height);
    depth.texture.color = &assets->get<type::image>("depth");
    depth.texture.color->create(display_width, display_height, 0, 0, 0, 0);
    //depth.texture.depth = true;
    depth.xy_projection(0, 0, display_width, display_height, false, true);
    compile(depth);
    fbos[depth.instance].init(depth, this);

    // Setup the post process blur buffer
    fbos[blur.instance].deinit();
    blur = spatial::quad(display_width * scale, display_height * scale);
    blur.texture.color = &assets->get<type::image>("blur");
    blur.texture.color->create(display_width * scale, display_height * scale, 0, 0, 0, 0);
    blur.xy_projection(0, 0, display_width * scale, display_height * scale, false, true);
    compile(blur);
    fbos[blur.instance].init(blur, this);

    // Setup the picking buffer
    fbos[picking.instance].deinit();
    picking = spatial::quad(display_width, display_height);
    picking.texture.color = &assets->get<type::image>("picking");
    picking.texture.color->create(display_width, display_height, 0, 0, 0, 0);
    picking.xy_projection(0, 0, display_width, display_height, false, true);
    compile(picking);
    pixels.resize(display_width * display_height * 4, 0);
    fbos[picking.instance].init(picking, this, false, pixels.data());
}

void implementation::opengl::graphics::init(void) {
    // Depth test
    GL_TEST(glEnable(GL_DEPTH_TEST));
    GL_TEST(glDepthFunc(GL_LEQUAL));
    GL_TEST(glDepthMask(GL_TRUE));
    GL_TEST(glClearDepthf(1.0f));
    GL_TEST(glDepthRangef(0.0f, 1.0f));

    // Alpha blending
    GL_TEST(glEnable(GL_BLEND));
    GL_TEST(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    // Backface culling, makes GLES rendering of objects easier since they don't need to be drawn back to front
    GL_TEST(glEnable(GL_CULL_FACE));
    GL_TEST(glFrontFace(GL_CCW));
    GL_TEST(glCullFace(GL_BACK));

    auto glsl = glGetString(GL_SHADING_LANGUAGE_VERSION);

    //auto str = utilities::tokenize((const char*)glGetString(GL_EXTENSIONS), " ");

    // Setup the ray used for drawing normals
    ray = spatial::ray(spatial::vector(0.0, 0.0, 0.0), spatial::vector(2.0, 0.0, 0.0));
    ray.texture.color = &assets->get<type::image>("ray");
    ray.texture.color->create(1, 1, 255, 255, 255, 255);
    ray.xy_projection(0, 0, 1, 1);
    compile(ray);

    // Setup the shadow depth map
    shadow = spatial::quad(256, 256);
    shadow.texture.color = &assets->get<type::image>("shadowmap");
    shadow.texture.color->create(4096, 4096, 0, 0, 0, 0);
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
    
    auto size_vertex = sizeof(spatial::vertex);
    auto size_vector = sizeof(spatial::vector);
    
    auto size_type = sizeof(spatial::vector::type_t);
    
    auto sum_type = size_type * 3;
    auto sum_vector = size_vector * 3;
    
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
    GL_TEST(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GL_TEST(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));
}

void implementation::opengl::graphics::flush(void) {
    GL_TEST(glFlush());
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
    if (shader.compiled()) {
        return false;
    }

    if (shader.resource == NULL) {
        shader.resource = new type::info::opaque_t();
    }

    switch (shader.format()) {
    case(type::format::FORMAT_VERT):
        shader.resource->context = glCreateShader(GL_VERTEX_SHADER);
        break;
    case(type::format::FORMAT_FRAG):
        shader.resource->context = glCreateShader(GL_FRAGMENT_SHADER);
        break;
    }
    if (!shader.resource->context) {
        return false;
    }

    GLchar* text = (GLchar*)shader.text.c_str();
    GLint length = strlen(text);
    GL_TEST(glShaderSource(shader.resource->context, 1, (const GLchar**)&text, &length));

    GL_TEST(glCompileShader(shader.resource->context));
    
    GLint compiled = GL_FALSE;
    GL_TEST(glGetShaderiv(shader.resource->context, GL_COMPILE_STATUS, &compiled));
    if (compiled == GL_FALSE) {
        glGetShaderiv(shader.resource->context, GL_INFO_LOG_LENGTH, &length);
        if (length) {
            char* info = (char*)malloc(length);
            glGetShaderInfoLog(shader.resource->context, length, NULL, info);
            event(info);
            free(info);
            glDeleteShader(shader.resource->context);
            shader.resource->context = 0;
            return false;
        }
    }

    return shader.compiled(true);
}

bool implementation::opengl::graphics::compile(type::program& program) {
    if (program.compiled()) {
        return false;
    }

    if (compile(program.vertex) == false) {
        return false;
    }
    if (compile(program.fragment) == false) {
        return false;
    }

    if (program.resource == NULL) {
        program.resource = new type::info::opaque_t();
    }

    program.resource->context = glCreateProgram();
    if (!program.resource->context) {
        return false;
    }

    GL_TEST(glAttachShader(program.resource->context, program.vertex.resource->context));
    GL_TEST(glAttachShader(program.resource->context, program.fragment.resource->context));

    GL_TEST(glLinkProgram(program.resource->context));

    GLint linked = GL_FALSE;
    GL_TEST(glGetProgramiv(program.resource->context, GL_LINK_STATUS, &linked));
    if (linked != GL_TRUE) {
        GLint length = 0;
        glGetProgramiv(program.resource->context, GL_INFO_LOG_LENGTH, &length);
        if (length) {
            char* info = (char*)malloc(length);
            glGetProgramInfoLog(program.resource->context, length, NULL, info);
            event(info);
            free(info);
        }
        glDeleteProgram(program.resource->context);
        program.resource->context = 0;
    }

    GL_TEST(glUseProgram(program.resource->context));

    program.a_ModelMatrix = glGetAttribLocation(program.resource->context, "a_ModelMatrix");

    program.a_Identifier = glGetAttribLocation(program.resource->context, "a_Identifier");
    program.a_Flags = glGetAttribLocation(program.resource->context, "a_Flags");

    program.a_Vertex = glGetAttribLocation(program.resource->context, "a_Vertex");
    program.a_Texture = glGetAttribLocation(program.resource->context, "a_Texture");
    program.a_Normal = glGetAttribLocation(program.resource->context, "a_Normal");

    program.u_ProjectionMatrix = glGetUniformLocation(program.resource->context, "u_ProjectionMatrix");
    program.u_ViewMatrix = glGetUniformLocation(program.resource->context, "u_ViewMatrix");
    program.u_ModelMatrix = glGetUniformLocation(program.resource->context, "u_ModelMatrix");
    program.u_LightingMatrix = glGetUniformLocation(program.resource->context, "u_LightingMatrix");

    program.u_Clipping = glGetUniformLocation(program.resource->context, "u_Clipping");

    program.u_AmbientLightPosition = glGetUniformLocation(program.resource->context, "u_AmbientLightPosition");
    program.u_AmbientLightColor = glGetUniformLocation(program.resource->context, "u_AmbientLightColor");
    program.u_AmbientLightBias = glGetUniformLocation(program.resource->context, "u_AmbientLightBias");
    program.u_AmbientLightStrength = glGetUniformLocation(program.resource->context, "u_AmbientLightStrength");

    program.u_Flags = glGetUniformLocation(program.resource->context, "u_Flags");
    program.u_Parameters = glGetUniformLocation(program.resource->context, "u_Parameters");

    program.u_SurfaceTextureUnit = glGetUniformLocation(program.resource->context, "u_SurfaceTextureUnit");
    program.u_NormalTextureUnit = glGetUniformLocation(program.resource->context, "u_NormalTextureUnit");
    program.u_ShadowTextureUnit = glGetUniformLocation(program.resource->context, "u_ShadowTextureUnit");
    program.u_DepthTextureUnit = glGetUniformLocation(program.resource->context, "u_DepthTextureUnit");
    program.u_BlurTextureUnit = glGetUniformLocation(program.resource->context, "u_BlurTextureUnit");
    program.u_PickingTextureUnit = glGetUniformLocation(program.resource->context, "u_PickingTextureUnit");

    program.u_TextureSize = glGetUniformLocation(program.resource->context, "u_TextureSize");
    
    return program.compiled(true);
}

bool implementation::opengl::graphics::compile(type::material& material) {
    if (material.compiled()) {
        return false;
    }
    if (material.color == NULL && material.normal == NULL) {
        return false;
    }

    if (material.color) {
        if (material.color->resource == NULL) {
            material.color->resource = new type::info::opaque_t();
        }
        if (material.color->resource->context) {
            GL_TEST(glDeleteTextures(1, &material.color->resource->context));
        }
        if (material.depth) {
            GL_TEST(glGenTextures(1, &material.color->resource->context));
            GL_TEST(glBindTexture(GL_TEXTURE_2D, material.color->resource->context));
            GL_TEST(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, material.color->properties.width, material.color->properties.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
            GL_TEST(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            GL_TEST(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            GL_TEST(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            GL_TEST(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        }
        else {
            GL_TEST(glGenTextures(1, &material.color->resource->context));
            GL_TEST(glBindTexture(GL_TEXTURE_2D, material.color->resource->context));
            GL_TEST(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
            GL_TEST(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            GL_TEST(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, material.color->properties.width, material.color->properties.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, material.color->raster.data()));
            GL_TEST(glGenerateMipmap(GL_TEXTURE_2D));
        }
    }

    if (material.normal) {
        if (material.normal->resource == NULL) {
            material.normal->resource = new type::info::opaque_t();
        }
        if (material.normal->resource->context) {
            GL_TEST(glDeleteTextures(1, &material.normal->resource->context));
        }
        GL_TEST(glGenTextures(1, &material.normal->resource->context));
        GL_TEST(glBindTexture(GL_TEXTURE_2D, material.normal->resource->context));
        GL_TEST(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GL_TEST(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL_TEST(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, material.normal->properties.width, material.normal->properties.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, material.normal->raster.data()));
        GL_TEST(glGenerateMipmap(GL_TEXTURE_2D));
    }

    GL_TEST(glBindTexture(GL_TEXTURE_2D, 0));

    return material.compiled(true);
}

bool implementation::opengl::graphics::compile(type::object& object) {
    for (auto &child : object.children) {
        compile(child);
    }
    
    if (object.vertices.size() == 0) {
        return false;
    }
    
    compile(object.texture);
    
    type::program* shader = NULL;
    for(auto ptr=renderer.rbegin(); shader == NULL && ptr != renderer.rend(); ptr++) {
        shader = *ptr;
    }
    if(shader == NULL) {
        return false;
    }

    if (object.resource == NULL) {
        object.resource = new type::info::opaque_t();
    }
    
    if (object.resource->context != 0 && object.resource->vao.find(shader) != object.resource->vao.end()) {
        GL_TEST(glBindVertexArray(object.resource->vao[shader]));
        GL_TEST(glBindBuffer(GL_ARRAY_BUFFER, object.resource->context));
        GL_TEST(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(spatial::vertex) * object.vertices.size(), object.vertices.data()));
        if(object.emitter && object.emitter->bake() && object.emitter->identifiers.resource) {
            auto entity = *object.emitter;
            GL_TEST(glBindBuffer(GL_ARRAY_BUFFER, entity.identifiers.resource->context));
            GL_TEST(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(unsigned int) * entity.identifiers.content.size(), entity.identifiers.content.data()));
            GL_TEST(glBindBuffer(GL_ARRAY_BUFFER, entity.flags.resource->context));
            GL_TEST(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(unsigned int) * entity.flags.content.size(), entity.flags.content.data()));
            GL_TEST(glBindBuffer(GL_ARRAY_BUFFER, entity.flags.resource->context));
            GL_TEST(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(unsigned int) * entity.flags.content.size(), entity.flags.content.data()));
            return true;
        }
        //return true;
    }
    else {
        GL_TEST(glGenVertexArrays(1, &object.resource->vao[shader]));
        GL_TEST(glBindVertexArray(object.resource->vao[shader]));

        GL_TEST(glGenBuffers(1, &object.resource->context));
        GL_TEST(glBindBuffer(GL_ARRAY_BUFFER, object.resource->context));
        GL_TEST(glBufferData(GL_ARRAY_BUFFER, sizeof(spatial::vertex) * object.vertices.size(), object.vertices.data(), GL_STATIC_DRAW));
        
        GL_TEST(glVertexAttribPointer(shader->a_Vertex, 4, GL_FLOAT, GL_FALSE, sizeof(spatial::vertex), BUFFER_OFFSET(offset_vector)));
        GL_TEST(glVertexAttribPointer(shader->a_Texture, 4, GL_FLOAT, GL_FALSE, sizeof(spatial::vertex), BUFFER_OFFSET(sizeof(spatial::vector) + offset_vector)));
        GL_TEST(glVertexAttribPointer(shader->a_Normal, 4, GL_FLOAT, GL_TRUE, sizeof(spatial::vertex), BUFFER_OFFSET((sizeof(spatial::vector) * 2) + offset_vector)));
        
        GL_TEST(glEnableVertexAttribArray(shader->a_Vertex));
        GL_TEST(glEnableVertexAttribArray(shader->a_Texture));
        GL_TEST(glEnableVertexAttribArray(shader->a_Normal));
    }

    if(object.emitter && object.emitter->bake()) {
        auto entity = *object.emitter;
        
        if (entity.identifiers.resource == NULL) {
            entity.identifiers.resource = new type::info::opaque_t();
        }
        if (entity.identifiers.content.size()) {
            GL_TEST(glGenBuffers(1, &entity.identifiers.resource->context));
            GL_TEST(glBindBuffer(GL_ARRAY_BUFFER, entity.identifiers.resource->context));
            GL_TEST(glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * 1024, NULL, GL_DYNAMIC_DRAW));
            GL_TEST(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(unsigned int) * entity.identifiers.content.size(), entity.identifiers.content.data()));
            GL_TEST(glVertexAttribIPointer(shader->a_Identifier, 1, GL_UNSIGNED_INT, sizeof(unsigned int), BUFFER_OFFSET(0)));
            GL_TEST(glEnableVertexAttribArray(shader->a_Identifier));
            GL_TEST(glVertexAttribDivisor(shader->a_Identifier, 1));
        }

        if (entity.flags.resource == NULL) {
            entity.flags.resource = new type::info::opaque_t();
        }
        if (entity.flags.content.size()) {
            GL_TEST(glGenBuffers(1, &entity.flags.resource->context));
            GL_TEST(glBindBuffer(GL_ARRAY_BUFFER, entity.flags.resource->context));
            GL_TEST(glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * 1024, NULL, GL_DYNAMIC_DRAW));
            GL_TEST(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(unsigned int) * entity.flags.content.size(), entity.flags.content.data()));
            GL_TEST(glBindBuffer(GL_ARRAY_BUFFER, entity.flags.resource->context));

            GL_TEST(glVertexAttribIPointer(shader->a_Flags, 1, GL_UNSIGNED_INT, sizeof(unsigned int), BUFFER_OFFSET(0)));
            GL_TEST(glEnableVertexAttribArray(shader->a_Flags));
            GL_TEST(glVertexAttribDivisor(shader->a_Flags, 1));
        }

        if (entity.positions.resource == NULL) {
            entity.positions.resource = new type::info::opaque_t();
        }
        if (entity.positions.content.size()) {
            GL_TEST(glGenBuffers(1, &entity.positions.resource->context));
            GL_TEST(glBindBuffer(GL_ARRAY_BUFFER, entity.positions.resource->context));
            GL_TEST(glBufferData(GL_ARRAY_BUFFER, sizeof(spatial::matrix) * 1024, NULL, GL_DYNAMIC_DRAW));
            GL_TEST(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(spatial::matrix) * entity.positions.content.size(), entity.positions.content.data()));
            
            GL_TEST(glBindBuffer(GL_ARRAY_BUFFER, entity.positions.resource->context));

            GL_TEST(glVertexAttribPointer(shader->a_ModelMatrix + 0, 4, GL_FLOAT, GL_FALSE, sizeof(spatial::matrix), BUFFER_OFFSET(offset_matrix + sizeof(float) * 0)));
            GL_TEST(glVertexAttribPointer(shader->a_ModelMatrix + 1, 4, GL_FLOAT, GL_FALSE, sizeof(spatial::matrix), BUFFER_OFFSET(offset_matrix + sizeof(float) * 4)));
            GL_TEST(glVertexAttribPointer(shader->a_ModelMatrix + 2, 4, GL_FLOAT, GL_FALSE, sizeof(spatial::matrix), BUFFER_OFFSET(offset_matrix + sizeof(float) * 8)));
            GL_TEST(glVertexAttribPointer(shader->a_ModelMatrix + 3, 4, GL_FLOAT, GL_FALSE, sizeof(spatial::matrix), BUFFER_OFFSET(offset_matrix + sizeof(float) * 12)));

            GL_TEST(glEnableVertexAttribArray(shader->a_ModelMatrix + 0));
            GL_TEST(glEnableVertexAttribArray(shader->a_ModelMatrix + 1));
            GL_TEST(glEnableVertexAttribArray(shader->a_ModelMatrix + 2));
            GL_TEST(glEnableVertexAttribArray(shader->a_ModelMatrix + 3));

            GL_TEST(glVertexAttribDivisor(shader->a_ModelMatrix + 0, 1));
            GL_TEST(glVertexAttribDivisor(shader->a_ModelMatrix + 1, 1));
            GL_TEST(glVertexAttribDivisor(shader->a_ModelMatrix + 2, 1));
            GL_TEST(glVertexAttribDivisor(shader->a_ModelMatrix + 3, 1));
        }
    }
    
    GL_TEST(glBindVertexArray(0));

    return true;
}

bool implementation::opengl::graphics::compile(type::font& font) {
    if (font.compiled()) {
        return false;
    }

    for (auto& glyph : font.glyphs) {
        if (glyph.identifier) {
            compile(glyph.quad);
        }
    }

    return font.compiled(true);
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
    static std::mutex lockgl;
    
    std::lock_guard<std::mutex> scoped(lockgl);
    
    if(shader.compiled() == false) {
        compile(shader);
    }
    
    // Look for the first object with vertices, just at the top level for now
    if (object.visible == false) {
        return;
    }
    if (object.vertices.size() == 0) {
        return;
    }

    renderer.push_back(&shader);
    compile(object);
    renderer.pop_back();
    
    if(object.resource->vao.find(&shader) != object.resource->vao.end()) {
        GL_TEST(glBindVertexArray(object.resource->vao[&shader]));
    }

    GL_TEST(glUseProgram(shader.resource->context));

    if (object.texture.color && object.texture.color->resource && object.texture.color->resource->context) {
        GL_TEST(glActiveTexture(GL_TEXTURE0));
        GL_TEST(glBindTexture(GL_TEXTURE_2D, object.texture.color->resource->context));
    }
    if (object.texture.normal && object.texture.normal->resource && object.texture.normal->resource->context) {
        GL_TEST(glActiveTexture(GL_TEXTURE0 + 1));
        GL_TEST(glBindTexture(GL_TEXTURE_2D, object.texture.normal->resource->context));
    }
    if (shadow.texture.color && shadow.texture.color->resource && shadow.texture.color->resource->context) {
        GL_TEST(glActiveTexture(GL_TEXTURE0 + 2));
        GL_TEST(glBindTexture(GL_TEXTURE_2D, shadow.texture.color->resource->context));
    }
    if (depth.texture.color && depth.texture.color->resource && depth.texture.color->resource->context) {
        GL_TEST(glActiveTexture(GL_TEXTURE0 + 3));
        GL_TEST(glBindTexture(GL_TEXTURE_2D, depth.texture.color->resource->context));
    }
    if (blur.texture.color && blur.texture.color->resource && blur.texture.color->resource->context) {
        GL_TEST(glActiveTexture(GL_TEXTURE0 + 4));
        GL_TEST(glBindTexture(GL_TEXTURE_2D, blur.texture.color->resource->context));
    }

    GL_TEST(glUniform1i(shader.u_SurfaceTextureUnit, 0));
    GL_TEST(glUniform1i(shader.u_NormalTextureUnit, 1));
    GL_TEST(glUniform1i(shader.u_ShadowTextureUnit, 2));
    GL_TEST(glUniform1i(shader.u_DepthTextureUnit, 3));
    GL_TEST(glUniform1i(shader.u_BlurTextureUnit, 4));

    if(object.texture.color) {
        GL_TEST(glUniform2f(shader.u_TextureSize, object.texture.color->properties.width, object.texture.color->properties.width));
    }
    
    GL_TEST(glUniformMatrix4fv(shader.u_ProjectionMatrix, 1, GL_FALSE, (GLfloat*)projection.data()));
    GL_TEST(glUniformMatrix4fv(shader.u_ViewMatrix, 1, GL_FALSE, (GLfloat*)view.data()));
    GL_TEST(glUniformMatrix4fv(shader.u_ModelMatrix, 1, GL_FALSE, (GLfloat*)model.data()));
    GL_TEST(glUniformMatrix4fv(shader.u_LightingMatrix, 1, GL_FALSE, (GLfloat*)lighting.data()));

    GL_TEST(glUniform4f(shader.u_AmbientLightPosition, ambient.position.eye.x, ambient.position.eye.y, ambient.position.eye.z, ambient.position.eye.w));
    GL_TEST(glUniform4f(shader.u_AmbientLightColor, ambient.color.x, ambient.color.y, ambient.color.z, ambient.color.w));
    GL_TEST(glUniform1f(shader.u_AmbientLightBias, ambient.bias));
    GL_TEST(glUniform1f(shader.u_AmbientLightStrength, ambient.strength));

    GL_TEST(glUniform4f(shader.u_Clipping, clip_top[clip_top.size()-1], clip_bottom[clip_bottom.size() - 1], clip_left[clip_left.size() - 1], clip_right[clip_right.size() - 1]));

    GL_TEST(glUniform1ui(shader.u_Flags, object.flags));

    GL_TEST(glUniformMatrix4fv(shader.u_Parameters, 1, GL_FALSE, (GLfloat*)parameters.data()));
    
    // Draw either the solids or wireframes
    int instances = object.emitter ? object.emitter->baked : 1;
    if (object.vertices.size() == 2 || options & render::WIREFRAME) {
        GL_TEST(glDrawArraysInstanced(GL_LINE_LOOP, 0, (int)object.vertices.size(), instances));
        frame.lines += object.vertices.size() / 2;
    }
    else {
        GL_TEST(glDrawArraysInstanced(GL_TRIANGLES, 0, (int)object.vertices.size(), instances));
        frame.triangles += object.vertices.size() / 3;
    }
    frame.vertices += object.vertices.size();

    GL_TEST(glBindVertexArray(0));

    // Primary object has been drawn, draw out the normals if requested.  Mostly for debugging.
    if (options & render::NORMALS) {
        for (auto vertex : object.vertices) {
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
    GL_TEST(glCullFace(GL_FRONT));
}

void implementation::opengl::graphics::uninvert() {
    GL_TEST(glCullFace(GL_BACK));
}

#endif
