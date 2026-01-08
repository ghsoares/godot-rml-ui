extends RMLDocument

func get_rml_tree_string(el: RMLElement, depth: int = 0, tab = "\t") -> String:
	var s: PackedStringArray
	s.append("%s<%s>" % [tab.repeat(depth), el.get_tag_name()])
	
	for c: RMLElement in el.get_children():
		s.append(get_rml_tree_string(c, depth + 1, tab))
	
	s.append("%s</%s>" % [tab.repeat(depth), el.get_tag_name()])
	
	return "\n".join(s)

func _ready() -> void:
	var test_doc: RID = RMLServer.create_document_from_path(get_canvas_item(), "res://test.rml")
	
	print(get_rml_tree_string(RMLServer.get_document_root(test_doc)))
	
	#var root_el: RMLElement = as_element()
	#root_el.set_property("width", "100%")
	#root_el.set_property("height", "100%")
	#root_el.set_property("background-color", "#FF0000")
	#
	#var head_el: RMLElement = create_element("head")
	#
	#print(root_el.get_tag_name())
	
	#var style_el: RMLElement = create_element("style")
	#style_el.set_property("font-family", "Open Sans")
	#style_el.set_text_content("body { font-family: 'Open Sans'; } div { background-color: #00FF00; }")
	#root_el.append_child(style_el)
	#
	#var test_el: RMLElement = create_element("div")
	#test_el.set_property("display", "block")
	#test_el.set_property("width", "256px")
	#test_el.set_property("height", "256px")
	##test_el.set_property("background-color", "#00FF00")
	#root_el.append_child(test_el)
	
	
	
	
	
	
	
	
	
