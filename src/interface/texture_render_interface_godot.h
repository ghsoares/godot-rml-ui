#pragma once
#include <RmlUi/Core/RenderInterface.h>
#include <godot_cpp/classes/rendering_server.hpp>
#include <vector>

#include "render_interface_godot.h"
#include "../rendering/rendering_resources.h"

namespace godot {

class TextureRenderInterfaceGodot: public RenderInterfaceGodot {
private:
    struct MeshData {
        RID position_buffer;
        RID color_buffer;
        RID uv_buffer;
        RID index_buffer;
        RID vertex_array;
        RID index_array;
    };

    struct TextureData {
        RID rid;
        Ref<Texture> tex_ref;
    };

    enum BufferTarget {
        NONE,
        PRIMARY_BUFFER0, PRIMARY_BUFFER1,
        SECONDARY_BUFFER0, SECONDARY_BUFFER1,
        BLEND_BUFFER0, BLEND_BUFFER1
    };

    struct ShaderRenderData {
        String name;

        struct Pass {
            RID shader;
            uint64_t pipeline_id;
            RID buffer;
            PackedByteArray push_const;

            BufferTarget src0 = BufferTarget::PRIMARY_BUFFER0;
            BufferTarget src1 = BufferTarget::NONE;
            BufferTarget dst = BufferTarget::PRIMARY_BUFFER1;

            BufferTarget swap0 = BufferTarget::PRIMARY_BUFFER0;
            BufferTarget swap1 = BufferTarget::PRIMARY_BUFFER1;
        };

        std::vector<Pass> passes;
    };

    struct RenderTarget {
        RID color0, color1;
        RID framebuffer0, framebuffer1;

        void swap() {
            SWAP(color0, color1);
            SWAP(framebuffer0, framebuffer1);
        }
    };

    struct RenderFrame {
        RenderTarget main_target;
        RenderTarget primary_filter_target;
        RenderTarget secondary_filter_target;
        RenderTarget blend_mask_target;
        RID main_tex;

        RID clip_mask;
        RID clip_mask_framebuffer;

        Vector2i current_size;
        Vector2i desired_size;

        bool is_valid() { return main_tex.is_valid(); }

        RID get_texture() { return main_tex; }
    };

    RenderFrame *target_frame;
    std::vector<RenderTarget *> render_target_stack;
    uintptr_t render_target_stack_ptr = 0;

    RenderingResources internal_rendering_resources;
    RenderingResources rendering_resources;

    int64_t color_framebuffer_format;
    int64_t geometry_framebuffer_format;
    int64_t clip_mask_framebuffer_format;
    int64_t geometry_vertex_format;

    RID shader_blit;
    RID shader_geometry;
    RID shader_clip_mask;
    RID shader_layer_composition;

    RID pipeline_blit;
    RID pipeline_geometry;
    RID pipeline_geometry_clipping;
    RID pipeline_clip_mask_set;
    RID pipeline_clip_mask_set_inverse;
    RID pipeline_clip_mask_intersect;
    RID pipeline_layer_composition;

    std::map<uint64_t, RID> shaders;
    std::map<uint64_t, std::tuple<RID, RID>> pipelines;

    RID sampler_nearest;
    RID sampler_linear;

    RID texture_white;

    bool scissor_enabled = false;
    Rect2 scissor_region = Rect2();
    bool clip_mask_enabled = false;

    Rml::Matrix4f drawing_matrix = Rml::Matrix4f::Identity();

    Rml::Matrix4f get_final_transform(const Rml::Matrix4f &p_drawing_matrix, const Rml::Vector2f &translation);
    void matrix_to_pointer(float *ptr, const Rml::Matrix4f &p_mat);
    RID *filter_get_texture(RenderFrame *frame, const BufferTarget &target);
    RID *filter_get_framebuffer(RenderFrame *frame, const BufferTarget &target);

    void allocate_render_target(RenderTarget *p_target);
    void free_render_target(RenderTarget *p_target);

    void allocate_render_frame(RenderFrame *p_frame);
    void free_render_frame(RenderFrame *p_frame);

    void blit_texture(const RID &p_dst, const RID &p_src, const Vector2i &p_dst_pos, const Vector2i &p_src_pos, const Vector2i &p_size);

    void create_shader_pipeline(uint64_t p_id, const std::map<String, Variant> &p_params);
    RID get_shader_pipeline(uint64_t p_id) const;
public:
    void initialize() override;
    void finalize() override;

    void push_context(void *&p_ctx, const Vector2i &p_size) override;
    void pop_context() override;
    void draw_context(void *&p_ctx, const RID &p_canvas_item) override;
	void free_context(void *&p_ctx) override;

	Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices) override;
	void RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation, Rml::TextureHandle texture) override;
	void ReleaseGeometry(Rml::CompiledGeometryHandle geometry) override;

    Rml::TextureHandle LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source) override;
    Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions) override;
    void ReleaseTexture(Rml::TextureHandle texture) override;

    void EnableScissorRegion(bool enable) override;
    void SetScissorRegion(Rml::Rectanglei region) override;

    void EnableClipMask(bool enable) override;
    void RenderToClipMask(Rml::ClipMaskOperation operation, Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation) override;

    void SetTransform(const Rml::Matrix4f* transform) override;

    Rml::LayerHandle PushLayer() override;
    void RenderFilter(Rml::CompiledFilterHandle filter);
    void CompositeLayers(Rml::LayerHandle source, Rml::LayerHandle destination, Rml::BlendMode blend_mode, Rml::Span<const Rml::CompiledFilterHandle> filters) override;
    void PopLayer() override;

    Rml::TextureHandle SaveLayerAsTexture() override;
    Rml::CompiledFilterHandle SaveLayerAsMaskImage() override;

    Rml::CompiledFilterHandle CompileFilter(const Rml::String& name, const Rml::Dictionary& parameters) override;
    void ReleaseFilter(Rml::CompiledFilterHandle filter) override;

    Rml::CompiledShaderHandle CompileShader(const Rml::String& name, const Rml::Dictionary& parameters) override;
    void RenderShader(Rml::CompiledShaderHandle shader, Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation, Rml::TextureHandle texture) override;
    void ReleaseShader(Rml::CompiledShaderHandle shader) override;
};


}