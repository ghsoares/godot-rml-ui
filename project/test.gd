extends RMLDocument

func get_rml_tree_string(el: RMLElement, depth: int = 0, tab = "\t") -> String:
	var s: PackedStringArray
	s.append("%s<%s>" % [tab.repeat(depth), el.get_tag_name()])
	
	for c: RMLElement in el.get_children():
		s.append(get_rml_tree_string(c, depth + 1, tab))
	
	s.append("%s</%s>" % [tab.repeat(depth), el.get_tag_name()])
	
	return "\n".join(s)

func _ready() -> void:
	load_from_path("res://test.rml")
	pass
	
	
	
	
	
	
	
	
	
