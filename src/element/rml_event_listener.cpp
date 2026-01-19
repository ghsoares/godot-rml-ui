#include "rml_event_listener.h"
#include "rml_element.h"
#include "../rml_util.h"

#include <godot_cpp/variant/utility_functions.hpp>

#include <RmlUi/Core/Element.h>

using namespace godot;

std::map<listener_key, RMLEventListener *> RMLEventListener::listeners = {};

void RMLEventListener::ProcessEvent(Rml::Event &event) {
	Dictionary event_dict;

	ElementRef target_element_ref(event.GetTargetElement());
	ElementRef current_element_ref(event.GetCurrentElement());

	event_dict["type"] = rml_to_godot_string(event.GetType());
	event_dict["target_element"] = RMLElement::ref(target_element_ref);
	event_dict["current_element"] = RMLElement::ref(current_element_ref);

	for (auto it : event.GetParameters()) {
		event_dict[rml_to_godot_string(it.first)] = rml_to_godot_variant(it.second);
	}

	target_callable.call(event_dict);

	if (event_dict.get("stop_propagation", false)) {
		event.StopPropagation();
	}
	if (event_dict.get("stop_immediate_propagation", false)) {
		event.StopImmediatePropagation();
	}
}

void RMLEventListener::OnDetach(Rml::Element* element) {
	memdelete(this);
}

RMLEventListener *RMLEventListener::get_listener(Rml::Element *p_element, const String &p_event_id, const Callable &p_listener) {
	auto it = listeners.find(std::make_tuple((std::size_t)p_element, p_event_id.hash(), p_listener.hash()));
	if (it == listeners.end()) {
		return nullptr;
	}
	return it->second;
}

RMLEventListener *RMLEventListener::create_listener(Rml::Element *p_element, const String &p_event_id, const Callable &p_listener) {
	RMLEventListener *listener_obj = memnew(RMLEventListener);

	listeners.insert({
		std::make_tuple((std::size_t)p_element, p_event_id.hash(), p_listener.hash()), 
		listener_obj
	});

	listener_obj->target_callable = p_listener;
	listener_obj->event_id = p_event_id;

	p_element->AddEventListener(godot_to_rml_string(p_event_id), listener_obj);

	return listener_obj;
}

