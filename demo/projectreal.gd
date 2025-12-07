# project.gd
# Attach this to the ROOT node named "Project"

extends Node2D  # or Node, depending on what your Project node is

@onready var ui: CanvasLayer      = $UI
@onready var game_manager         = $GameManager
@onready var day_manager          = $GameManager/DayManager
@onready var quiz_manager         = $GameManager/QuizManager

func _ready() -> void:
	# Debug prints so we can see everything is wired correctly
	if game_manager:
		print("project.gd: game_manager path = ", game_manager.get_path())
	else:
		push_error("project.gd: GameManager node not found as child of Project")

	if day_manager:
		print("project.gd: day_manager path = ", day_manager.get_path())
	else:
		push_error("project.gd: DayManager node not found under GameManager")

	if quiz_manager:
		print("project.gd: quiz_manager path = ", quiz_manager.get_path())
	else:
		push_error("project.gd: QuizManager node not found under GameManager")

	# Initialize UI once at startup
	update_ui()


func update_ui() -> void:
	# Called by others to refresh labels; forwards to UI.gd helpers
	if not ui:
		push_error("project.gd: UI node not found")
		return

	if ui.has_method("update_day"):
		ui.update_day()
	if ui.has_method("update_tokens"):
		ui.update_tokens()


func _on_inspect_eyes_button_pressed() -> void:
	pass # Replace with function body.


func _on_inspect_teeth_button_pressed() -> void:
	pass # Replace with function body.


func _on_inspect_hands_button_pressed() -> void:
	pass # Replace with function body.


func _on_inspect_feet_button_pressed() -> void:
	pass # Replace with function body.


func _on_inspect_hair_button_pressed() -> void:
	pass # Replace with function body.


func _on_inspect_resident_3_button_pressed() -> void:
	pass # Replace with function body.
