#pragma once

#include <godot_cpp/variant/callable.hpp>
#include <RmlUi/Core/EventListener.h>

namespace godot {

typedef std::tuple<uintptr_t, int64_t, int64_t> listener_key;

class RMLEventListener: public Rml::EventListener {
private:
	Callable target_callable;
	String event_id;

	static std::map<listener_key, RMLEventListener *> listeners;
public:
	void ProcessEvent(Rml::Event& event) override;
	void OnDetach(Rml::Element* element) override;
	String get_event_id() const { return event_id; }

	static RMLEventListener *get_listener(Rml::Element *p_element, const String &p_event_id, const Callable &p_listener);
	static RMLEventListener *create_listener(Rml::Element *p_element, const String &p_event_id, const Callable &p_listener);
};

}
