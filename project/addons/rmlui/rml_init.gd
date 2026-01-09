@tool
extends Node

func _init() -> void:
	if not RMLServer.is_initialized():
		RMLServer.initialize()
	
func _exit_tree() -> void:
	if RMLServer.is_initialized():
		RMLServer.uninitialize()
	

	
