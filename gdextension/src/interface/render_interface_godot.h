#pragma once
#include <RmlUi/Core/RenderInterface.h>
#include <godot_cpp/classes/rendering_server.hpp>

#include "../rendering/context_render_state.h"

namespace godot {

class RMLServer;

class RenderInterfaceGodot: public Rml::RenderInterface {

    friend class RMLServer;
private:
    static RenderInterfaceGodot *singleton;

    RID render_target;
    std::map<String, RID> shaders;

    RID linear_sampler;
    RID white_texture;

    bool scissor_enabled = false;
    Rect2 scissor_region = Rect2();

    Projection drawing_matrix;
public:
    static RenderInterfaceGodot *get_singleton();

    void initialize();
    void uninitialize();
    
    void allocate_render_target(RID &p_rid, RID &p_rid_rd, const Vector2i &p_size);
    void set_render_target(const RID &p_rid);
    void free_render_target(const RID &p_rid, const RID &p_rid_rd);

    RID load_shader_from_path(const String &p_name, const String &p_path);
    RID get_shader(const String &p_name);
    void unload_shader(const String &p_name);

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