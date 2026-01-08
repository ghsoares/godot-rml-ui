#pragma once
#include <RmlUi/Core/Plugin.h>
#include <RmlUi/Core/StyleSheetContainer.h>
#include "../rml_util.h"

namespace godot {

class RmlPluginGodot : public Rml::Plugin {
	Rml::SharedPtr<Rml::StyleSheetContainer> default_stylesheet = nullptr;

	static RmlPluginGodot *singleton;

public:
	static RmlPluginGodot *get_singleton();
	void set_default_stylesheet(Rml::SharedPtr<Rml::StyleSheetContainer> stylesheet);

	void OnDocumentLoad(Rml::ElementDocument* document) override;

    RmlPluginGodot();
};

}
