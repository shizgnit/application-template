#include "engine.hpp"

#if defined __PLATFORM_SUPPORTS_METAL

#include <simd/simd.h>

namespace math2
{
    constexpr simd::float3 add( const simd::float3& a, const simd::float3& b )
    {
        return { a.x + b.x, a.y + b.y, a.z + b.z };
    }

    constexpr simd_float4x4 makeIdentity()
    {
        using simd::float4;
        return (simd_float4x4){ (float4){ 1.f, 0.f, 0.f, 0.f },
                                (float4){ 0.f, 1.f, 0.f, 0.f },
                                (float4){ 0.f, 0.f, 1.f, 0.f },
                                (float4){ 0.f, 0.f, 0.f, 1.f } };
    }

    simd::float4x4 makePerspective( float fovRadians, float aspect, float znear, float zfar )
    {
        using simd::float4;
        float ys = 1.f / tanf(fovRadians * 0.5f);
        float xs = ys / aspect;
        float zs = zfar / ( znear - zfar );
        return simd_matrix_from_rows((float4){ xs, 0.0f, 0.0f, 0.0f },
                                     (float4){ 0.0f, ys, 0.0f, 0.0f },
                                     (float4){ 0.0f, 0.0f, zs, znear * zs },
                                     (float4){ 0, 0, -1, 0 });
    }

    simd::float4x4 makeXRotate( float angleRadians )
    {
        using simd::float4;
        const float a = angleRadians;
        return simd_matrix_from_rows((float4){ 1.0f, 0.0f, 0.0f, 0.0f },
                                     (float4){ 0.0f, cosf( a ), sinf( a ), 0.0f },
                                     (float4){ 0.0f, -sinf( a ), cosf( a ), 0.0f },
                                     (float4){ 0.0f, 0.0f, 0.0f, 1.0f });
    }

    simd::float4x4 makeYRotate( float angleRadians )
    {
        using simd::float4;
        const float a = angleRadians;
        return simd_matrix_from_rows((float4){ cosf( a ), 0.0f, sinf( a ), 0.0f },
                                     (float4){ 0.0f, 1.0f, 0.0f, 0.0f },
                                     (float4){ -sinf( a ), 0.0f, cosf( a ), 0.0f },
                                     (float4){ 0.0f, 0.0f, 0.0f, 1.0f });
    }

    simd::float4x4 makeZRotate( float angleRadians )
    {
        using simd::float4;
        const float a = angleRadians;
        return simd_matrix_from_rows((float4){ cosf( a ), sinf( a ), 0.0f, 0.0f },
                                     (float4){ -sinf( a ), cosf( a ), 0.0f, 0.0f },
                                     (float4){ 0.0f, 0.0f, 1.0f, 0.0f },
                                     (float4){ 0.0f, 0.0f, 0.0f, 1.0f });
    }

    simd::float4x4 makeTranslate( const simd::float3& v )
    {
        using simd::float4;
        const float4 col0 = { 1.0f, 0.0f, 0.0f, 0.0f };
        const float4 col1 = { 0.0f, 1.0f, 0.0f, 0.0f };
        const float4 col2 = { 0.0f, 0.0f, 1.0f, 0.0f };
        const float4 col3 = { v.x, v.y, v.z, 1.0f };
        return simd_matrix( col0, col1, col2, col3 );
    }

    simd::float4x4 makeScale( const simd::float3& v )
    {
        using simd::float4;
        return simd_matrix((float4){ v.x, 0, 0, 0 },
                           (float4){ 0, v.y, 0, 0 },
                           (float4){ 0, 0, v.z, 0 },
                           (float4){ 0, 0, 0, 1.0 });
    }

    simd::float3x3 discardTranslation( const simd::float4x4& m )
    {
        return simd_matrix( m.columns[0].xyz, m.columns[1].xyz, m.columns[2].xyz );
    }

}



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

    const size_t cameraDataSize = sizeof( spatial::matrix ) * 4;
    _pCameraDataBuffer = _pDevice->newBuffer( cameraDataSize, MTL::ResourceStorageModeManaged );
    
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
    if (shader.compiled()) {
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
    
    return shader.compiled(true);
}

bool implementation::metal::graphics::compile(type::program& program) {
    if (program.compiled()) {
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
    
    return program.compiled(true);
}

bool implementation::metal::graphics::compile(type::material& material) {
    if (material.compiled()) {
        return false;
    }
    if (material.color == NULL && material.normal == NULL) {
        return false;
    }

    if (material.color->resource == NULL) {
        material.color->resource = new type::info::opaque_t;
    }

/*
    const uint32_t tw = 128;
    const uint32_t th = 128;

    MTL::TextureDescriptor* pTextureDesc = MTL::TextureDescriptor::alloc()->init();
    pTextureDesc->setWidth( tw );
    pTextureDesc->setHeight( th );
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

    pTexture->replaceRegion( MTL::Region( 0, 0, 0, tw, th, 1 ), 0, pTextureData, tw * 4 );

    pTextureDesc->release();
*/

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

    pTexture->replaceRegion( MTL::Region( 0, 0, 0, tw, th, 1 ), 0, (uint8_t*)material.color->raster.data(), tw * 4 );
    
    return material.compiled(true);
}

bool implementation::metal::graphics::compile(type::object& object) {
    for (auto &child : object.children) {
        compile(child);
    }
    if (object.compiled() || object.vertices.size() == 0) {
        return false;
    }

    if (object.resource == NULL) {
        object.resource = new type::info::opaque_t;
    }
    
/*
    const float s = 0.5f;
    
    vertexData verts[] = {
        { { -s, -s, +s, 1.0 }, { 0.f, 1.f, 0.f, 0.f }, {  0.f,  0.f,  1.f, 1.f } }, // 0
        { { +s, -s, +s, 1.0 }, { 1.f, 1.f, 0.f, 0.f }, {  0.f,  0.f,  1.f, 1.f } }, // 1
        { { +s, +s, +s, 1.0 }, { 1.f, 0.f, 0.f, 0.f }, {  0.f,  0.f,  1.f, 1.f } }, // 2
        { { +s, +s, +s, 1.0 }, { 1.f, 0.f, 0.f, 0.f }, {  0.f,  0.f,  1.f, 1.f } }, // 2
        { { -s, +s, +s, 1.0 }, { 0.f, 0.f, 0.f, 0.f }, {  0.f,  0.f,  1.f, 1.f } }, // 3
        { { -s, -s, +s, 1.0 }, { 0.f, 1.f, 0.f, 0.f }, {  0.f,  0.f,  1.f, 1.f } }, // 0
        { { +s, -s, +s, 1.0 }, { 0.f, 1.f, 0.f, 0.f }, {  1.f,  0.f,  0.f, 1.f } }, // 4
        { { +s, -s, -s, 1.0 }, { 1.f, 1.f, 0.f, 0.f }, {  1.f,  0.f,  0.f, 1.f } }, // 5
        { { +s, +s, -s, 1.0 }, { 1.f, 0.f, 0.f, 0.f }, {  1.f,  0.f,  0.f, 1.f } }, // 6
        { { +s, +s, -s, 1.0 }, { 1.f, 0.f, 0.f, 0.f }, {  1.f,  0.f,  0.f, 1.f } }, // 6
        { { +s, +s, +s, 1.0 }, { 0.f, 0.f, 0.f, 0.f }, {  1.f,  0.f,  0.f, 1.f } }, // 7
        { { +s, -s, +s, 1.0 }, { 0.f, 1.f, 0.f, 0.f }, {  1.f,  0.f,  0.f, 1.f } }, // 4
        { { +s, -s, -s, 1.0 }, { 0.f, 1.f, 0.f, 0.f }, {  0.f,  0.f, -1.f, 1.f } }, // 8
        { { -s, -s, -s, 1.0 }, { 1.f, 1.f, 0.f, 0.f }, {  0.f,  0.f, -1.f, 1.f } }, // 9
        { { -s, +s, -s, 1.0 }, { 1.f, 0.f, 0.f, 0.f }, {  0.f,  0.f, -1.f, 1.f } }, // 10
        { { -s, +s, -s, 1.0 }, { 1.f, 0.f, 0.f, 0.f }, {  0.f,  0.f, -1.f, 1.f } }, // 10
        { { +s, +s, -s, 1.0 }, { 0.f, 0.f, 0.f, 0.f }, {  0.f,  0.f, -1.f, 1.f } }, // 11
        { { +s, -s, -s, 1.0 }, { 0.f, 1.f, 0.f, 0.f }, {  0.f,  0.f, -1.f, 1.f } }, // 8
        { { -s, -s, -s, 1.0 }, { 0.f, 1.f, 0.f, 0.f }, { -1.f,  0.f,  0.f, 1.f } }, // 12
        { { -s, -s, +s, 1.0 }, { 1.f, 1.f, 0.f, 0.f }, { -1.f,  0.f,  0.f, 1.f } }, // 13
        { { -s, +s, +s, 1.0 }, { 1.f, 0.f, 0.f, 0.f }, { -1.f,  0.f,  0.f, 1.f } }, // 14
        { { -s, +s, +s, 1.0 }, { 1.f, 0.f, 0.f, 0.f }, { -1.f,  0.f,  0.f, 1.f } }, // 14
        { { -s, +s, -s, 1.0 }, { 0.f, 0.f, 0.f, 0.f }, { -1.f,  0.f,  0.f, 1.f } }, // 15
        { { -s, -s, -s, 1.0 }, { 0.f, 1.f, 0.f, 0.f }, { -1.f,  0.f,  0.f, 1.f } }, // 12
        { { -s, +s, +s, 1.0 }, { 0.f, 1.f, 0.f, 0.f }, {  0.f,  1.f,  0.f, 1.f } }, // 16
        { { +s, +s, +s, 1.0 }, { 1.f, 1.f, 0.f, 0.f }, {  0.f,  1.f,  0.f, 1.f } }, // 17
        { { +s, +s, -s, 1.0 }, { 1.f, 0.f, 0.f, 0.f }, {  0.f,  1.f,  0.f, 1.f } }, // 18
        { { +s, +s, -s, 1.0 }, { 1.f, 0.f, 0.f, 0.f }, {  0.f,  1.f,  0.f, 1.f } }, // 18
        { { -s, +s, -s, 1.0 }, { 0.f, 0.f, 0.f, 0.f }, {  0.f,  1.f,  0.f, 1.f } }, // 19
        { { -s, +s, +s, 1.0 }, { 0.f, 1.f, 0.f, 0.f }, {  0.f,  1.f,  0.f, 1.f } }, // 16
        { { -s, -s, -s, 1.0 }, { 0.f, 1.f, 0.f, 0.f }, {  0.f, -1.f,  0.f, 1.f } }, // 20
        { { +s, -s, -s, 1.0 }, { 1.f, 1.f, 0.f, 0.f }, {  0.f, -1.f,  0.f, 1.f } }, // 21
        { { +s, -s, +s, 1.0 }, { 1.f, 0.f, 0.f, 0.f }, {  0.f, -1.f,  0.f, 1.f } }, // 22
        { { +s, -s, +s, 1.0 }, { 1.f, 0.f, 0.f, 0.f }, {  0.f, -1.f,  0.f, 1.f } }, // 22
        { { -s, -s, +s, 1.0 }, { 0.f, 0.f, 0.f, 0.f }, {  0.f, -1.f,  0.f, 1.f } }, // 23
        { { -s, -s, -s, 1.0 }, { 0.f, 1.f, 0.f, 0.f }, {  0.f, -1.f,  0.f, 1.f } } // 20
    };

    std::vector<spatial::vertex> sample;

    for(int i=0; i<36; i++) {
        spatial::vertex vert;
        
        vert.coordinate.x = verts[i].position.x;
        vert.coordinate.y = verts[i].position.y;
        vert.coordinate.z = verts[i].position.z;
        vert.coordinate.w = verts[i].position.w;
 
        vert.texture.x = verts[i].texcoord.x;
        vert.texture.y = verts[i].texcoord.y;
        vert.texture.z = verts[i].texcoord.z;
        vert.texture.w = verts[i].texcoord.w;

        vert.normal.x = verts[i].normal.x;
        vert.normal.y = verts[i].normal.y;
        vert.normal.z = verts[i].normal.z;
        vert.normal.w = verts[i].normal.w;
        
        sample.push_back(vert);
    }
    
    float sdata[1024];
    memcpy(sdata, (void*)sample.data(), sizeof(spatial::vertex) * sample.size());
    
    size_t size = sizeof(spatial::vertex);
    
    const size_t vertexDataSize = sizeof(spatial::vertex) * sample.size();
    
    MTL::Buffer* pVertexBuffer = _pDevice->newBuffer( vertexDataSize, MTL::ResourceStorageModeManaged );

    object.resource->ptr = (type::info::opaque_t *)pVertexBuffer;

    memcpy( pVertexBuffer->contents(), (void*)sample.data(), vertexDataSize );

    pVertexBuffer->didModifyRange( NS::Range::Make( 0, pVertexBuffer->length() ) );
    
    compile(object.texture);
*/
     
    const size_t vertexDataSize = sizeof(spatial::vertex) * object.vertices.size();
    
    MTL::Buffer* pVertexBuffer = _pDevice->newBuffer( vertexDataSize, MTL::ResourceStorageModeManaged );

    object.resource->ptr = (type::info::opaque_t *)pVertexBuffer;

    memcpy( pVertexBuffer->contents(), (void*)object.vertices.data(), vertexDataSize );

    pVertexBuffer->didModifyRange( NS::Range::Make( 0, pVertexBuffer->length() ) );
    
    compile(object.texture);
    
    return object.compiled(true);
}

bool implementation::metal::graphics::compile(type::font& font) {
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

bool implementation::metal::graphics::compile(type::entity& entity) {
    if(entity.bake() == false && entity.compiled()) {
        return false;
    }

    if(entity.object) {
        compile(*entity.object);
    }
    
    if(entity.instances.size() == 0) {
        return false;
    }

    if (entity.positions.resource == NULL) {
        entity.positions.resource = new type::info::opaque_t;
    }
    
    const size_t instanceDataSize = sizeof(spatial::matrix) * entity.positions.content.size();

    MTL::Buffer* pInstanceBuffer = _pDevice->newBuffer( instanceDataSize, MTL::ResourceStorageModeManaged );

    entity.positions.resource->ptr = (type::info::opaque_t *)pInstanceBuffer;

    memcpy( pInstanceBuffer->contents(), (void*)entity.positions.content.data(), instanceDataSize );

    pInstanceBuffer->didModifyRange( NS::Range::Make( 0, pInstanceBuffer->length() ) );
    
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

    if(_pCameraDataBuffer == NULL || object.resource == NULL || shader.resource == NULL) {
        return;
    }
    
    if(object.compiled() == false) {
        compile(object);
    }
    
    cameraData* pCameraData = reinterpret_cast< cameraData * >( _pCameraDataBuffer->contents() );
    
    pCameraData->projection = math2::makePerspective( 45.f * M_PI / 180.f, 1.f, 0.03f, 500.0f );
    pCameraData->view = math2::makeIdentity();
    pCameraData->model = math2::makeIdentity();
    pCameraData->light = math2::makeIdentity();
    
    //pCameraData->projection = projection;
    //pCameraData->view = view;
    //pCameraData->model = model;
    //pCameraData->light = lighting;
    _pCameraDataBuffer->didModifyRange( NS::Range::Make( 0, sizeof( cameraData ) ) );
    
    MTL::RenderPassDescriptor* pRpd = _pView->currentRenderPassDescriptor();
    MTL::RenderCommandEncoder* pEnc = _pCmd->renderCommandEncoder( pRpd );
    
    pEnc->setDepthStencilState( _pDepthStencilState );
    
    pEnc->setCullMode( MTL::CullModeBack );
    pEnc->setFrontFacingWinding( MTL::Winding::WindingCounterClockwise );

    auto _pPSO = reinterpret_cast<MTL::RenderPipelineState*>(shader.resource->ptr);    
    pEnc->setRenderPipelineState( _pPSO );
    
    auto _pVertexDataBuffer = reinterpret_cast<MTL::Buffer*>(object.resource->ptr);
    pEnc->setVertexBuffer( _pVertexDataBuffer, /* offset */ 0, /* index */ 0 );
    pEnc->setVertexBuffer( _pCameraDataBuffer, /* offset */ 0, /* index */ 1 );
    auto _pTexture = reinterpret_cast<MTL::Texture*>(object.texture.color->resource->ptr);
    
    pEnc->setFragmentTexture( _pTexture, /* index */ 0 );

    if(object.emitter) {
        if(object.emitter->compiled() == false) {
            compile(*object.emitter);
        }
        if(object.emitter->positions.resource && object.emitter->positions.resource->ptr) {
            auto _pInstanceDataBuffer = reinterpret_cast<MTL::Buffer*>(object.emitter->positions.resource->ptr);
            pEnc->setVertexBuffer( _pInstanceDataBuffer, /* offset */ 0, /* index */ 2 );
        }
        pEnc->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, 0, object.vertices.size(), object.emitter->positions.content.size());
    }
    else {
        pEnc->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, 0, object.vertices.size(), 1);
    }
    
    //pEnc->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, 0, object.vertices.size(), kNumInstances );

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


