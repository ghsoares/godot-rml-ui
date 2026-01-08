#pragma once

#include <RmlUi/Core/StringUtilities.h>
#include <RmlUi/Core/Variant.h>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>

namespace godot {

Rml::String godot_to_rml_string(const String &p_str);
String rml_to_godot_string(const Rml::String &p_str);

Rml::Variant godot_to_rml_variant(const Variant &p_var);
Variant rml_to_godot_variant(const Rml::Variant &p_var);


}

