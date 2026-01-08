@tool
extends EditorPlugin

func _enter_tree() -> void:
	# RMLServer.load_font_face("res://addons/rml/fonts/OpenSans-VariableFont_wdth,wght.ttf")
	var default_font: FontFile = ThemeDB.fallback_font as FontFile
	if default_font:
		RMLServer.load_font_face_from_buffer(default_font.data, "default")

func _exit_tree() -> void:
	pass


