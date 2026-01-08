#include <iostream>
#include <sstream>

#include "../server/rml_server.h"
#include "rml_element.h"
#include "rml_document.h"

using namespace godot;

void RMLDocument::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			RMLServer::get_singleton()->document_set_size(rid, Vector2i(
				get_size().x,
				get_size().y
			));
		} break;
		case NOTIFICATION_RESIZED: {
			RMLServer::get_singleton()->document_set_size(rid, Vector2i(
				get_size().x,
				get_size().y
			));
		} break;
	}
}

void RMLDocument::_gui_input(const Ref<InputEvent> &p_event) {
	if (RMLServer::get_singleton()->document_process_event(rid, p_event)) {
		accept_event();
	}
}

Ref<RMLElement> RMLDocument::as_element() const {
	return RMLServer::get_singleton()->get_document_root(rid);
}

Ref<RMLElement> RMLDocument::create_element(const String &p_tag_name) const {
	return RMLServer::get_singleton()->create_element(rid, p_tag_name);
}

void RMLDocument::_bind_methods() {
	ClassDB::bind_method(D_METHOD("as_element"), &RMLDocument::as_element);
	ClassDB::bind_method(D_METHOD("create_element", "tag_name"), &RMLDocument::create_element);
}

RMLDocument::RMLDocument() { 
	rid = RMLServer::get_singleton()->create_document(get_canvas_item());
	RMLServer::get_singleton()->document_set_size(rid, get_size());
}

RMLDocument::~RMLDocument() {
	if (rid.is_valid()) {
		RMLServer::get_singleton()->free_rid(rid);
	}
}