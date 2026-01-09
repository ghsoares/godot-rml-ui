#include "render_interface_godot.h"
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/texture.hpp>
#include <godot_cpp/classes/rendering_device.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/uniform_set_cache_rd.hpp>
#include <godot_cpp/classes/rd_uniform.hpp>
#include <godot_cpp/classes/rd_shader_source.hpp>
#include <godot_cpp/classes/rd_shader_file.hpp>
#include <godot_cpp/classes/rd_shader_spirv.hpp>
#include <godot_cpp/classes/rd_sampler_state.hpp>
#include <godot_cpp/classes/rd_texture_format.hpp>
#include <godot_cpp/classes/rd_attachment_format.hpp>
#include <godot_cpp/classes/rd_texture_view.hpp>
#include <godot_cpp/classes/rd_vertex_attribute.hpp>
#include <godot_cpp/classes/rd_pipeline_rasterization_state.hpp>
#include <godot_cpp/classes/rd_pipeline_multisample_state.hpp>
#include <godot_cpp/classes/rd_pipeline_depth_stencil_state.hpp>
#include <godot_cpp/classes/rd_pipeline_color_blend_state.hpp>
#include <godot_cpp/classes/rd_pipeline_color_blend_state_attachment.hpp>

#include <iostream>

#include "../util.h"

using namespace godot;

RenderInterfaceGodot *RenderInterfaceGodot::singleton = nullptr;

RenderInterfaceGodot *RenderInterfaceGodot::get_singleton() {
    return singleton;
}

void RenderInterfaceGodot::initialize() {
    RenderingServer *rs = RenderingServer::get_singleton();
    RenderingDevice *rd = rs->get_rendering_device();

    load_shader_from_path("__render_interface_geometry_shader", "res://addons/rmlui/shaders/geometry.glsl");

    Ref<RDSamplerState> sampler_state;
    sampler_state.instantiate();

    sampler_state->set_min_filter(RenderingDevice::SAMPLER_FILTER_LINEAR);
    sampler_state->set_mag_filter(RenderingDevice::SAMPLER_FILTER_LINEAR);
    sampler_state->set_repeat_u(RenderingDevice::SAMPLER_REPEAT_MODE_CLAMP_TO_EDGE);
    sampler_state->set_repeat_v(RenderingDevice::SAMPLER_REPEAT_MODE_CLAMP_TO_EDGE);
    sampler_state->set_repeat_w(RenderingDevice::SAMPLER_REPEAT_MODE_CLAMP_TO_EDGE);

    linear_sampler = rd->sampler_create(sampler_state);

    Ref<RDTextureFormat> tex_format;
    Ref<RDTextureView> tex_view;
    tex_format.instantiate();
    tex_view.instantiate();
    tex_format->set_texture_type(RenderingDevice::TEXTURE_TYPE_2D);
    tex_format->set_width(4);
    tex_format->set_height(4);
    tex_format->set_format(RenderingDevice::DATA_FORMAT_R8G8B8A8_UNORM);
    tex_format->set_usage_bits(RenderingDevice::TEXTURE_USAGE_SAMPLING_BIT);

    PackedByteArray white_texture_buf;
    white_texture_buf.resize(4 * 4 * 4); // width * height * rgba
    white_texture_buf.fill(255);

    white_texture = rd->texture_create(tex_format, tex_view, { white_texture_buf });
}

void RenderInterfaceGodot::uninitialize() {
    RenderingServer *rs = RenderingServer::get_singleton();
    RenderingDevice *rd = rs->get_rendering_device();

    unload_shader("__render_interface_geometry_shader");

    rd->free_rid(linear_sampler);
    rd->free_rid(white_texture);
}

RID RenderInterfaceGodot::load_shader_from_path(const String &p_name, const String &p_path) {
    RenderingServer *rs = RenderingServer::get_singleton();
    RenderingDevice *rd = rs->get_rendering_device();

    Ref<RDShaderFile> shader_file = ResourceLoader::get_singleton()->load(p_path);
    ERR_FAIL_COND_V(!shader_file.is_valid(), RID());

    Ref<RDShaderSPIRV> shader_spirv = shader_file->get_spirv();
    ERR_FAIL_COND_V(!shader_spirv.is_valid(), RID());

    RID shader = rd->shader_create_from_spirv(shader_spirv, p_name);
    ERR_FAIL_COND_V(!shader.is_valid(), RID());

    shaders.insert({p_name, shader});

    return shader;
}

RID RenderInterfaceGodot::get_shader(const String &p_name) {
    auto it = shaders.find(p_name);
    if (it == shaders.end()) {
        ERR_FAIL_V_MSG(RID(), vformat("Shader of name '%s' not found", p_name));
    }
    return it->second;
}

void RenderInterfaceGodot::unload_shader(const String &p_name) {
    RID shader = get_shader(p_name);
    ERR_FAIL_COND(!shader.is_valid());

    RenderingServer *rs = RenderingServer::get_singleton();
    RenderingDevice *rd = rs->get_rendering_device();
    
    rd->free_rid(shader);
}

void RenderInterfaceGodot::allocate_render_target(RID &p_rid, RID &p_rid_rd, const Vector2i &p_size) {
    RenderingServer *rs = RenderingServer::get_singleton();
    RenderingDevice *rd = rs->get_rendering_device();

    if (p_rid.is_valid()) {
        rd->free_rid(p_rid);
        rs->free_rid(p_rid_rd);
    }

    Ref<RDTextureFormat> tex_format;
    Ref<RDTextureView> tex_view;
    tex_format.instantiate();
    tex_view.instantiate();

    tex_format->set_texture_type(RenderingDevice::TEXTURE_TYPE_2D);
    tex_format->set_width(Math::max(p_size.x, 1));
    tex_format->set_height(Math::max(p_size.y, 1));
    tex_format->set_format(RenderingDevice::DATA_FORMAT_R8G8B8A8_UNORM);
    tex_format->set_usage_bits(RenderingDevice::TEXTURE_USAGE_COLOR_ATTACHMENT_BIT | RenderingDevice::TEXTURE_USAGE_SAMPLING_BIT | RenderingDevice::TEXTURE_USAGE_CAN_COPY_TO_BIT);

    p_rid = rd->texture_create(tex_format, tex_view);
    p_rid_rd = rs->texture_rd_create(p_rid);
}

void RenderInterfaceGodot::set_render_target(const RID &p_rid) {
    render_target = p_rid;
    if (!render_target.is_valid()) return;

    RenderingServer *rs = RenderingServer::get_singleton();
    RenderingDevice *rd = rs->get_rendering_device();

    rd->texture_clear(p_rid, Color(0, 0, 0, 0), 0, 1, 0, 1);
}

void RenderInterfaceGodot::free_render_target(const RID &p_rid, const RID &p_rid_rd) {
    RenderingServer *rs = RenderingServer::get_singleton();
    RenderingDevice *rd = rs->get_rendering_device();

    rd->free_rid(p_rid);
    rs->free_rid(p_rid_rd);
}

Rml::CompiledGeometryHandle RenderInterfaceGodot::CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices) {
    MeshData *mesh_data = memnew(MeshData);

    RenderingServer *rs = RenderingServer::get_singleton();
    RenderingDevice *rd = rs->get_rendering_device();

    Ref<RDVertexAttribute> pos_attr, color_attr, uv_attr;
    pos_attr.instantiate();
    color_attr.instantiate();
    uv_attr.instantiate();

    pos_attr->set_format(RenderingDevice::DATA_FORMAT_R32G32_SFLOAT);
    pos_attr->set_location(0);
    pos_attr->set_stride(4 * 2); // 4 bytes * 2 members (xy)

    color_attr->set_format(RenderingDevice::DATA_FORMAT_R32G32B32A32_SFLOAT);
    color_attr->set_location(1);
    color_attr->set_stride(4 * 4); // 4 bytes * 4 members (rgba)

    uv_attr->set_format(RenderingDevice::DATA_FORMAT_R32G32_SFLOAT);
    uv_attr->set_location(2);
    uv_attr->set_stride(4 * 2); // 4 bytes * 2 members (xy)

    PackedByteArray position_array, color_array, uv_array, indices_array;
    position_array.resize(vertices.size() * 2 * 4);
    color_array.resize(vertices.size() * 4 * 4);
    uv_array.resize(vertices.size() * 2 * 4);
    indices_array.resize(indices.size() * 4);

    float *position_ptr = (float *)(position_array.ptrw());
    float *color_ptr = (float *)(color_array.ptrw());
    float *uv_ptr = (float *)(uv_array.ptrw());
    int *index_ptr = (int *)(indices_array.ptrw());

    int vert_count = vertices.size();
    int index_count = indices.size();

    for (int i = vert_count - 1; i >= 0; i--) {
        const Rml::Vertex &v = vertices[i];
        position_ptr[i * 2 + 0] = v.position.x;
        position_ptr[i * 2 + 1] = v.position.y;
        color_ptr[i * 4 + 0] = v.colour.red / 255.0;
        color_ptr[i * 4 + 1] = v.colour.green / 255.0;
        color_ptr[i * 4 + 2] = v.colour.blue / 255.0;
        color_ptr[i * 4 + 3] = v.colour.alpha / 255.0;
        uv_ptr[i * 2 + 0] = v.tex_coord.x;
        uv_ptr[i * 2 + 1] = v.tex_coord.y;
    }

    for (int i = index_count - 1; i >= 0; i--) {
        index_ptr[i] = indices[i];
    }

    RID position_buffer = rd->vertex_buffer_create(position_array.size(), position_array);
    RID color_buffer = rd->vertex_buffer_create(color_array.size(), color_array);
    RID uv_buffer = rd->vertex_buffer_create(uv_array.size(), uv_array);
    RID index_buffer = rd->index_buffer_create(indices.size(), RenderingDevice::INDEX_BUFFER_FORMAT_UINT32, indices_array);

    TypedArray<Ref<RDVertexAttribute>> vertex_attributes;
    int64_t vertex_format = rd->vertex_format_create({ pos_attr, color_attr, uv_attr });

    RID vertex_array = rd->vertex_array_create(vertices.size(), vertex_format, { position_buffer, color_buffer, uv_buffer });
    RID index_array = rd->index_array_create(index_buffer, 0, indices.size());

    mesh_data->position_buffer = position_buffer;
    mesh_data->color_buffer = color_buffer;
    mesh_data->uv_buffer = uv_buffer;
    mesh_data->index_buffer = index_buffer;
    mesh_data->vertex_array = vertex_array;
    mesh_data->index_array = index_array;
    mesh_data->vertex_format = vertex_format;

    return reinterpret_cast<uintptr_t>(mesh_data);
}

void RenderInterfaceGodot::RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation, Rml::TextureHandle texture) {
    ERR_FAIL_COND(!render_target.is_valid());

    MeshData *mesh_data = reinterpret_cast<MeshData *>(geometry);
    ERR_FAIL_NULL(mesh_data);

    RID geometry_shader = get_shader("__render_interface_geometry_shader");
    ERR_FAIL_COND(!geometry_shader.is_valid());
    
    RenderingServer *rs = RenderingServer::get_singleton();
    RenderingDevice *rd = rs->get_rendering_device();

    Ref<RDTextureFormat> tex_format = rd->texture_get_format(render_target);

    RID render_target_fb = rd->framebuffer_create({ render_target });
    int render_target_format = rd->framebuffer_get_format(render_target_fb);

    Ref<RDPipelineRasterizationState> rasterization_state;
    Ref<RDPipelineMultisampleState> multisample_state;
    Ref<RDPipelineDepthStencilState> depth_stencil_state;
    Ref<RDPipelineColorBlendState> color_blend_state;
    Ref<RDPipelineColorBlendStateAttachment> color_attachment;
    rasterization_state.instantiate();
    multisample_state.instantiate();
    depth_stencil_state.instantiate();
    color_blend_state.instantiate();
    color_attachment.instantiate();
    color_attachment->set_as_mix();
    color_blend_state->set_attachments({ color_attachment });

    RID pipeline = rd->render_pipeline_create(
        geometry_shader, 
        render_target_format, 
        mesh_data->vertex_format,
        RenderingDevice::RENDER_PRIMITIVE_TRIANGLES,
        rasterization_state,
        multisample_state,
        depth_stencil_state,
        color_blend_state
    );

    Ref<RDUniform> texture_uniform;
    texture_uniform.instantiate();

    texture_uniform->set_uniform_type(RenderingDevice::UNIFORM_TYPE_SAMPLER_WITH_TEXTURE);
    texture_uniform->set_binding(0);
    texture_uniform->add_id(linear_sampler);
    if (texture == 0) {
        texture_uniform->add_id(white_texture);
    } else {
        TextureData *tex_data = reinterpret_cast<TextureData *>(texture);
        texture_uniform->add_id(tex_data->rid_rd);
    }

    RID uniform_set = UniformSetCacheRD::get_cache(geometry_shader, 0, { texture_uniform });

    Projection final_transform = drawing_matrix * Projection(Transform3D(Basis(), Vector3(translation.x, translation.y, 0.0)));

    PackedByteArray push_const;
    push_const.resize(80);
    float *push_const_ptr = (float *)push_const.ptrw();
    push_const_ptr[0] = 1.0 / tex_format->get_width();
    push_const_ptr[1] = 1.0 / tex_format->get_height();

    push_const_ptr[4] = final_transform.columns[0].x;
    push_const_ptr[5] = final_transform.columns[0].y;
    push_const_ptr[6] = final_transform.columns[0].z;
    push_const_ptr[7] = final_transform.columns[0].w;

    push_const_ptr[8] = final_transform.columns[1].x;
    push_const_ptr[9] = final_transform.columns[1].y;
    push_const_ptr[10] = final_transform.columns[1].z;
    push_const_ptr[11] = final_transform.columns[1].w;

    push_const_ptr[12] = final_transform.columns[2].x;
    push_const_ptr[13] = final_transform.columns[2].y;
    push_const_ptr[14] = final_transform.columns[2].z;
    push_const_ptr[15] = final_transform.columns[2].w;

    push_const_ptr[16] = final_transform.columns[3].x;
    push_const_ptr[17] = final_transform.columns[3].y;
    push_const_ptr[18] = final_transform.columns[3].z;
    push_const_ptr[19] = final_transform.columns[3].w;

    int draw_list = rd->draw_list_begin(render_target_fb, RenderingDevice::DRAW_IGNORE_ALL);
    if (scissor_enabled) {
        rd->draw_list_enable_scissor(draw_list, scissor_region);
    } else {
        RenderingServer *rs = RenderingServer::get_singleton();
        RenderingDevice *rd = rs->get_rendering_device();
        rd->draw_list_disable_scissor(draw_list);
    }
    rd->draw_list_bind_render_pipeline(draw_list, pipeline);
    rd->draw_list_bind_vertex_array(draw_list, mesh_data->vertex_array);
    rd->draw_list_bind_index_array(draw_list, mesh_data->index_array);
    rd->draw_list_bind_uniform_set(draw_list, uniform_set, 0);
    rd->draw_list_set_push_constant(draw_list, push_const, push_const.size());

    rd->draw_list_draw(draw_list, true, 1);
    rd->draw_list_end();

    rd->free_rid(pipeline);
    rd->free_rid(render_target_fb);
}

void RenderInterfaceGodot::ReleaseGeometry(Rml::CompiledGeometryHandle geometry) {
    MeshData *mesh_data = reinterpret_cast<MeshData *>(geometry);
    ERR_FAIL_NULL(mesh_data);

    RenderingServer *rs = RenderingServer::get_singleton();
    RenderingDevice *rd = rs->get_rendering_device();
    
    rd->free_rid(mesh_data->vertex_array);
    rd->free_rid(mesh_data->index_array);
    rd->free_rid(mesh_data->position_buffer);
    rd->free_rid(mesh_data->color_buffer);
    rd->free_rid(mesh_data->uv_buffer);
    rd->free_rid(mesh_data->index_buffer);

    memdelete(mesh_data);
}

Rml::TextureHandle RenderInterfaceGodot::LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source) {
    ResourceLoader *rl = ResourceLoader::get_singleton();

    String source_str = String(source.c_str());

    Ref<Texture> tex = rl->load(source_str);
    if (!tex.is_valid()) {
        return 0;
    }

    RenderingServer *rs = RenderingServer::get_singleton();

    TextureData *tex_data = memnew(TextureData());
    tex_data->tex_ref = tex;
    tex_data->rid = tex->get_rid();
    tex_data->rid_rd = rs->texture_get_rd_texture(tex_data->rid);

    return reinterpret_cast<uintptr_t>(tex_data);
}

Rml::TextureHandle RenderInterfaceGodot::GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions) {
    PackedByteArray p_data;
    p_data.resize(source.size());

    uint8_t *ptrw = p_data.ptrw();

    memcpy(ptrw, source.data(), source.size());

    Ref<Image> img = Image::create_from_data(source_dimensions.x, source_dimensions.y, false, Image::FORMAT_RGBA8, p_data);
    if (!img.is_valid()) {
        return 0;
    }

    RenderingServer *rs = RenderingServer::get_singleton();
    RID tex_rid = rs->texture_2d_create(img);

    TextureData *tex_data = memnew(TextureData());
    tex_data->rid = tex_rid;
    tex_data->rid_rd = rs->texture_get_rd_texture(tex_data->rid);

    return reinterpret_cast<uintptr_t>(tex_data);
}

void RenderInterfaceGodot::ReleaseTexture(Rml::TextureHandle texture) {
    TextureData *tex_data = reinterpret_cast<TextureData *>(texture);

    // Is a generated texture
    if (!tex_data->tex_ref.is_valid()) {
        RenderingServer *rs = RenderingServer::get_singleton();
        
        rs->free_rid(tex_data->rid);
    }
    memdelete(tex_data);
}

void RenderInterfaceGodot::EnableScissorRegion(bool enable) {
    scissor_enabled = enable;
}

void RenderInterfaceGodot::SetScissorRegion(Rml::Rectanglei region) {
    scissor_region = Rect2(
        region.Position().x,
        region.Position().y,
        region.Size().x,
        region.Size().y
    );
}

void RenderInterfaceGodot::SetTransform(const Rml::Matrix4f* transform) {
    if (transform == nullptr) {
        drawing_matrix = Projection();
        return;
    }
    Rml::Matrix4f mat = *transform;
    drawing_matrix[0] = Vector4(mat[0].x, mat[0].y, mat[0].z, mat[0].w);
    drawing_matrix[1] = Vector4(mat[1].x, mat[1].y, mat[1].z, mat[1].w);
    drawing_matrix[2] = Vector4(mat[2].x, mat[2].y, mat[2].z, mat[2].w);
    drawing_matrix[3] = Vector4(mat[3].x, mat[3].y, mat[3].z, mat[3].w);
}

RenderInterfaceGodot::RenderInterfaceGodot() {
    singleton = this;
}
RenderInterfaceGodot::~RenderInterfaceGodot() {
    singleton = nullptr;
}
