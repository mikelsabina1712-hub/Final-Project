extends GameManager

var days_passed: int = 0
enum Difficulty { EASY, MID, HARD }
var difficulty: Difficulty = Difficulty.EASY
var current_correct_index: int = -1

func _ready() -> void:
	print("GameManager (GDScript) READY")
