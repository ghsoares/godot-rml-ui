# Godot RmlUi

A GDExtension for rendering and manipulating documents using [RmlUi](https://github.com/mikke89/RmlUi).

Supports Godot version 4.5+

## Platforms

- Linux
- Windows
- Android (todo)

## Getting Started

Download the addon from releases pages, or head over to [Building Section](#building) to build the GDExtension locally.

A new Control-based node is added: RMLDocument, add it to your scene to create a new document.

By default, this node creates an empty document where you can add, remove and modify elements. If you want to load a .rml document instead, use `load_from_path`, for example:

```xml
<!-- res://test.rml -->
<rml>
<body>
	<span>Hello!</span>
	<button>Test Button</button>
	<svg src="icon.svg"/>
	<input type="text"/>
</body>
</rml>
```

```godot
extends RMLDocument

func _ready() -> void:
	load_from_path("res://test.rml")
```

## Supported features

- Document creation and manipulation with `RMLDocument` node;
	- Can also create and manipulate documents directly through `RMLServer`.
- Element reference with `RMLElement` class;
- `query_selector` and `query_selector_all` to finding elements in the document;
- Element inner RML, text content, attributes, style properties, class names and id;
- Full event integration, as well listening to RmlUi's issued events through `add_event_listener`;
- Full style support with [RCSS](https://mikke89.github.io/RmlUiDoc/pages/rcss.html);
- User-agent style for basic element presentation and style based on Godot's theme;
	- Optional, can set project setting `RmlUi/load_user_agent_stylesheet` to false;
	- Can also override with project setting `RmlUi/custom_user_agent_stylesheet`;

## Building

See [Introduction to the buildsystem](https://docs.godotengine.org/en/4.4/contributing/development/compiling/introduction_to_the_buildsystem.html) for the required tools to building this GDExtension in your machine.

You can also build the Dockerfile provided in this repository.

Then build this GDExtension with:

```bash
scons platform=<platform> target=<target>
```

Replace `<platform>` with the desired platform:
- `linux` for Linux systems;
- `windows` for Windows systems.

Replace `<target>` with the desiredbuild target:
- `template_debug` for debug builds;
- `template_release` for optimized, release builds.

The binary will be compiled in `project/addons/rmlui/bin/` folder.

## Thirdparty

See [thirdparty README](thirdparty/README.md)