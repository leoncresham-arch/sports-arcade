extends Node3D
## Sports Arcade — original, asset-free 3D football prototype.
## Controls: WASD/arrow keys move, Space shoots, Shift sprints, P pauses, R resets.

const FIELD_X := 46.0
const FIELD_Z := 28.0
const MATCH_SECONDS := 90.0

var player: CharacterBody3D
var ball: RigidBody3D
var opponent: CharacterBody3D
var keeper: CharacterBody3D
var hud_label: Label
var score_label: Label
var message_label: Label
var time_left := MATCH_SECONDS
var home_score := 0
var away_score := 0
var match_running := true
var match_over := false
var kick_cooldown := 0.0
var stamina := 100.0
var camera_pivot: Node3D

func _ready() -> void:
    _make_world()
    _make_pitch()
    _make_goals()
    _make_players()
    _make_ball()
    _make_camera()
    _make_hud()
    _update_hud()

func _process(delta: float) -> void:
    if match_running and not match_over:
        time_left = maxf(0.0, time_left - delta)
        kick_cooldown = maxf(0.0, kick_cooldown - delta)
        if time_left <= 0.0:
            match_over = true
            match_running = false
            message_label.text = "FULL TIME  •  Press R to play again"
            message_label.visible = true
        _update_hud()
    if Input.is_action_just_pressed("pause_game") and not match_over:
        match_running = not match_running
        message_label.text = "PAUSED  •  Press P to resume" if not match_running else ""
        message_label.visible = not match_running
    if Input.is_action_just_pressed("reset_game"):
        _reset_match()

func _physics_process(delta: float) -> void:
    if not match_running or match_over:
        player.velocity = Vector3.ZERO
        player.move_and_slide()
        opponent.velocity = Vector3.ZERO
        opponent.move_and_slide()
        return

    var input_2d := Input.get_vector("move_left", "move_right", "move_forward", "move_back")
    var direction := Vector3(input_2d.x, 0.0, input_2d.y)
    if direction.length() > 1.0:
        direction = direction.normalized()

    var sprinting := Input.is_action_pressed("sprint") and stamina > 0.0 and direction.length() > 0.1
    var speed := 7.0 if sprinting else 4.8
    if sprinting:
        stamina = maxf(0.0, stamina - 27.0 * delta)
    else:
        stamina = minf(100.0, stamina + 15.0 * delta)

    player.velocity.x = direction.x * speed
    player.velocity.z = direction.z * speed
    player.velocity.y -= 24.0 * delta
    player.move_and_slide()
    player.global_position.x = clampf(player.global_position.x, -FIELD_X / 2.0 + 1.0, FIELD_X / 2.0 - 1.0)
    player.global_position.z = clampf(player.global_position.z, -FIELD_Z / 2.0 + 1.0, FIELD_Z / 2.0 - 1.0)

    # Simple opponent AI: chases the ball, then attacks the left-hand goal.
    var to_ball := ball.global_position - opponent.global_position
    to_ball.y = 0.0
    var ai_dir := to_ball.normalized()
    if to_ball.length() > 1.35:
        opponent.velocity.x = ai_dir.x * 3.0
        opponent.velocity.z = ai_dir.z * 3.0
    else:
        opponent.velocity.x = -3.2
        opponent.velocity.z = clampf(-ball.global_position.z * 0.5, -1.3, 1.3)
        if kick_cooldown <= 0.0:
            ball.apply_central_impulse(Vector3(-8.0, 1.1, -ball.global_position.z * 0.18))
            kick_cooldown = 1.1
    opponent.velocity.y -= 24.0 * delta
    opponent.move_and_slide()
    opponent.global_position.x = clampf(opponent.global_position.x, -FIELD_X / 2.0 + 1.0, FIELD_X / 2.0 - 1.0)
    opponent.global_position.z = clampf(opponent.global_position.z, -FIELD_Z / 2.0 + 1.0, FIELD_Z / 2.0 - 1.0)

    # Keeper tracks the ball across a limited section in front of its goal.
    keeper.global_position.z = move_toward(keeper.global_position.z, clampf(ball.global_position.z, -4.3, 4.3), 4.0 * delta)

    if Input.is_action_just_pressed("shoot"):
        _kick_ball()
    _check_goal()
    _update_hud()

func _make_world() -> void:
    var world_env := WorldEnvironment.new()
    var env := Environment.new()
    env.background_mode = Environment.BG_COLOR
    env.background_color = Color(0.035, 0.075, 0.055)
    env.ambient_light_source = Environment.AMBIENT_SOURCE_COLOR
    env.ambient_light_color = Color(0.7, 0.82, 0.72)
    env.ambient_light_energy = 0.8
    world_env.environment = env
    add_child(world_env)

    var sun := DirectionalLight3D.new()
    sun.rotation_degrees = Vector3(-54, -25, 0)
    sun.light_energy = 1.45
    sun.shadow_enabled = true
    add_child(sun)

func _make_pitch() -> void:
    var floor_body := StaticBody3D.new()
    floor_body.name = "Pitch"
    add_child(floor_body)
    var floor_mesh := MeshInstance3D.new()
    var plane := BoxMesh.new()
    plane.size = Vector3(FIELD_X, 0.4, FIELD_Z)
    floor_mesh.mesh = plane
    floor_mesh.position.y = -0.25
    var turf := StandardMaterial3D.new()
    turf.albedo_color = Color(0.07, 0.38, 0.18)
    turf.roughness = 0.95
    floor_mesh.material_override = turf
    floor_body.add_child(floor_mesh)
    var floor_collision := CollisionShape3D.new()
    var floor_shape := BoxShape3D.new()
    floor_shape.size = Vector3(FIELD_X, 0.4, FIELD_Z)
    floor_collision.shape = floor_shape
    floor_collision.position.y = -0.25
    floor_body.add_child(floor_collision)

    # Alternating turf bands and white pitch markings.
    for i in range(10):
        var stripe := MeshInstance3D.new()
        var stripe_mesh := BoxMesh.new()
        stripe_mesh.size = Vector3(FIELD_X / 10.0, 0.012, FIELD_Z)
        stripe.mesh = stripe_mesh
        stripe.position = Vector3(-FIELD_X / 2.0 + (i + 0.5) * FIELD_X / 10.0, -0.035, 0)
        var stripe_mat := StandardMaterial3D.new()
        stripe_mat.albedo_color = Color(0.085, 0.42, 0.21) if i % 2 == 0 else Color(0.07, 0.37, 0.18)
        stripe.material_override = stripe_mat
        add_child(stripe)

    _line(Vector3(0, 0.01, 0), Vector3(0, 0.01, FIELD_Z), 0.09)
    _line(Vector3(-FIELD_X/2, 0.01, -FIELD_Z/2), Vector3(FIELD_X/2, 0.01, -FIELD_Z/2), 0.09)
    _line(Vector3(-FIELD_X/2, 0.01, FIELD_Z/2), Vector3(FIELD_X/2, 0.01, FIELD_Z/2), 0.09)
    _line(Vector3(-FIELD_X/2, 0.01, -FIELD_Z/2), Vector3(-FIELD_X/2, 0.01, FIELD_Z/2), 0.09)
    _line(Vector3(FIELD_X/2, 0.01, -FIELD_Z/2), Vector3(FIELD_X/2, 0.01, FIELD_Z/2), 0.09)
    _line(Vector3(-FIELD_X/2, 0.01, -FIELD_Z/2), Vector3(FIELD_X/2, 0.01, -FIELD_Z/2), 0.09)
    _line(Vector3(-FIELD_X/2, 0.01, FIELD_Z/2), Vector3(FIELD_X/2, 0.01, FIELD_Z/2), 0.09)
    _make_circle(Vector3.ZERO, 3.2, 0.06)
    _line(Vector3(-FIELD_X/2, 0.01, -6), Vector3(-FIELD_X/2+5, 0.01, -6), 0.07)
    _line(Vector3(-FIELD_X/2+5, 0.01, -6), Vector3(-FIELD_X/2+5, 0.01, 6), 0.07)
    _line(Vector3(-FIELD_X/2+5, 0.01, 6), Vector3(-FIELD_X/2, 0.01, 6), 0.07)
    _line(Vector3(FIELD_X/2, 0.01, -6), Vector3(FIELD_X/2-5, 0.01, -6), 0.07)
    _line(Vector3(FIELD_X/2-5, 0.01, -6), Vector3(FIELD_X/2-5, 0.01, 6), 0.07)
    _line(Vector3(FIELD_X/2-5, 0.01, 6), Vector3(FIELD_X/2, 0.01, 6), 0.07)

func _line(a: Vector3, b: Vector3, thickness: float) -> void:
    var mesh_instance := MeshInstance3D.new()
    var box := BoxMesh.new()
    box.size = Vector3(maxf(thickness, absf(a.x - b.x)), 0.025, maxf(thickness, absf(a.z - b.z)))
    mesh_instance.mesh = box
    mesh_instance.position = (a + b) * 0.5
    mesh_instance.position.y = 0.015
    var mat := StandardMaterial3D.new()
    mat.albedo_color = Color(0.88, 0.96, 0.88)
    mesh_instance.material_override = mat
    add_child(mesh_instance)

func _make_circle(center: Vector3, radius: float, thickness: float) -> void:
    for i in range(48):
        var a1 := TAU * float(i) / 48.0
        var a2 := TAU * float(i + 1) / 48.0
        _line(center + Vector3(cos(a1) * radius, 0.01, sin(a1) * radius), center + Vector3(cos(a2) * radius, 0.01, sin(a2) * radius), thickness)

func _make_goals() -> void:
    _goal_frame(-FIELD_X / 2.0 - 1.0)
    _goal_frame(FIELD_X / 2.0 + 1.0)

func _goal_frame(x: float) -> void:
    var mat := StandardMaterial3D.new()
    mat.albedo_color = Color(0.92, 0.97, 0.94)
    for pos in [Vector3(x, 1.1, -4.2), Vector3(x, 1.1, 4.2), Vector3(x, 2.2, 0)]:
        var post := MeshInstance3D.new()
        var mesh := BoxMesh.new()
        mesh.size = Vector3(0.18, 2.2 if absf(pos.z) < 0.1 else 0.18, 0.18)
        post.mesh = mesh
        post.position = pos
        post.material_override = mat
        add_child(post)

func _make_players() -> void:
    player = _character("ARCADE FC", Vector3(8, 0.9, 0), Color(0.72, 1.0, 0.22))
    opponent = _character("CPU UNITED", Vector3(-8, 0.9, 0), Color(0.95, 0.24, 0.18))
    keeper = _character("KEEPER", Vector3(-FIELD_X/2.0 + 1.7, 0.9, 0), Color(0.7, 0.48, 1.0), 0.55)

func _character(label: String, pos: Vector3, color: Color, scale_factor: float = 1.0) -> CharacterBody3D:
    var body := CharacterBody3D.new()
    body.name = label.replace(" ", "")
    body.position = pos
    add_child(body)
    var shape_node := CollisionShape3D.new()
    var capsule := CapsuleShape3D.new()
    capsule.radius = 0.42 * scale_factor
    capsule.height = 1.8 * scale_factor
    shape_node.shape = capsule
    body.add_child(shape_node)
    var mesh_node := MeshInstance3D.new()
    var capsule_mesh := CapsuleMesh.new()
    capsule_mesh.radius = 0.42 * scale_factor
    capsule_mesh.height = 1.8 * scale_factor
    mesh_node.mesh = capsule_mesh
    var material := StandardMaterial3D.new()
    material.albedo_color = color
    material.roughness = 0.72
    mesh_node.material_override = material
    body.add_child(mesh_node)
    var head := MeshInstance3D.new()
    var sphere := SphereMesh.new()
    sphere.radius = 0.23 * scale_factor
    sphere.height = 0.46 * scale_factor
    head.mesh = sphere
    head.position.y = 0.95 * scale_factor
    var skin := StandardMaterial3D.new()
    skin.albedo_color = Color(0.86, 0.68, 0.51)
    head.material_override = skin
    body.add_child(head)
    return body

func _make_ball() -> void:
    ball = RigidBody3D.new()
    ball.name = "Ball"
    ball.position = Vector3(0, 0.55, 0)
    ball.mass = 0.45
    ball.linear_damp = 0.75
    ball.angular_damp = 0.6
    add_child(ball)
    var shape_node := CollisionShape3D.new()
    var sphere_shape := SphereShape3D.new()
    sphere_shape.radius = 0.32
    shape_node.shape = sphere_shape
    ball.add_child(shape_node)
    var mesh_node := MeshInstance3D.new()
    var sphere_mesh := SphereMesh.new()
    sphere_mesh.radius = 0.32
    sphere_mesh.height = 0.64
    mesh_node.mesh = sphere_mesh
    var material := StandardMaterial3D.new()
    material.albedo_color = Color(0.96, 0.98, 0.92)
    material.roughness = 0.45
    mesh_node.material_override = material
    ball.add_child(mesh_node)

func _make_camera() -> void:
    camera_pivot = Node3D.new()
    camera_pivot.name = "CameraRig"
    add_child(camera_pivot)
    var camera := Camera3D.new()
    camera.position = Vector3(0, 28, 25)
    camera.rotation_degrees = Vector3(-48, 0, 0)
    camera.fov = 52
    camera.current = true
    camera_pivot.add_child(camera)

func _make_hud() -> void:
    var layer := CanvasLayer.new()
    add_child(layer)
    var root := Control.new()
    root.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
    layer.add_child(root)

    score_label = Label.new()
    score_label.position = Vector2(0, 18)
    score_label.size = Vector2(1280, 44)
    score_label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
    score_label.add_theme_font_size_override("font_size", 28)
    score_label.add_theme_color_override("font_color", Color(0.82, 1.0, 0.35))
    root.add_child(score_label)

    hud_label = Label.new()
    hud_label.position = Vector2(22, 20)
    hud_label.size = Vector2(300, 90)
    hud_label.add_theme_font_size_override("font_size", 17)
    root.add_child(hud_label)

    message_label = Label.new()
    message_label.set_anchors_and_offsets_preset(Control.PRESET_CENTER)
    message_label.position += Vector2(-300, -45)
    message_label.size = Vector2(600, 90)
    message_label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
    message_label.vertical_alignment = VERTICAL_ALIGNMENT_CENTER
    message_label.add_theme_font_size_override("font_size", 28)
    message_label.add_theme_color_override("font_color", Color(0.82, 1.0, 0.35))
    message_label.text = ""
    message_label.visible = false
    root.add_child(message_label)

    var controls := Label.new()
    controls.set_anchors_and_offsets_preset(Control.PRESET_BOTTOM_WIDE)
    controls.position = Vector2(20, -38)
    controls.size = Vector2(1240, 26)
    controls.text = "WASD / ARROWS  Move     SHIFT  Sprint     SPACE  Shoot     P  Pause     R  Reset"
    controls.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
    controls.add_theme_color_override("font_color", Color(0.9, 0.95, 0.9))
    root.add_child(controls)

func _kick_ball() -> void:
    if kick_cooldown > 0.0:
        return
    if player.global_position.distance_to(ball.global_position) > 2.8:
        message_label.text = "Get closer to the ball to shoot"
        message_label.visible = true
        get_tree().create_timer(1.2).timeout.connect(func(): message_label.visible = false if match_running else message_label.visible)
        return
    var aim := Vector3(-1.0, 0.08, 0.0)
    if Input.is_action_pressed("move_left"):
        aim.x = -1.0
    elif Input.is_action_pressed("move_right"):
        aim.x = 1.0
    if Input.is_action_pressed("move_forward"):
        aim.z = -0.65
    elif Input.is_action_pressed("move_back"):
        aim.z = 0.65
    ball.apply_central_impulse(aim.normalized() * 10.5)
    kick_cooldown = 0.45

func _check_goal() -> void:
    if absf(ball.global_position.z) > 4.2:
        return
    if ball.global_position.x < -FIELD_X / 2.0 - 0.1:
        away_score += 1
        _restart_positions()
    elif ball.global_position.x > FIELD_X / 2.0 + 0.1:
        home_score += 1
        _restart_positions()

func _restart_positions() -> void:
    ball.linear_velocity = Vector3.ZERO
    ball.angular_velocity = Vector3.ZERO
    ball.global_position = Vector3(0, 0.55, 0)
    player.global_position = Vector3(8, 0.9, 0)
    opponent.global_position = Vector3(-8, 0.9, 0)
    message_label.text = "GOAL!"
    message_label.visible = true
    get_tree().create_timer(1.4).timeout.connect(func(): message_label.visible = false if match_running else message_label.visible)
    _update_hud()

func _update_hud() -> void:
    if not is_instance_valid(score_label):
        return
    score_label.text = "ARCADE FC     %d   —   %d     CPU UNITED     |     %02d:%02d" % [home_score, away_score, int(time_left) / 60, int(time_left) % 60]
    hud_label.text = "STAMINA  %d%%\n%s" % [int(stamina), "MATCH LIVE" if match_running else ("FULL TIME" if match_over else "PAUSED")]

func _reset_match() -> void:
    home_score = 0
    away_score = 0
    time_left = MATCH_SECONDS
    stamina = 100.0
    match_over = false
    match_running = true
    message_label.visible = false
    _restart_positions()
    _update_hud()

func _unhandled_key_input(event: InputEvent) -> void:
    if event is InputEventKey and event.pressed and not event.echo:
        if event.keycode == KEY_R:
            _reset_match()

func _enter_tree() -> void:
    _add_input("move_left", [KEY_A, KEY_LEFT])
    _add_input("move_right", [KEY_D, KEY_RIGHT])
    _add_input("move_forward", [KEY_W, KEY_UP])
    _add_input("move_back", [KEY_S, KEY_DOWN])
    _add_input("shoot", [KEY_SPACE])
    _add_input("sprint", [KEY_SHIFT])
    _add_input("pause_game", [KEY_P])
    _add_input("reset_game", [KEY_R])

func _add_input(action: String, keys: Array) -> void:
    if not InputMap.has_action(action):
        InputMap.add_action(action)
    for code in keys:
        var event := InputEventKey.new()
        event.keycode = code
        InputMap.action_add_event(action, event)
