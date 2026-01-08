@tool
extends EditorPlugin

func _enter_tree() -> void:
	add_autoload_singleton("RMLInit", "res://addons/rmlui/rml_init.gd")

func _exit_tree() -> void:
	remove_autoload_singleton("RMLInit")
	pass


