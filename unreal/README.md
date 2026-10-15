# Sports Arcade — Unreal Engine PC edition

This directory is a **PC-first Unreal Engine 5.5 C++ scaffold**, not a playable game or a finished visual prototype. It is intentionally separate from the early Godot and browser experiments.

## What exists
- `SportsArcade.uproject`: Unreal 5.5 project descriptor.
- `Source/`: game/editor targets, module, a minimal game mode, and a locomotion/camera character.
- `Config/`: default engine settings and WASD / arrow input mapping.

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
