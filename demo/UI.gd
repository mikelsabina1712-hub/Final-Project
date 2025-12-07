extends CanvasLayer

# -------------------------
# PHASES
# -------------------------

enum Phase {
	INTRO,
	HOW_TO_PLAY,
	DIFFICULTY_SELECT,
	NIGHT,
	SLEEPING,
	MORNING_NEWS,
	QUIZ,
	INSPECTION,
	GAME_OVER,
	VICTORY
}

var current_phase : int = Phase.INTRO

@onready var background_image : TextureRect = $BackgroundImage

@export var bg_intro         : Texture2D
@export var bg_how_to        : Texture2D
@export var bg_difficulty    : Texture2D
@export var bg_night         : Texture2D
@export var bg_sleeping      : Texture2D
@export var bg_morning_news  : Texture2D
@export var bg_quiz          : Texture2D
@export var bg_inspection    : Texture2D
@export var bg_game_over     : Texture2D
@export var bg_victory       : Texture2D

# -------------------------
# C++ MANAGERS
# -------------------------

@onready var game_manager = get_parent().get_node("GameManager")
@onready var day_manager  = get_parent().get_node("GameManager/DayManager")
@onready var quiz_manager = get_parent().get_node("GameManager/QuizManager")

# -------------------------
# BASIC UI
# -------------------------

@onready var day_label    : Label  = $DayLabel
@onready var tokens_label : Label  = $TokensLabel
@onready var status_label : Label  = $StatusLabel
var quiz_finished: bool = false


@onready var next_phase_button : Button = $NextPhaseButton

# -------------------------
# DIFFICULTY SELECTION
# -------------------------

@onready var difficulty_panel : Panel  = $DifficultyPanel
@onready var easy_button      : Button = $DifficultyPanel/EasyButton
@onready var mid_button       : Button = $DifficultyPanel/MidButton
@onready var hard_button      : Button = $DifficultyPanel/HardButton


# -------------------------
# INTRO / HOW TO PLAY
# -------------------------

@onready var intro_panel  : Panel       = $IntroPanel
@onready var intro_image  : TextureRect = $IntroPanel/IntroImage
@onready var intro_label  : Label       = $IntroPanel/IntroLabel

@onready var howto_panel  : Panel         = $HowToPlayPanel
@onready var howto_image  : TextureRect   = $HowToPlayPanel/HowToImage
@onready var howto_label  : RichTextLabel = $HowToPlayPanel/HowToText

var howto_lines : Array[String] = [
	"Welcome! In this game you manage who gets into your safe house during a zombie outbreak.",
	"Every morning, three visitors show up at your door.",
	"Some visitors are humans. Some are zombies pretending to be teachers. You must decide who to let in and who to reject.",
	"At night, anyone inside your house can protect you… or kill you. Humans keep you safe. Zombies can attack.",
	"If nobody is in the house, somebody will come at kill you, so you better have someone.",
	"Each day starts with a quiz. Correct answers earn you tokens. You can spend tokens to inspect residents more closely.",
	"Survive enough days without dying and you win. Choose wisely. Ready to begin?"
]

var howto_index : int = 0

# -------------------------
# NIGHT / VISITORS
# -------------------------

@onready var visitor_image : TextureRect = $VisitorPanel/VisitorImage
@onready var visitor_panel : Panel  = $VisitorPanel
@onready var visitor_name  : Label  = $VisitorPanel/VisitorName
@onready var let_in_button : Button = $VisitorPanel/LetInButton
@onready var reject_button : Button = $VisitorPanel/RejectButton
var current_visitor: Character = null

var current_visitor_index : int = 0

# -------------------------
# SLEEPING
# -------------------------

@onready var sleeping_panel: Control = $SleepingPanel
@onready var sleep_notifications_label: Label = $SleepingPanel/SleepNotificationsLabel

# -------------------------
# NEWS
# -------------------------


@onready var morning_news_panel : Panel = $MorningNewsPanel
@onready var morning_news_title : Label = $MorningNewsPanel/TitleLabel
@onready var morning_news_text  : RichTextLabel = $MorningNewsPanel/NewsText
@onready var morning_news_example_image: TextureRect = $MorningNewsPanel/ExampleImage

@export var morning_example_textures: Array[Texture2D] = []


# Which zombie clues appear on which day,and which feature they unlock for inspection.
var zombie_clues : Array[Dictionary] = [
	{
		"feature": "eyes",
		"text": "Intel: It has been said that zombies may have blue eyes, not confirmed."
	},
	{
		"feature": "teeth",
		"text": "Intel: Zombies often have sharp, uneven  and gross teeth, but you never know with some."
	},
	{
		"feature": "hands",
		"text": "Intel: Zombies usually have dirty or wounded hands."
	},
	{
		"feature": "feet",
		"text": "Intel: Zombies tend to have damaged and dirty feet."
	},
	{
		"feature": "hair",
		"text": "Intel: Zombies rarely bother with neat hair."
	},
	{
		"feature": "eyes",
		"text": "Intel: It has been confirmed, if you see somebody with blue eyes, it's a zombie."
	},
	{
		"feature": "hair",
		"text": "Intel: Final piece of advice, missing teeth= zombie."
	},
]

# Feature names you are allowed to inspect so far
var unlocked_features : Array[String] = []


func _feature_unlocked(feature: String) -> bool:
	return unlocked_features.has(feature)

# -------------------------
# QUIZ
# -------------------------

@onready var quiz_panel       : Panel  = $QuizPanel
@onready var question_label   : Label  = $QuizPanel/QuestionLabel
@onready var answer_a_button  : Button = $QuizPanel/AnswerAButton
@onready var answer_b_button  : Button = $QuizPanel/AnswerBButton
@onready var answer_c_button  : Button = $QuizPanel/AnswerCButton
@onready var answer_d_button  : Button = $QuizPanel/AnswerDButton

var current_question : Dictionary = {}
var quiz_questions_answered : int = 0
var max_quiz_questions : int = 5

# -------------------------
# FEATURES
# -------------------------
var current_inspection_resident_index: int = -1
@onready var inspect_result_panel: Control = $InspectPartsPanel/PartsButtonsContainer

@onready var residents_panel      : Control          = $ResidentsPanel
@onready var inspection_panel     : Panel          = $InspectPartsPanel

# Residents overview buttons (example for 3 slots)
@onready var inspect_resident0_button: Button = $ResidentsPanel/ScrollContainer/ResidentsList/ResidentEntry0/ButtonsRow0/InspectResident0Button
@onready var kill_resident0_button  : Button = $ResidentsPanel/ScrollContainer/ResidentsList/ResidentEntry0/ButtonsRow0/KillResident0Button

@onready var inspect_resident1_button: Button = $ResidentsPanel/ScrollContainer/ResidentsList/ResidentEntry1/ButtonsRow1/InspectResident1Button
@onready var kill_resident1_button  : Button = $ResidentsPanel/ScrollContainer/ResidentsList/ResidentEntry1/ButtonsRow1/KillResident1Button

@onready var inspect_resident2_button: Button = $ResidentsPanel/ScrollContainer/ResidentsList/ResidentEntry2/ButtonsRow2/InspectResident2Button
@onready var kill_resident2_button  : Button = $ResidentsPanel/ScrollContainer/ResidentsList/ResidentEntry2/ButtonsRow2/KillResident2Button

# Inspect-parts buttons (already exist, just make sure they live under InspectionPanel)
@onready var inspect_eyes_button  : Button = $InspectPartsPanel/PartsButtonsContainer/InspectEyesButton
@onready var inspect_hair_button  : Button = $InspectPartsPanel/PartsButtonsContainer/InspectHairButton
@onready var inspect_teeth_button : Button = $InspectPartsPanel/PartsButtonsContainer/InspectTeethButton
@onready var inspect_hands_button : Button = $InspectPartsPanel/PartsButtonsContainer/InspectHandsButton
@onready var inspect_feet_button  : Button = $InspectPartsPanel/PartsButtonsContainer/InspectFeetButton

@onready var inspection_back_button: Button = $InspectPartsPanel/BackButton



# -------------------------
# FEATURES (per-part inspect)
# -------------------------

@onready var inspect_parts_panel  : Panel       = $InspectPartsPanel
@onready var inspect_image        : TextureRect = $InspectPartsPanel/InspectImage

var inspected_resident_index : int = -1
var inspected_character      : Node = null



# ---------------------------------------------------------
# READY
# ---------------------------------------------------------

func _ready() -> void:
	if game_manager:
		print("UI.gd: game_manager path = ", game_manager.get_path())
	if day_manager:
		print("UI.gd: day_manager path = ", day_manager.get_path())
	if quiz_manager:
		print("UI.gd: quiz_manager path = ", quiz_manager.get_path())

	# Connect buttons
	easy_button.pressed.connect(_on_easy_button_pressed)
	mid_button.pressed.connect(_on_mid_button_pressed)
	hard_button.pressed.connect(_on_hard_button_pressed)

	let_in_button.pressed.connect(_on_let_in_pressed)
	reject_button.pressed.connect(_on_reject_pressed)

	next_phase_button = $NextPhaseButton  # or the correct path
	next_phase_button.pressed.connect(_on_next_phase_pressed)

	answer_a_button.pressed.connect(_on_answer_a_pressed)
	answer_b_button.pressed.connect(_on_answer_b_pressed)
	answer_c_button.pressed.connect(_on_answer_c_pressed)
	answer_d_button.pressed.connect(_on_answer_d_pressed)

	# initial UI: intro screen first
	_show_intro_screen()
	_update_background_for_phase()
	_update_hud("Welcome!")
	update_ui()
	_update_inspection_buttons()

# ---------------------------------------------------------
# HUD HELPERS
# ---------------------------------------------------------
func _show_manager_notifications() -> void:
	if not game_manager:
		return
	if not game_manager.has_method("get_notifications"):
		return

	var msgs: Array = game_manager.get_notifications()
	if msgs.is_empty():
		return

	# Join all messages into one block of text
	var text := "\n".join(msgs)

	print("UI.gd: showing notifications:\n", text)  # debug
	_update_hud(text)

	if game_manager.has_method("clear_notifications"):
		game_manager.clear_notifications()


func _show_character_feature(character: Node, part_name: String) -> void:
	if character == null:
		_update_hud("No character to inspect.")
		return

	# Get the sprite for this body part from the character
	var sprite := character.get_node_or_null(part_name)
	if sprite == null or not (sprite is Sprite2D):
		_update_hud("Character has no %s sprite." % part_name)
		return

	var s := sprite as Sprite2D
	if s.texture == null:
		_update_hud("No texture found for %s." % part_name)
		return

	inspect_image.texture = s.texture
	inspect_image.visible = true



func _update_day_and_tokens() -> void:
	if day_label == null:
		day_label = get_node_or_null("DayLabel")
	if tokens_label == null:
		tokens_label = get_node_or_null("TokensLabel")

	if day_label == null or tokens_label == null:
		return

	if game_manager:
		if game_manager.has_method("get_day"):
			day_label.text = "Day %d" % game_manager.get_day()
		elif game_manager.has_method("get_days_passed"):
			day_label.text = "Day %d" % game_manager.get_days_passed()
		else:
			day_label.text = "Day ?"

		if game_manager.has_method("get_tokens"):
			tokens_label.text = "Tokens: %d" % game_manager.get_tokens()
		else:
			tokens_label.text = "Tokens: ?"
	else:
		day_label.text = "Day -"
		tokens_label.text = "Tokens: -"



func update_ui() -> void:
	_update_day_and_tokens()

func update_day() -> void:
	_update_day_and_tokens()

func update_tokens() -> void:
	_update_day_and_tokens()

func _update_hud(text: String) -> void:
	if status_label:
		status_label.text = text

func update_hud(text: String = "") -> void:
	if text != "":
		_update_hud(text)

# ---------------------------------------------------------
# DIFFICULTY & GAME START
# ---------------------------------------------------------
func _show_intro_screen() -> void:
	current_phase = Phase.INTRO

	intro_panel.visible      = true
	howto_panel.visible      = false
	difficulty_panel.visible = false
	visitor_panel.visible    = false
	quiz_panel.visible       = false
	residents_panel.visible  = false
	sleeping_panel.visible = false



	next_phase_button.disabled = false
	next_phase_button.text = "Start game"

	_update_background_for_phase()

func enter_how_to_play_phase() -> void:
	current_phase = Phase.HOW_TO_PLAY

	intro_panel.visible        = false
	howto_panel.visible        = true
	difficulty_panel.visible   = false
	quiz_panel.visible         = false
	inspect_parts_panel.visible   = false
	residents_panel.visible    = false
	visitor_panel.visible      = false
	morning_news_panel.visible = false
	sleeping_panel.visible = false

	howto_index = 0
	_update_howto_text()

	next_phase_button.disabled = false
	next_phase_button.text = "Continue"

	_update_background_for_phase()


func _update_howto_text() -> void:
	if howto_index >= 0 and howto_index < howto_lines.size():
		howto_label.text = howto_lines[howto_index]
	else:
		howto_label.text = ""

func _show_difficulty_select() -> void:
	current_phase = Phase.DIFFICULTY_SELECT

	intro_panel.visible        = false
	howto_panel.visible        = false
	difficulty_panel.visible   = true
	quiz_panel.visible         = false
	inspect_parts_panel.visible   = false
	residents_panel.visible    = false
	visitor_panel.visible      = false
	morning_news_panel.visible = false
	sleeping_panel.visible = false

	next_phase_button.disabled = true
	next_phase_button.text = "Next"

	_update_background_for_phase()


func _start_game_with_difficulty(diff: int) -> void:
	if game_manager:
		if game_manager.has_method("set_difficulty"):
			game_manager.set_difficulty(diff)
		game_manager.start_new_game()

	update_ui()
	difficulty_panel.visible = false
	enter_night_phase()

func _on_easy_button_pressed() -> void:
	_start_game_with_difficulty(0)

func _on_mid_button_pressed() -> void:
	_start_game_with_difficulty(1)

func _on_hard_button_pressed() -> void:
	_start_game_with_difficulty(2)

# ---------------------------------------------------------
# PHASE ENTRY FUNCTIONS
# ---------------------------------------------------------
func _update_background_for_phase() -> void:
	if not is_instance_valid(background_image):
		return

	match current_phase:
		Phase.INTRO:
			background_image.texture = bg_intro
		Phase.HOW_TO_PLAY:
			background_image.texture = bg_how_to
		Phase.DIFFICULTY_SELECT:
			background_image.texture = bg_difficulty
		Phase.NIGHT:
			background_image.texture = bg_night
		Phase.SLEEPING:
			background_image.texture = bg_sleeping
		Phase.MORNING_NEWS:
			background_image.texture = bg_morning_news
		Phase.QUIZ:
			background_image.texture = bg_quiz
		Phase.INSPECTION:
			background_image.texture = bg_inspection
		Phase.GAME_OVER:
			background_image.texture = bg_game_over
		Phase.VICTORY:
			background_image.texture = bg_victory

	# Hide if no texture set for this phase (optional)
	if background_image.texture == null:
		background_image.visible = false
	else:
		background_image.visible = true


func enter_night_phase() -> void:
	if not game_manager or not day_manager:
		return

	current_phase = Phase.NIGHT

	intro_panel.visible        = false
	howto_panel.visible        = false
	difficulty_panel.visible   = false
	quiz_panel.visible         = false
	inspect_parts_panel.visible   = false
	residents_panel.visible    = true
	visitor_panel.visible      = false
	morning_news_panel.visible = false
	sleeping_panel.visible = false
	
	_refresh_residents_ui()

	let_in_button.disabled = false
	reject_button.disabled = false

	next_phase_button.disabled = false
	next_phase_button.text = "Sleep"

	game_manager.start_night()

	current_visitor_index = 0
	_show_next_visitor()

	var day = game_manager.get_day() if game_manager.has_method("get_day") else 1
	_update_hud("Night %d – Someone is at the door." % day)
	update_ui()
	
	_update_background_for_phase()


func enter_sleeping_phase() -> void:
	current_phase = Phase.SLEEPING

	intro_panel.visible        = false
	howto_panel.visible        = false
	difficulty_panel.visible   = false
	quiz_panel.visible         = false
	inspect_parts_panel.visible   = false
	residents_panel.visible    = false
	visitor_panel.visible      = false
	morning_news_panel.visible = false
	sleeping_panel.visible = false

	next_phase_button.disabled = false

	next_phase_button.text = "Next Morning"
	_update_hud("You go to sleep...")

	# Night resolution in C++
	if game_manager and game_manager.has_method("resolve_night_danger"):
		game_manager.resolve_night_danger()
		
	_show_sleep_notifications()


	if game_manager:
		if game_manager.has_method("has_player_won") and game_manager.has_player_won():
			enter_victory_phase()
			return
		if game_manager.has_method("is_player_alive") and not game_manager.is_player_alive():
			enter_game_over_phase()
			return

	_update_background_for_phase()


func enter_morning_news_phase() -> void:
	if current_phase == Phase.GAME_OVER or current_phase == Phase.VICTORY:
		return

	current_phase = Phase.MORNING_NEWS

	intro_panel.visible        = false
	howto_panel.visible        = false
	difficulty_panel.visible   = false
	quiz_panel.visible         = false
	inspect_parts_panel.visible   = false
	residents_panel.visible    = false
	visitor_panel.visible      = false
	morning_news_panel.visible = true
	sleeping_panel.visible = false


	# Let the C++ GameManager resolve the night (same as before)
	if game_manager and game_manager.has_method("finish_night"):
		game_manager.finish_night()

		# Check win/lose after finishing the night
		if game_manager.has_method("has_player_won") and game_manager.has_player_won():
			enter_victory_phase()
			return
		if game_manager.has_method("is_player_alive") and not game_manager.is_player_alive():
			enter_game_over_phase()
			return

	# Get current day from C++ (this method DOES exist)
	var day_num: int = 0
	if game_manager and game_manager.has_method("get_day"):
		day_num = int(game_manager.get_day())

	# Basic morning text – you can customize this
	morning_news_text.text = "Morning of day %d. Watch the news..." % day_num

	# Add the daily zombie intel and unlock a feature
	_apply_daily_zombie_clue(day_num)
	_update_morning_example_image(day_num)
	next_phase_button.disabled = false
	next_phase_button.text = "Go to Quiz"

	_update_background_for_phase()
	update_ui()




func enter_quiz_phase() -> void:
	if not quiz_manager or not game_manager:
		return

	current_phase = Phase.QUIZ

	intro_panel.visible        = false
	howto_panel.visible        = false
	difficulty_panel.visible   = false
	quiz_panel.visible         = true
	inspect_parts_panel.visible   = false
	residents_panel.visible    = false
	visitor_panel.visible      = false
	morning_news_panel.visible = false
	sleeping_panel.visible = false

	next_phase_button.text = "Finish Quiz"
	next_phase_button.disabled = true

	quiz_finished = false
	quiz_questions_answered = 0
	current_question = {}

	var diff : int = 0
	if game_manager.has_method("get_difficulty"):
		diff = game_manager.get_difficulty()

	var day : int = 0
	if game_manager.has_method("get_day"):
		day = game_manager.get_day()
	elif game_manager.has_method("get_days_passed"):
		day = game_manager.get_days_passed()

	if quiz_manager.has_method("load_questions_for_day"):
		quiz_manager.load_questions_for_day(diff, day)
	elif quiz_manager.has_method("load_questions"):
		var diff_str := "easy"
		if diff == 1:
			diff_str = "mid"
		elif diff == 2:
			diff_str = "hard"
		quiz_manager.load_questions(diff_str, day)

	_update_hud("Quiz started for day %d." % day)
	_show_next_quiz_question()

	_update_background_for_phase()

func _update_inspection_buttons() -> void:
	inspect_eyes_button.disabled  = not _feature_unlocked("eyes")
	inspect_hair_button.disabled  = not _feature_unlocked("hair")
	inspect_teeth_button.disabled = not _feature_unlocked("teeth")
	inspect_hands_button.disabled = not _feature_unlocked("hands")
	inspect_feet_button.disabled  = not _feature_unlocked("feet")

func _on_BackButton_pressed() -> void:
	inspection_panel.visible = false
	residents_panel.visible  = true
	
	current_inspection_resident_index = -1


func enter_inspection_phase() -> void:
	current_phase = Phase.INSPECTION

	# SHOW: residents overview
	residents_panel.visible    = true

	# HIDE: detailed inspection + other panels
	inspection_panel.visible   = false
	intro_panel.visible        = false
	howto_panel.visible        = false
	difficulty_panel.visible   = false
	quiz_panel.visible         = false
	visitor_panel.visible      = false
	morning_news_panel.visible = false
	sleeping_panel.visible = false

	current_inspection_resident_index = -1

	next_phase_button.disabled = false
	next_phase_button.text = "Go to night"

	_update_inspection_buttons()
	_update_background_for_phase()
	_refresh_residents_ui()


func _on_InspectEyesButton_pressed() -> void:
	if not _feature_unlocked("eyes"):
		_update_hud("You don't know how to inspect eyes yet.")
		return

	if current_inspection_resident_index < 0:
		_update_hud("Choose a resident first.")
		return

	if not game_manager.spend_tokens(1):
		_update_hud("Not enough tokens.")
		return

	update_tokens()
	var c = game_manager.get_resident_character(current_inspection_resident_index)
	_show_character_feature(c, "Eyes")


func _on_InspectHairButton_pressed() -> void:
	if not _feature_unlocked("hair"):
		_update_hud("You don't know how to inspect hair yet.")
		return

	if current_inspection_resident_index < 0:
		_update_hud("Choose a resident first.")
		return

	if not game_manager.spend_tokens(1):
		_update_hud("Not enough tokens.")
		return
	update_tokens()

	var c = game_manager.get_resident_character(current_inspection_resident_index)
	_show_character_feature(c, "Hair")


func _on_InspectTeethButton_pressed() -> void:
	if not _feature_unlocked("teeth"):
		_update_hud("You don't know how to inspect teeth yet.")
		return

	if current_inspection_resident_index < 0:
		_update_hud("Choose a resident first.")
		return

	if not game_manager.spend_tokens(1):
		_update_hud("Not enough tokens.")
		return
	update_tokens()

	var c = game_manager.get_resident_character(current_inspection_resident_index)
	_show_character_feature(c, "Teeth")


func _on_InspectHandsButton_pressed() -> void:
	if not _feature_unlocked("hands"):
		_update_hud("You don't know how to inspect hands yet.")
		return

	if current_inspection_resident_index < 0:
		_update_hud("Choose a resident first.")
		return

	if not game_manager.spend_tokens(1):
		_update_hud("Not enough tokens.")
		return

	update_tokens()

	var c = game_manager.get_resident_character(current_inspection_resident_index)
	_show_character_feature(c, "Hands")


func _on_InspectFeetButton_pressed() -> void:
	if not _feature_unlocked("feet"):
		_update_hud("You don't know how to inspect feet yet.")
		return

	if current_inspection_resident_index < 0:
		_update_hud("Choose a resident first.")
		return

	if not game_manager.spend_tokens(1):
		_update_hud("Not enough tokens.")
		return
	update_tokens()

	var c = game_manager.get_resident_character(current_inspection_resident_index)
	_show_character_feature(c, "Feet")








func _kill_resident_at_index(index: int) -> void:
	if not game_manager:
		return

	print("--- _kill_resident_at_index --- index =", index)

	# 1) Make sure this slot has a resident
	var count := 0
	if game_manager.has_method("get_resident_count"):
		count = game_manager.get_resident_count()
	print("  resident_count before kill =", count)

	if index < 0 or index >= count:
		_update_hud("There is no resident in that slot to kill.")
		return

	# 2) Check that the character is still valid
	var c: Node = null
	if game_manager.has_method("get_resident_character"):
		c = game_manager.get_resident_character(index)

	if c == null or not is_instance_valid(c):
		_update_hud("There is no resident in that slot to kill.")
		return

	# 3) Killing costs 1 token
	if not game_manager.spend_tokens(1):
		_update_hud("Not enough tokens to kill.")
		update_tokens()
		return

	update_tokens()

	# 4) Tell C++ to kill and erase this alive index
	if game_manager.has_method("decide_resident_fate"):
		game_manager.decide_resident_fate(index, true)
		_update_hud("Resident %d removed." % index)

	# 5) Refresh residents UI (indices now compressed)
	_refresh_residents_ui()

	# Optional C++ debug
	if game_manager.has_method("debug_print_residents"):
		game_manager.debug_print_residents()








func _start_detailed_inspection_for(resident_index: int) -> void:
	current_inspection_resident_index = resident_index


	# Switch the UI to the "inspect parts" screen
	residents_panel.visible  = false
	inspection_panel.visible = true
	sleeping_panel.visible = false

	# Update which body-part buttons are enabled based on unlocked_features
	_update_inspection_buttons()

func enter_game_over_phase() -> void:
	current_phase = Phase.GAME_OVER

	intro_panel.visible        = false
	howto_panel.visible        = false
	difficulty_panel.visible   = false
	quiz_panel.visible         = false
	inspect_parts_panel.visible   = false
	residents_panel.visible    = false
	visitor_panel.visible      = false
	morning_news_panel.visible = false
	sleeping_panel.visible = false

	next_phase_button.disabled = false
	next_phase_button.text = "Back to menu"

	_update_background_for_phase()

	next_phase_button.text = "Restart"
	_update_hud("Game over. You died.")

	_update_background_for_phase()

func enter_victory_phase() -> void:
	current_phase = Phase.VICTORY

	intro_panel.visible        = false
	howto_panel.visible        = false
	difficulty_panel.visible   = false
	quiz_panel.visible         = false
	inspect_parts_panel.visible   = false
	residents_panel.visible    = false
	visitor_panel.visible      = false
	morning_news_panel.visible = false
	sleeping_panel.visible = false

	next_phase_button.text = "Restart"
	_update_hud("YOU WIN! You survived all 7 days.")

	_update_background_for_phase()

func restart_to_difficulty() -> void:
	_show_difficulty_select()
	update_ui()

	_update_background_for_phase()


# ---------------------------------------------------------
# NIGHT / VISITORS
# ---------------------------------------------------------

func _update_visitor_image() -> void:
	if not visitor_image:
		return

	if not current_visitor or not is_instance_valid(current_visitor):
		visitor_image.texture = null
		return

	# Use Portrait as the main picture
	var sprite: Sprite2D = current_visitor.get_node_or_null("Portrait")

	# Fallbacks in case portrait is missing for some reason
	if not sprite or not sprite.texture:
		sprite = current_visitor.get_node_or_null("Hair")
	if not sprite or not sprite.texture:
		sprite = current_visitor.get_node_or_null("Eyes")

	if sprite and sprite.texture:
		visitor_image.texture = sprite.texture
	else:
		visitor_image.texture = null



func _show_next_visitor() -> void:
	if not day_manager:
		push_error("UI.gd: day_manager is null in _show_next_visitor")
		return

	var chars: Array = day_manager.get_daily_characters()
	print("UI._show_next_visitor: count =", chars.size())

	# No visitors left → go to sleep
	if chars.size() == 0:
		visitor_panel.visible = false
		next_phase_button.text = "Go to sleep"
		next_phase_button.disabled = false
		_update_hud("No more visitors today. Go to sleep.")
		return

# 	Take the *first* remaining visitor
	current_visitor = chars[0]

	visitor_panel.visible   = true
	quiz_panel.visible      = false
	residents_panel.visible = false

	next_phase_button.text = "Skip to night"
	next_phase_button.disabled = true

	# show a picture of this visitor
	_update_visitor_image()
	_update_hud("A visitor has arrived. Decide whether to let them in or not.")


func _on_let_in_pressed() -> void:
	if not current_visitor or not game_manager:
		return

	game_manager.let_visitor_in(current_visitor)
	
	if game_manager.has_method("debug_print_residents"):
		game_manager.debug_print_residents()


	# Update the ResidentsPanel portraits
	_refresh_residents_ui()
	_show_next_visitor()

func _on_reject_pressed() -> void:
	if not current_visitor or not game_manager:
		return
	game_manager.reject_visitor(current_visitor)
	_show_next_visitor()

# -------------------------
# SLEEPING
# -------------------------
func _show_sleep_notifications() -> void:
	if not game_manager:
		return
	if not game_manager.has_method("get_notifications"):
		return

	var msgs: Array = game_manager.get_notifications()

	if msgs.is_empty():
		sleep_notifications_label.text = "You sleep peacefully..."
	else:
		sleep_notifications_label.text = "\n".join(msgs)

	# Clear after showing so next day starts clean
	if game_manager.has_method("clear_notifications"):
		game_manager.clear_notifications()


# ---------------------------------------------------------
# NEWS
# ---------------------------------------------------------


func _apply_daily_zombie_clue(day_num: int) -> void:
		# Day 0: no clue yet
	if day_num == 0:
		return

	# day_num starts at 1 → turn into index 0,1,2,...
	var idx: int = clamp(day_num - 1, 0, zombie_clues.size() - 1)

	if idx < 0 or idx >= zombie_clues.size():
		return

	var clue: Dictionary = zombie_clues[idx]
	var feature_name: String = clue["feature"]
	var text: String = clue["text"]

	# Unlock this feature for inspection
	if not unlocked_features.has(feature_name):
		unlocked_features.append(feature_name)

	if not unlocked_features.has(feature_name):
		unlocked_features.append(feature_name)
		# Make sure the buttons reflect the new intel
		_update_inspection_buttons()

	# Append clue to the Morning News rich text
	morning_news_text.text += "\n\nNew Intel: " + text
	_update_morning_example_image(day_num)


func _update_morning_example_image(day_num: int) -> void:
	if day_num == 0:
		morning_news_example_image.visible = false
		return

	var idx: int = day_num - 1

	if idx >= 0 and idx < morning_example_textures.size():
		var tex: Texture2D = morning_example_textures[idx]
		if tex != null:
			morning_news_example_image.texture = tex
			morning_news_example_image.visible = true
		else:
			morning_news_example_image.visible = false
	else:
		morning_news_example_image.visible = false



# ---------------------------------------------------------
# QUIZ
# ---------------------------------------------------------

func _show_next_quiz_question() -> void:
	if not quiz_manager:
		return

	if quiz_manager.has_method("get_next_question"):
		current_question = quiz_manager.get_next_question()
	elif quiz_manager.has_method("get_question"):
		current_question = quiz_manager.get_question()
	else:
		current_question = {}

	if current_question.is_empty():
		next_phase_button.disabled = false
		return

	var qtext  = str(current_question.get("question", ""))
	var choices = current_question.get("choices", [])

	question_label.text = qtext

	var buttons = [answer_a_button, answer_b_button, answer_c_button, answer_d_button]

	for i in range(buttons.size()):
		if i < choices.size():
			buttons[i].visible = true
			buttons[i].disabled = false
			buttons[i].text = str(choices[i])
		else:
			buttons[i].visible = false
			buttons[i].disabled = true

func _on_quiz_finished() -> void:
	print("UI.gd: Quiz finished.")
	quiz_finished = true

	# Disable answer buttons so the player can't keep answering
	if answer_a_button:
		answer_a_button.disabled = true
	if answer_b_button:
		answer_b_button.disabled = true
	if answer_c_button:
		answer_c_button.disabled = true
	if answer_d_button:
		answer_d_button.disabled = true

	# Update the Next Phase button so it's clear what to do
	if next_phase_button:
		next_phase_button.text = "Finish quiz"
		next_phase_button.disabled = false

	# HUD message
	if game_manager and game_manager.has_method("get_tokens"):
		_update_hud("Quiz finished. You now have %d tokens. Press 'Finish quiz' to inspect residents." % game_manager.get_tokens())
	else:
		_update_hud("Quiz finished. Press 'Finish quiz' to inspect residents.")

	update_ui()


func _handle_answer(choice_index: int) -> void:
	if current_phase != Phase.QUIZ:
		return

	if quiz_finished:
		print("_handle_answer: quiz already finished, ignoring answer buttons.")
		return

	if not game_manager:
		push_error("UI.gd: game_manager is null in _handle_answer!")
		return

	print("--- _handle_answer ---")
	print("Using GameManager.answer_quiz_question")
	print("choice_index = ", choice_index)

	var correct: bool = false
	if game_manager.has_method("answer_quiz_question"):
		correct = game_manager.answer_quiz_question(choice_index)
	else:
		push_error("UI.gd: GameManager has no method 'answer_quiz_question'!")
		return

	print("choice_index = ", choice_index, ", correct = ", correct)
	print("Tokens after answer (from C++): ", game_manager.get_tokens())

	if correct:
		_update_hud("Correct!")
	else:
		_update_hud("Wrong.")

	quiz_questions_answered += 1

	var more := false
	if game_manager.has_method("quiz_has_more_questions"):
		more = game_manager.quiz_has_more_questions()

	if more:
		_show_next_quiz_question()
	else:
		_on_quiz_finished()

	update_ui()

func _on_answer_a_pressed() -> void:
	_handle_answer(0)

func _on_answer_b_pressed() -> void:
	_handle_answer(1)

func _on_answer_c_pressed() -> void:
	_handle_answer(2)

func _on_answer_d_pressed() -> void:
	_handle_answer(3)

# ---------------------------------------------------------
# RESIDENTS / INSPECTION
# ---------------------------------------------------------

@onready var resident_entries := [
	$ResidentsPanel/ScrollContainer/ResidentsList/ResidentEntry0,
	$ResidentsPanel/ScrollContainer/ResidentsList/ResidentEntry1,
	$ResidentsPanel/ScrollContainer/ResidentsList/ResidentEntry2,
	$ResidentsPanel/ScrollContainer/ResidentsList/ResidentEntry3,
	$ResidentsPanel/ScrollContainer/ResidentsList/ResidentEntry4,

]

@onready var resident_portraits := [
	$ResidentsPanel/ScrollContainer/ResidentsList/ResidentEntry0/Portrait0,
	$ResidentsPanel/ScrollContainer/ResidentsList/ResidentEntry1/Portrait1,
	$ResidentsPanel/ScrollContainer/ResidentsList/ResidentEntry2/Portrait2,
	$ResidentsPanel/ScrollContainer/ResidentsList/ResidentEntry3/Portrait3,
	$ResidentsPanel/ScrollContainer/ResidentsList/ResidentEntry4/Portrait4,

]

@onready var inspect_resident_buttons := [
	$ResidentsPanel/ScrollContainer/ResidentsList/ResidentEntry0/ButtonsRow0/InspectResident0Button,
	$ResidentsPanel/ScrollContainer/ResidentsList/ResidentEntry1/ButtonsRow1/InspectResident1Button,
	$ResidentsPanel/ScrollContainer/ResidentsList/ResidentEntry2/ButtonsRow2/InspectResident2Button,
	$ResidentsPanel/ScrollContainer/ResidentsList/ResidentEntry3/ButtonsRow3/InspectResident3Button,
	$ResidentsPanel/ScrollContainer/ResidentsList/ResidentEntry4/ButtonsRow4/InspectResident4Button,

]

@onready var kill_resident_buttons := [
	$ResidentsPanel/ScrollContainer/ResidentsList/ResidentEntry0/ButtonsRow0/KillResident0Button,
	$ResidentsPanel/ScrollContainer/ResidentsList/ResidentEntry1/ButtonsRow1/KillResident1Button,
	$ResidentsPanel/ScrollContainer/ResidentsList/ResidentEntry2/ButtonsRow2/KillResident2Button,
	$ResidentsPanel/ScrollContainer/ResidentsList/ResidentEntry3/ButtonsRow3/KillResident3Button,
	$ResidentsPanel/ScrollContainer/ResidentsList/ResidentEntry4/ButtonsRow4/KillResident4Button,

]

func _refresh_residents_ui() -> void:
	if not game_manager:
		return

	var count := 0
	if game_manager.has_method("get_resident_count"):
		count = game_manager.get_resident_count()

	var max_slots := resident_entries.size()
	print("--- _refresh_residents_ui --- count =", count, " max_slots =", max_slots)

	# Optional: ask C++ to dump its internal state
	if game_manager.has_method("debug_print_residents"):
		game_manager.debug_print_residents()

	for i in range(max_slots):
		var entry    = resident_entries[i]
		var portrait = resident_portraits[i]

		if i >= count:
			entry.visible = false
			portrait.texture = null
			continue

		entry.visible = true
		portrait.texture = null

		var c: Node = null
		if game_manager.has_method("get_resident_character"):
			c = game_manager.get_resident_character(i)

		if c == null or not is_instance_valid(c):
			print("  slot", i, "→ no valid character from GameManager")
			entry.visible = false
			portrait.texture = null
			continue

		# Try to get the 'Portrait' Sprite2D child from the character
		var sprite: Sprite2D = null
		if c.has_node("Portrait"):
			sprite = c.get_node("Portrait") as Sprite2D

		if sprite and sprite.texture:
			portrait.texture = sprite.texture
			print("  slot", i, "→ portrait set from", sprite)
		else:
			print("  slot", i, "→ character has no portrait texture")
			portrait.texture = null









func _on_inspect_resident_pressed(index: int) -> void:
	if not game_manager:
		return

	var tokens := 0
	if game_manager.has_method("get_tokens"):
		tokens = game_manager.get_tokens()

	if tokens <= 0:
		_update_hud("No tokens left to inspect.")
		return

	var is_zombie := false
	if game_manager.has_method("inspect_resident"):
		is_zombie = game_manager.inspect_resident(index, 1)

	update_ui()

	var result_text := ""
	if is_zombie:
		result_text = "Resident %d IS a zombie." % index
	else:
		result_text = "Resident %d seems HUMAN." % index
	_update_hud(result_text)

	inspected_resident_index = index
	inspected_character = null

	if game_manager.has_method("get_resident_character"):
		inspected_character = game_manager.get_resident_character(index)
		print("Inspect: got character for index", index, "->", inspected_character)
	else:
		print("GameManager has no get_resident_character")

	inspect_parts_panel.visible = true
	inspect_image.texture = null
	inspect_image.visible = true


func _inspect_resident_index(index: int) -> void:
	if not game_manager:
		return

	print("--- _inspect_resident_index --- index =", index)

	# 1) Check that this slot actually has a living resident
	if game_manager.has_method("get_resident_count"):
		var count = game_manager.get_resident_count()
		print("  resident_count =", count)
		if index < 0 or index >= count:
			_update_hud("There is no resident in that slot.")
			return

	# 2) Get the character for this alive index
	inspected_character = null
	if game_manager.has_method("get_resident_character"):
		inspected_character = game_manager.get_resident_character(index)
		print("  got character:", inspected_character)
	else:
		print("GameManager has no get_resident_character")
		return

	# 3) If null or freed, bail out
	if inspected_character == null or not is_instance_valid(inspected_character):
		_update_hud("There is no resident in that slot anymore.")
		return

	# 4) Store index, switch to InspectPartsPanel
	inspected_resident_index = index
	current_inspection_resident_index = index

	residents_panel.visible = false
	inspect_parts_panel.visible = true

	inspect_image.texture = null
	inspect_image.visible = true

	# Update which body-part buttons are enabled based on unlocked_features
	_update_inspection_buttons()

	_update_hud("Inspecting resident %d." % index)

	# Optional debug from C++
	if game_manager.has_method("debug_print_residents"):
		game_manager.debug_print_residents()






func _on_InspectResident0Button_pressed() -> void:
	_inspect_resident_index(0)

func _on_InspectResident1Button_pressed() -> void:
	_inspect_resident_index(1)

func _on_InspectResident2Button_pressed() -> void:
	_inspect_resident_index(2)

func _on_InspectResident3Button_pressed() -> void:
	_inspect_resident_index(3)

func _on_InspectResident4Button_pressed() -> void:
	_inspect_resident_index(4)





func _on_kill_resident_pressed(index: int) -> void:
	if not game_manager:
		return

	if game_manager.has_method("decide_resident_fate"):
		game_manager.decide_resident_fate(index, true)

	_refresh_residents_ui()
	_update_hud("Resident %d removed." % index)

# ---------------------------------------------------------
# NEXT PHASE BUTTON
# ---------------------------------------------------------

func _on_next_phase_pressed() -> void:
	print("_on_next_phase_pressed: current_phase =", current_phase, " quiz_finished =", quiz_finished)

	match current_phase:
		Phase.INTRO:
			# From intro → how-to-play screen
			enter_how_to_play_phase()

		Phase.HOW_TO_PLAY:
			# Click through lines of tutorial text
			if howto_index < howto_lines.size() - 1:
				howto_index += 1
				_update_howto_text()
			else:
				# Finished all tutorial lines → go to difficulty selection
				_show_difficulty_select()
				_update_hud("Select a difficulty to start.")

		Phase.DIFFICULTY_SELECT:
			print("NextPhase: still at difficulty select, nothing to do.")

		Phase.NIGHT:
			_show_manager_notifications()
			enter_sleeping_phase()

		Phase.SLEEPING:
			
			sleeping_panel.visible = false
			enter_morning_news_phase()

		Phase.MORNING_NEWS:
			enter_quiz_phase()

		Phase.QUIZ:
			if quiz_finished:
				current_phase = Phase.INSPECTION
				enter_inspection_phase()

				# 🔥 Sync portraits with *current* residents
				_refresh_residents_ui()

		Phase.INSPECTION:
			print("NextPhase: from INSPECTION → NIGHT")
			enter_night_phase()

		Phase.GAME_OVER:
			restart_to_difficulty()

		Phase.VICTORY:
			restart_to_difficulty()


func _on_answer_d_button_pressed() -> void:
	_handle_answer(3)


func _on_answer_c_button_pressed() -> void:
	_handle_answer(2)


func _on_answer_b_button_pressed() -> void:
	_handle_answer(1)



func _on_answer_a_button_pressed() -> void:
	_handle_answer(0)


 


func _on_kill_resident_0_button_pressed() -> void:
	_kill_resident_at_index(0)



 


func _on_kill_resident_1_button_pressed() -> void:
	_kill_resident_at_index(1)



 


func _on_kill_resident_2_button_pressed() -> void:
	_kill_resident_at_index(2)





func _on_kill_resident_3_button_pressed() -> void:
	_kill_resident_at_index(3)




func _on_kill_resident_4_button_pressed() -> void:
	_kill_resident_at_index(4)




func _on_back_button_pressed() -> void:
	inspection_panel.visible = false
	residents_panel.visible  = true
	current_inspection_resident_index = -1
