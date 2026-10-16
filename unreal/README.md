# Sports Arcade — Unreal Engine PC edition

This directory is a **PC-first Unreal Engine 5.5 C++ scaffold**, not a playable game or a finished visual prototype. It is intentionally separate from the early Godot and browser experiments.

## What exists
- `SportsArcade.uproject`: Unreal 5.5 project descriptor.
- `Source/`: game/editor targets, module, a minimal game mode, and a locomotion/camera character.
- `Config/`: default engine settings and WASD / arrow input mapping.

## Physics football foundation

- `SportsArcadeBall`: configurable 22 cm diameter / 0.43 kg physics sphere, directional impulse interface, reset, and placeholder sphere visual.
- The [native review-scene script](Tools/Editor/setup_visual_review.py) now places the ball at the centre spot.
- [Ball physics integration notes](docs/ball-physics.md) detail unfinished collisions, spin, dribbling, and genuine Unreal testing. This code is **not compiled**.

## PC rendering baseline (source-only)

- [PC renderer baseline](docs/pc-renderer-baseline.md) now defines Temporal Super Resolution, Lumen GI/reflections, software distance fields and virtual shadow maps as a **starting** high-end graphics configuration.
- Hardware ray tracing is not forced. Performance and visuals still require real native GPU profiling.
- This changes the potential renderer features; it does **not** add professional player or stadium art, nor prove graphics quality.

## Daylight stadium lighting foundation

- `SportsArcadeLightingRig`: configurable C++ lighting actor combining the directional sun, skylight, sky atmosphere and subtle fog.
- The [native visual-review setup script](Tools/Editor/setup_visual_review.py) now places this reusable actor, providing a consistent reference for judging realistic character, turf and kit materials later.
- This source-level lighting setup has not been rendered or validated in Unreal.

## Connected native broadcast view

- `SportsArcadeGameMode::BeginPlay` now searches for a placed `SportsArcadeBroadcastCamera`, chooses it as the first local player's view target, and optionally follows the reference football.
- If a level does not contain this camera actor, the usual pawn camera remains in use.
- These functions are source-level additions, with structural tests only; they have **not** been compiled or play-tested in Unreal.

## Unreal 5 editor API alignment

- The native visual-review bootstrap now uses `LevelEditorSubsystem.new_level` and `save_current_level` instead of the older `EditorLevelLibrary` map methods.
- Expanded simulated-editor tests to cover unavailable level-editor subsystem behavior.
- This is an API alignment based on Unreal documentation, **not evidence that the script ran in Unreal 5.5**.

## Scene bootstrap regression tests

- `tests/test_preview_bootstrap.py` now exercises the Unreal Editor setup script with a **simulated editor**. It verifies actors, pitch-end goal coordinates, camera/ball placement, refusal to overwrite an existing level and missing-class failures.
- The broadcast camera no longer forces its world position in the C++ constructor; the authored level defines initial placement.
- GitHub CI validation is still **not** a real UE compile or scene render.

## Authentic Unreal screenshot preparation

- Added [native render capture helper](Tools/Editor/capture_visual_review.py) and [review procedure](docs/authentic-render-capture.md).
- It requests a high-resolution screenshot from the **actual Unreal Editor camera**, refuses an explicit failed request, and writes an *unverified request log* only when Unreal does not reject the request. No screenshot has been generated, inspected or uploaded yet.
- CI includes simulated-editor tests for approval gating, camera lookup and capture-request metadata. No native Unreal editor is connected.

## Native visual review scene preparation

- [Preview build instructions](docs/visual-preview-build.md) and [Unreal Editor Python setup](Tools/Editor/setup_visual_review.py) now describe an initial level with pitch, goals, broadcast camera and sunlight/sky.
- The Python script is syntax-checked in CI, **not executed in Unreal**; an actual Unreal workstation and licensed professional 3D art remain necessary to produce a realistic preview.

## Asset-import safety maintenance

- Added checks for path traversal, links outside a private asset staging folder, and unsupported file extensions before Unreal import.
- Updated [Unreal Python console instructions](docs/licensed-asset-intake.md).
- These tests run without UE; no real artist assets have been imported or previewed.

## Licensed high-fidelity asset intake

- [Asset intake instructions](docs/licensed-asset-intake.md) document a reviewed-license manifest, fail-closed Python validation, and a local Unreal Editor import script.
- The [manifest template](asset_pipeline/manifest.example.json) contains **no assets** and intentionally fails until actual author, license, source, game-use rights and file locations are verified.
- Unreal Editor import has **not been executed**; no rigged player, animation, turf material or stadium model has been acquired or rendered.

## Free automated checks

Every change under `unreal/` triggers [Unreal source scaffold checks](../.github/workflows/unreal-source-check.yml) on GitHub Actions. These zero-cost Python unit tests check the project descriptor, module/targets, header/source pairing, generated-header ordering, camera interfaces, and animation/input wiring.

Run locally with `python3 -m unittest discover -s unreal/tests -v` from the repository root.

**Passing these checks is not proof of successful Unreal compilation, asset integration, graphics quality, or gameplay.** A native Unreal Engine editor build and GPU render are still required.

## Character animation foundation

- `SportsArcadeAnimInstance` now exposes locomotion data (ground speed, movement direction, turn rate, acceleration, sprint/fall flags) for a future realistic footballer Animation Blueprint.
- Jog and sprint input is wired into the PC C++ character using **WASD / arrows** and **Left Shift**.
- See [rigging and animation integration](docs/character-animation-pipeline.md) for asset requirements, Unreal setup and visual acceptance tests. No licensed character meshes or animation clips have been integrated yet.

## Regulation goal reference

- `SportsArcadeGoal`: C++ scene actor for goal posts and crossbar at 7.32 m clear width / 2.44 m clear height.
- `SportsArcadePitch` now also creates 1 m corner quarter-arcs.
- [Goal and stadium layout integration](docs/goals-and-stadium-layout.md) documents pitch axes, correct placements and future production asset requirements.
- The frame uses construction cubes only; no realistic net or rendered stadium has been implemented.

## Measured football pitch foundation

- `SportsArcadePitch`: source-level 105 × 68 m pitch actor with configurable turf/paint materials, halfway line, centre circle, penalty and goal areas, penalty spot and arcs.
- [Pitch geometry and art integration](docs/pitch-production.md) explains coordinate conventions and unfinished stadium/grass work.
- This is **not** photorealistic grass or a compiled Unreal level.

## New graphics foundations

- `SportsArcadeBroadcastCamera`: Unreal camera actor intended for a televised football view. It can track a focus actor and smoothly follow play across the pitch; it is not wired into a stadium level yet.
- `Config/DefaultScalability.ini`: initial scalability guidelines without forcing maximum graphics settings onto unsupported PCs.
- [Camera integration and graphics test instructions](docs/camera-and-graphics-setup.md).

## What is *not* done
- The project has not been opened, compiled, cooked, packaged, or tested in Unreal Engine.
- No stadium, pitch, playable match, character mesh, facial material, animation rig, physics ball, keeper, networking, or photorealistic graphics have been implemented.
- The default map is Unreal's engine Entry map, which is a **placeholder**, not a football level. A skeletal mesh, animation blueprint, and authored stadium level must be imported and assigned in an editor.
- The default character camera remains a chase-camera scaffold. The new broadcast camera exists as a separate actor but is not assigned as the default view or tested in an authored stadium.

## Visual priority
The non-negotiable goal is an original premium-quality football simulation. Visual production must start from legally usable, high-quality, rigged human models, football animations, detailed stadium architecture, PBR materials, and proper lighting—not primitive objects presented as production art.

### Zero-cost asset acquisition policy
- Use assets only when the license explicitly permits the planned use and redistribution in a shipped game.
- Keep attribution/license metadata alongside every imported asset.
- Do not download game files, models, motion clips, logos, player likenesses, or other assets extracted from FIFA / EA Sports FC.
- Do not commit third-party files until they have been reviewed for compatibility and download size.

## How it will be tested later
1. On a Windows PC with Unreal Engine 5.5, open `unreal/SportsArcade.uproject`.
2. Allow Unreal to generate and build the C++ solution.
3. Resolve any compiler or engine-version issues identified by the actual build.
4. Create an authored level, import legal 3D assets, and set character mesh/animation assets.
5. Test 1080p native rendering and input behavior, then profile on intended PC hardware.

No Unreal editor or cloud GPU is available from the current iPad-only free workflow. GitHub commits are source-control milestones, not evidence of a validated game build.

## Graphics acceptance criteria
- Believable anatomy, kit and material response at broadcast camera distances.
- Smooth football-specific locomotion and turn/stop/strike transitions.
- Football-sized pitch with high-quality turf, real goal nets, believable stadium scale and crowds.
- Cinematic lighting, natural color grading, and convincing shadows.
- Stable performance on a named minimum-spec PC, measured rather than assumed.
- User approval of actual rendered footage or an interactive build before expanding gameplay scope.

## Broadcast camera placement correction

- At native BeginPlay, the review camera now uses the saved map placement to establish its tracking offset, rather than snapping back to a hard-coded position. A source regression test covers the intended contract.
- This is uncompiled UE C++ pending validation in a real 5.5 editor.

## Native football controls
- J: pass; K: shoot. The character finds a nearby football and applies an impulse in its facing direction using the existing C++ ball actor. Tuning, foot-contact animations, targeting, match rules and native UE compilation remain outstanding.
- Corrected input configuration newline formatting.

## Native endurance system (uncompiled)

- Footballer sprint input now requests sprinting, while a per-frame stamina system drains endurance when moving and sprinting, recovers otherwise, and requires recovery after exhaustion. Rates and endurance can be tuned per character class; normalized stamina is exposed for future UI/animation use.
- Implemented in native C++; source checks cannot establish compiled behavior or gameplay balance.
