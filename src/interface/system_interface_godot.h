#pragma once
#include <RmlUi/Core/SystemInterface.h>
#include "../rml_util.h"

namespace godot {

class SystemInterfaceGodot : public Rml::SystemInterface {
public:
	double GetElapsedTime() override;

	int TranslateString(Rml::String& translated, const Rml::String& input) override;

	void JoinPath(Rml::String& translated_path, const Rml::String& document_path, const Rml::String& path) override;

	bool LogMessage(Rml::Log::Type msg_type, const Rml::String& msg) override;

	void SetMouseCursor(const Rml::String& cursor_name) override;

	void SetClipboardText(const Rml::String& text) override;

	void GetClipboardText(Rml::String& text) override;

	void ActivateKeyboard(Rml::Vector2f caret_position, float line_height) override;

	void DeactivateKeyboard() override;
};

}
