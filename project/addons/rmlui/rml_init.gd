@tool
extends Node

func _init() -> void:
	if not RMLServer.is_initialized():
		RMLServer.load_default_stylesheet("res://addons/rmlui/default.rcss")
		var default_font: FontFile = ThemeDB.fallback_font as FontFile
		if default_font:
			RMLServer.load_font_face_from_buffer(default_font.data, "default", true)
		RMLServer.initialize()
		print("initialized!")
	
