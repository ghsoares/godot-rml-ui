#pragma once
#include <RmlUi/Core/RenderInterface.h>
#include <godot_cpp/classes/rendering_server.hpp>
#include <deque>

#include "../rendering/rendering_utils.h"

namespace godot {

class RMLServer;

class RenderInterfaceGodot: public Rml::RenderInterface {

    friend class RMLServer;
private:
    static RenderInterfaceGodot *singleton;

    std::deque<RenderTarget *> render_target_stack;
    RenderingResources internal_rendering_resources;
    RenderingResources rendering_resources;

    int64_t color_and_alpha_framebuffer_format;
    int64_t geometry_vertex_format;

    RID shader_blit;
    RID shader_geometry;

    RID pipeline_blit;
    RID pipeline_geometry;

    RID sampler_nearest;
    RID sampler_linear;

    RID texture_white;

    bool scissor_enabled = false;
    Rect2 scissor_region = Rect2();

    Projection drawing_matrix;

    void allocate_render_target(RenderTarget *p_target);
    void blit_render_target(RenderTarget *p_target);
public:
    static RenderInterfaceGodot *get_singleton();

    void initialize();
    void uninitialize();
    
    void set_render_target(RenderTarget *p_target);
    void free_render_target(RenderTarget *p_target);

	Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices) override;
	void RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation, Rml::TextureHandle texture) override;
	void ReleaseGeometry(Rml::CompiledGeometryHandle geometry) override;

    Rml::TextureHandle LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source) override;
    Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions) override;
    void ReleaseTexture(Rml::TextureHandle texture) override;

    void EnableScissorRegion(bool enable) override;
    void SetScissorRegion(Rml::Rectanglei region) override;

    void SetTransform(const Rml::Matrix4f* transform) override;

    RenderInterfaceGodot();
    ~RenderInterfaceGodot();
};

}