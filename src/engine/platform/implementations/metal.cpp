#include "engine.hpp"

#if defined __PLATFORM_SUPPORTS_METAL

struct type::info::opaque_t {
    void *ptr;
};


void implementation::metal::graphics::projection(int fov) {
    ortho = spatial::matrix().ortho(0, display_width, 0, display_height);
    perspective = spatial::matrix().perspective(fov, (float)display_width / (float)display_height, 0.0f, 100.0f);
}

void implementation::metal::graphics::dimensions(int width, int height, float scale) {
    bool init = (display_width == 0 && display_height == 0);

    display_width = width;
    display_height = height;
}

void implementation::metal::graphics::init(void) {
    MTL::DepthStencilDescriptor* pDsDesc = MTL::DepthStencilDescriptor::alloc()->init();
    pDsDesc->setDepthCompareFunction( MTL::CompareFunction::CompareFunctionLess );
    pDsDesc->setDepthWriteEnabled( true );

    _pDepthStencilState = _pDevice->newDepthStencilState( pDsDesc );

    pDsDesc->release();
}

void implementation::metal::graphics::clear(void) {
    // TODO
}

void implementation::metal::graphics::flush(void) {
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

bool implementation::metal::graphics::compile(type::shader& shader) {
    if (shader.compile() == false) {
        return false;
    }

    if (shader.resource == NULL) {
        shader.resource = new type::info::opaque_t;
    }
    
    NS::Error* pError = nullptr;
    MTL::Library* pLibrary = _pDevice->newLibrary( NS::String::string(shader.text.c_str(), NS::UTF8StringEncoding), nullptr, &pError );
    if ( !pLibrary )
    {
        __builtin_printf( "%s", pError->localizedDescription()->utf8String() );
        assert( false );
    }

    shader.resource->ptr = pLibrary;
    
    return true;
}

bool implementation::metal::graphics::compile(type::program& program) {
    if (program.compile() == false) {
        return false;
    }

    if (compile(program.unified) == false) {
        return false;
    }
    
    if (program.resource == NULL) {
        program.resource = new type::info::opaque_t;
    }

    NS::Error* pError = nullptr;

    auto pLibrary = reinterpret_cast<MTL::Library *>(program.unified.resource->ptr);
    
    MTL::Function* pVertexFn = pLibrary->newFunction( NS::String::string("vertexMain", NS::UTF8StringEncoding) );
    MTL::Function* pFragFn = pLibrary->newFunction( NS::String::string("fragmentMain", NS::UTF8StringEncoding) );

    MTL::RenderPipelineDescriptor* pDesc = MTL::RenderPipelineDescriptor::alloc()->init();
    pDesc->setVertexFunction( pVertexFn );
    pDesc->setFragmentFunction( pFragFn );
    pDesc->colorAttachments()->object(0)->setPixelFormat( MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB );
    pDesc->setDepthAttachmentPixelFormat( MTL::PixelFormat::PixelFormatDepth16Unorm );

    program.resource->ptr = _pDevice->newRenderPipelineState( pDesc, &pError );
    if ( program.resource->ptr == NULL )
    {
        __builtin_printf( "%s", pError->localizedDescription()->utf8String() );
        assert( false );
    }

    pVertexFn->release();
    pFragFn->release();
    pDesc->release();
    
    return true;
}

bool implementation::metal::graphics::compile(type::material& material) {
    if (material.compile() == false) {
        return false;
    }
    if (material.color == NULL && material.normal == NULL) {
        return false;
    }

    if (material.color->resource == NULL) {
        material.color->resource = new type::info::opaque_t;
    }

    auto tw = material.color->properties.width;
    auto th = material.color->properties.height;
    
    MTL::TextureDescriptor* pTextureDesc = MTL::TextureDescriptor::alloc()->init();
    pTextureDesc->setWidth( material.color->properties.width );
    pTextureDesc->setHeight( material.color->properties.height );
    pTextureDesc->setPixelFormat( MTL::PixelFormatRGBA8Unorm );
    pTextureDesc->setTextureType( MTL::TextureType2D );
    pTextureDesc->setStorageMode( MTL::StorageModeManaged );
    pTextureDesc->setUsage( MTL::ResourceUsageSample | MTL::ResourceUsageRead );

    MTL::Texture *pTexture = _pDevice->newTexture( pTextureDesc );
    
    material.color->resource->ptr = (type::info::opaque_t *)pTexture;

    uint8_t* pTextureData = (uint8_t *)alloca( tw * th * 4 );
    for ( size_t y = 0; y < th; ++y )
    {
        for ( size_t x = 0; x < tw; ++x )
        {
            bool isWhite = (x^y) & 0b1000000;
            uint8_t c = isWhite ? 0xFF : 0xA;

            size_t i = y * tw + x;

            pTextureData[ i * 4 + 0 ] = c;
            pTextureData[ i * 4 + 1 ] = c;
            pTextureData[ i * 4 + 2 ] = c;
            pTextureData[ i * 4 + 3 ] = 0xFF;
        }
    }

    pTexture->replaceRegion( MTL::Region( 0, 0, 0, tw, th, 1 ), 0, (uint8_t*)material.color->raster.data(), tw * 4 );

    pTextureDesc->release();
    
    return true;
}

bool implementation::metal::graphics::compile(type::object& object) {
    for (auto &child : object.children) {
        compile(child);
    }
    if (object.compile() == false || object.vertices.size() == 0) {
        return false;
    }

    if (object.resource == NULL) {
        object.resource = new type::info::opaque_t;
    }
    
    const size_t vertexDataSize = sizeof(spatial::vertex) * object.vertices.size();
    
    MTL::Buffer* pVertexBuffer = _pDevice->newBuffer( vertexDataSize, MTL::ResourceStorageModeManaged );

    object.resource->ptr = (type::info::opaque_t *)pVertexBuffer;

    memcpy( pVertexBuffer->contents(), object.vertices.data(), vertexDataSize );

    pVertexBuffer->didModifyRange( NS::Range::Make( 0, pVertexBuffer->length() ) );

    compile(object.texture);

    return true;
}

bool implementation::metal::graphics::compile(type::font& font) {
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

bool implementation::metal::graphics::compile(type::entity& entity) {
    entity.bake();

    // TODO

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

bool implementation::metal::graphics::compile(platform::assets* assets) {
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

void implementation::metal::graphics::draw(type::object& object, type::program& shader, const spatial::matrix& projection, const spatial::matrix& view, const spatial::matrix& model, const spatial::matrix& lighting, unsigned int options) {

    MTL::RenderPassDescriptor* pRpd = _pView->currentRenderPassDescriptor();
    MTL::RenderCommandEncoder* pEnc = _pCmd->renderCommandEncoder( pRpd );
    
    //pEnc->setDepthStencilState( _pDepthStencilState );

    
    auto _pPSO = reinterpret_cast<MTL::RenderPipelineState*>(shader.resource->ptr);
    
    pEnc->setRenderPipelineState( _pPSO );
    
    auto _pVertexDataBuffer = reinterpret_cast<MTL::Buffer*>(object.resource->ptr);

    pEnc->setVertexBuffer( _pVertexDataBuffer, /* offset */ 0, /* index */ 0 );
    //pEnc->setVertexBuffer( pInstanceDataBuffer, /* offset */ 0, /* index */ 1 );
    //pEnc->setVertexBuffer( pCameraDataBuffer, /* offset */ 0, /* index */ 2 );

    auto _pTexture = reinterpret_cast<MTL::Texture*>(object.texture.color->resource->ptr);
    
    pEnc->setFragmentTexture( _pTexture, /* index */ 0 );

    pEnc->setCullMode( MTL::CullModeBack );
    pEnc->setFrontFacingWinding( MTL::Winding::WindingCounterClockwise );
    
    pEnc->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, 0, object.vertices.size(), 1);

    //pEnc->drawIndexedPrimitives( MTL::PrimitiveType::PrimitiveTypeTriangle, 6 * 6, MTL::IndexType::IndexTypeUInt16, _pIndexBuffer, 0, kNumInstances );

    pEnc->endEncoding();
    
    _pCmd->presentDrawable( _pView->currentDrawable() );
    _pCmd->commit();

}

void implementation::metal::graphics::draw(std::string text, type::font& font, type::program& shader, const spatial::matrix& projection, const spatial::matrix& view, const spatial::matrix& model, const spatial::matrix& lighting, unsigned int options) {
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


void implementation::metal::graphics::ontarget(type::object& object) {
    // TODO
}

void implementation::metal::graphics::untarget() {
    // TODO
}

void implementation::metal::graphics::oninvert() {
    // TODO
}

void implementation::metal::graphics::uninvert() {
    // TODO
}


#endif


