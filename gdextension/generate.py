


def generate_input_key_mapping():
	# Maps godot -> RmlUi keys
	key_mapping = {
		"KEY_NONE": "KI_UNKNOWN",
		"KEY_ESCAPE": "KI_ESCAPE",
		"KEY_TAB": "KI_TAB",
		"KEY_BACKSPACE": "KI_BACK",
		"KEY_ENTER": "KI_RETURN",
		"KEY_KP_ENTER": "KI_NUMPADENTER",
		"KEY_INSERT": "KI_INSERT",
		"KEY_DELETE": "KI_DELETE",
		"KEY_PAUSE": "KI_PAUSE",
		"KEY_PRINT": "KI_PRINT",
		"KEY_CLEAR": "KI_CLEAR",
		"KEY_HOME": "KI_HOME",
		"KEY_END": "KI_END",
		"KEY_LEFT": "KI_LEFT",
		"KEY_UP": "KI_UP",
		"KEY_RIGHT": "KI_RIGHT",
		"KEY_DOWN": "KI_DOWN",
		"KEY_PAGEUP": "KI_PRIOR",
		"KEY_PAGEDOWN": "KI_NEXT",
		"KEY_SHIFT": "KI_LSHIFT",
		"KEY_CTRL": "KI_LCONTROL",
		"KEY_META": "KI_LMETA",
		"KEY_CAPSLOCK": "KI_CAPITAL",
		"KEY_NUMLOCK": "KI_NUMLOCK",
		"KEY_SCROLLLOCK": "KI_SCROLL",
		"KEY_KP_MULTIPLY": "KI_MULTIPLY",
		"KEY_KP_DIVIDE": "KI_DIVIDE",
		"KEY_KP_SUBTRACT": "KI_SUBTRACT",
		"KEY_KP_PERIOD": "KI_DECIMAL",
		"KEY_KP_ADD": "KI_ADD",
	}

	for i in range(0, 24):
		key_mapping[f"KEY_F{i+1}"] = f"KI_F{i+1}"
	for i in range(0, 10):
		key_mapping[f"KEY_KP_{i}"] = f"KI_NUMPAD{i}"
		key_mapping[f"KEY_{i}"] = f"KI_{i}"
	for i in range(0, 26):
		key_mapping[f"KEY_{chr(65 + i)}"] = f"KI_{chr(65 + i)}"

	godot_to_rml_key_code = "switch (p_key) {"
	rml_to_godot_key_code = "switch (p_key) {"

	for godot_key, rml_ui_key in key_mapping.items():
		godot_to_rml_key_code += f"case Key::{godot_key}: return Rml::Input::{rml_ui_key}; "
		rml_to_godot_key_code += f"case Rml::Input::{rml_ui_key}: return Key::{godot_key}; "

	godot_to_rml_key_code += "default: return Rml::Input::KI_UNKNOWN;}"
	rml_to_godot_key_code += "default: return Key::KEY_NONE;}"

	generated_code = f"""// Generated using generate.py
#include "../rml_util.h"

using namespace godot;

Rml::Input::KeyIdentifier godot::godot_to_rml_key(const Key &p_key) {{{godot_to_rml_key_code}}}
Key godot::rml_to_godot_key(const Rml::Input::KeyIdentifier &p_key) {{{rml_to_godot_key_code}}}
	"""


	filename = "src/input/godot_rml_key_mapping.cpp"

	try:
		with open(filename, "w") as file:
			file.write(generated_code.strip())
		print(f"Generated {filename}")
	except IOError as e:
		print(f"Error generating file {filename}: {e}")

if __name__ == "__main__":
	generate_input_key_mapping()

