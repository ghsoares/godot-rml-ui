#include <iostream>
#include <sstream>

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/classes/input_event_mouse_motion.hpp>
#include "../interface/render_interface_godot.h"
#include "../plugin/rml_godot_plugin.h"
#include "../rml_util.h"
#include "rml_server.h"

using namespace godot;

RMLServer *RMLServer::singleton = nullptr;

RMLServer *RMLServer::get_singleton() {
	return singleton;
}

RID RMLServer::initialize_document(const RID &p_canvas_item) {
	RID new_rid = document_owner.make_rid();
	std::stringstream context_str;
	context_str << "godot_context_" << new_rid.get_id();
	
	DocumentData *doc_data = document_owner.get_or_null(new_rid);
	doc_data->ctx = Rml::CreateContext(
		context_str.str(),
		Rml::Vector2i(1, 1)
	);

	if (doc_data->ctx == nullptr) {
		document_owner.free(new_rid);
		ERR_FAIL_V_MSG(RID(), "Couldn't create the context");
	}

	return new_rid;
}

RID RMLServer::create_document(const RID &p_canvas_item) {
	RID new_rid = initialize_document(p_canvas_item);
	DocumentData *doc_data = document_owner.get_or_null(new_rid);

	doc_data->doc = doc_data->ctx->CreateDocument();
	if (doc_data->doc == nullptr) {
		Rml::RemoveContext(doc_data->ctx->GetName());
		document_owner.free(new_rid);
		ERR_FAIL_V_MSG(RID(), "Couldn't create the document");
		return RID();
	}

	doc_data->doc->Show();

	doc_data->canvas_item = p_canvas_item;

	return new_rid;
}

RID RMLServer::create_document_from_rml_string(const RID &p_canvas_item, const String &p_string) {
	RID new_rid = initialize_document(p_canvas_item);
	DocumentData *doc_data = document_owner.get_or_null(new_rid);

	doc_data->doc = doc_data->ctx->LoadDocumentFromMemory(godot_to_rml_string(p_string));
	if (doc_data->doc == nullptr) {
		Rml::RemoveContext(doc_data->ctx->GetName());
		document_owner.free(new_rid);
		ERR_FAIL_V_MSG(RID(), "Couldn't create the document");
		return RID();
	}

	doc_data->doc->Show();

	doc_data->canvas_item = p_canvas_item;

	return new_rid;
}

RID RMLServer::create_document_from_path(const RID &p_canvas_item, const String &p_path) {
	RID new_rid = initialize_document(p_canvas_item);
	DocumentData *doc_data = document_owner.get_or_null(new_rid);

	doc_data->doc = doc_data->ctx->LoadDocument(godot_to_rml_string(p_path));
	if (doc_data->doc == nullptr) {
		Rml::RemoveContext(doc_data->ctx->GetName());
		document_owner.free(new_rid);
		ERR_FAIL_V_MSG(RID(), "Couldn't create the document");
		return RID();
	}

	doc_data->doc->Show();

	doc_data->canvas_item = p_canvas_item;

	return new_rid;
}

Ref<RMLElement> RMLServer::get_document_root(const RID &p_document) {
	ERR_FAIL_COND_V(!document_owner.owns(p_document), nullptr);
	DocumentData *doc_data = document_owner.get_or_null(p_document);
	ERR_FAIL_NULL_V(doc_data, nullptr);

	ElementRef ref = ElementRef(doc_data->doc);
	return RMLElement::ref(ref);
}

Ref<RMLElement> RMLServer::create_element(const RID &p_document, const String &p_tag_name) {
	ERR_FAIL_COND_V(!document_owner.owns(p_document), nullptr);
	DocumentData *doc_data = document_owner.get_or_null(p_document);
	ERR_FAIL_NULL_V(doc_data, nullptr);

	Rml::String tag_name = godot_to_rml_string(p_tag_name);

	ElementRef ref = ElementRef(doc_data->doc->CreateElement(tag_name));
	return RMLElement::ref(ref);
}

void RMLServer::document_set_size(const RID &p_document, const Vector2i &p_size) {
	ERR_FAIL_COND(!document_owner.owns(p_document));
	DocumentData *doc_data = document_owner.get_or_null(p_document);
	ERR_FAIL_NULL(doc_data);
	doc_data->ctx->SetDimensions(Rml::Vector2i(
		p_size.x,
		p_size.y
	));
}

bool RMLServer::document_process_event(const RID &p_document, const Ref<InputEvent> &p_event) {
	ERR_FAIL_COND_V(!document_owner.owns(p_document), false);
	DocumentData *doc_data = document_owner.get_or_null(p_document);
	ERR_FAIL_NULL_V(doc_data, false);
	
	Ref<InputEventMouseButton> mb = p_event;
	Ref<InputEventMouseMotion> mm = p_event;

	if (mb.is_valid()) {
		if (mb->is_pressed()) {
			return !doc_data->ctx->ProcessMouseButtonDown(
				mb->get_button_index() - 1,
				0
			);
		} else {
			return !doc_data->ctx->ProcessMouseButtonUp(
				mb->get_button_index() - 1,
				0
			);
		}
	}

	if (mm.is_valid()) {
		return !doc_data->ctx->ProcessMouseMove(
			mm->get_position().x,
            mm->get_position().y,
            0
		);
	}

	return false;
}

bool RMLServer::load_default_stylesheet(const String &p_path) {
	Rml::SharedPtr<Rml::StyleSheetContainer> ss = Rml::Factory::InstanceStyleSheetFile(godot_to_rml_string(p_path));
	ERR_FAIL_NULL_V(ss, false);

	RmlPluginGodot::get_singleton()->set_default_stylesheet(ss);

	return true;
}

bool RMLServer::load_font_face_from_path(const String &p_path, bool p_fallback_face) {
	return Rml::LoadFontFace(godot_to_rml_string(p_path), p_fallback_face);
}

bool RMLServer::load_font_face_from_buffer(const PackedByteArray &p_buffer, const String &p_family, bool p_fallback_face, bool p_is_italic) {
	return Rml::LoadFontFace(
		Rml::Span(p_buffer.ptr(), p_buffer.size()), 
		godot_to_rml_string(p_family), 
		p_is_italic ? Rml::Style::FontStyle::Italic : Rml::Style::FontStyle::Normal,
		Rml::Style::FontWeight::Auto,
		p_fallback_face
	);
}

void RMLServer::free_rid(const RID &p_rid) {
	if (document_owner.owns(p_rid)) {
		DocumentData *doc_data = document_owner.get_or_null(p_rid);
		Rml::RemoveContext(doc_data->ctx->GetName());
		document_owner.free(p_rid);
	}
}

void RMLServer::render_pre_frame() {
	RenderingServer *rs = RenderingServer::get_singleton();
	RenderInterfaceGodot *ri = RenderInterfaceGodot::get_singleton();

	List<RID> contexts;
	document_owner.get_owned_list(&contexts);
	for (RID doc_rid : contexts) {
		DocumentData *doc = document_owner.get_or_null(doc_rid);
		doc->ctx->Update();
		rs->canvas_item_clear(doc->canvas_item);
		ri->set_drawing_canvas_item(doc->canvas_item);
		doc->ctx->Render();
		ri->set_drawing_canvas_item(RID());
	}
}

void RMLServer::initialize() {
	ERR_FAIL_COND_MSG(initialized, "Already initialized");
	RenderingServer::get_singleton()->connect("frame_pre_draw", callable_mp(this, &RMLServer::render_pre_frame));
	initialized = true;
	Rml::Log::Message(Rml::Log::LT_INFO, "RMLServer initialized.");
}

void RMLServer::_bind_methods() {;
	ClassDB::bind_method(D_METHOD("is_initialized"), &RMLServer::is_initialized);
	ClassDB::bind_method(D_METHOD("initialize"), &RMLServer::initialize);

	ClassDB::bind_method(D_METHOD("create_document", "canvas_item"), &RMLServer::create_document);
	ClassDB::bind_method(D_METHOD("create_document_from_rml_string", "canvas_item", "rml"), &RMLServer::create_document_from_rml_string);
	ClassDB::bind_method(D_METHOD("create_document_from_path", "canvas_item", "path"), &RMLServer::create_document_from_path);
	ClassDB::bind_method(D_METHOD("get_document_root", "document"), &RMLServer::get_document_root);
	ClassDB::bind_method(D_METHOD("create_element", "document", "tag_name"), &RMLServer::create_element);
	
	ClassDB::bind_method(D_METHOD("document_set_size", "document", "size"), &RMLServer::document_set_size);
	ClassDB::bind_method(D_METHOD("document_process_event", "document", "event"), &RMLServer::document_process_event);

	ClassDB::bind_method(D_METHOD("load_default_stylesheet", "path"), &RMLServer::load_default_stylesheet);
	ClassDB::bind_method(D_METHOD("load_font_face_from_path", "path", "fallback_face"), &RMLServer::load_font_face_from_path, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("load_font_face_from_buffer", "buffer", "family", "fallback_face", "is_italic"), &RMLServer::load_font_face_from_buffer, DEFVAL(false), DEFVAL(false));

	ClassDB::bind_method(D_METHOD("free_rid", "rid"), &RMLServer::free_rid);
}

RMLServer::RMLServer() {
	singleton = this;
}

