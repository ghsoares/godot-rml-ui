#include "../rml_util.h"

using namespace godot;

Rml::Input::KeyModifier godot::godot_to_rml_key_modifiers(const BitField<KeyModifierMask> &p_mod) {
	unsigned char mod = 0;

	if (p_mod.has_flag(KeyModifierMask::KEY_MASK_CTRL)) {
		mod |= Rml::Input::KeyModifier::KM_CTRL;
	}
	if (p_mod.has_flag(KeyModifierMask::KEY_MASK_SHIFT)) {
		mod |= Rml::Input::KeyModifier::KM_SHIFT;
	}
	if (p_mod.has_flag(KeyModifierMask::KEY_MASK_ALT)) {
		mod |= Rml::Input::KeyModifier::KM_ALT;
	}
	if (p_mod.has_flag(KeyModifierMask::KEY_MASK_META)) {
		mod |= Rml::Input::KeyModifier::KM_META;
	}

	return (Rml::Input::KeyModifier)mod;
}

BitField<KeyModifierMask> godot::rml_to_godot_key_modifiers(const Rml::Input::KeyModifier &p_mod) {
	BitField<KeyModifierMask> mod = 0;

	if (p_mod & Rml::Input::KeyModifier::KM_CTRL) {
		mod.set_flag(KeyModifierMask::KEY_MASK_CTRL);
	}
	if (p_mod & Rml::Input::KeyModifier::KM_SHIFT) {
		mod.set_flag(KeyModifierMask::KEY_MASK_SHIFT);
	}
	if (p_mod & Rml::Input::KeyModifier::KM_ALT) {
		mod.set_flag(KeyModifierMask::KEY_MASK_ALT);
	}
	if (p_mod & Rml::Input::KeyModifier::KM_META) {
		mod.set_flag(KeyModifierMask::KEY_MASK_META);
	}

	return mod;
}