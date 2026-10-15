# Sports Arcade — Rebuild brief

## Decision
The original 2D browser game is a **throwaway experiment**. No graphics, controls, physics, characters, UI, animations, or game design from it are assumed to be part of the final product. Preserve it only as an archive until a new playable build supersedes it.

## Product target
An original, polished **3D football simulation** inspired by the fidelity and broadcast feel of premium modern football games. Safari and Chrome on iPad and desktop are target platforms. Do not copy EA/FIFA trademarks, licensed player likenesses, graphics, footage, or assets.

## Graphics-first release gates

### Gate A — visual reference and technical foundations
- Camera framing comparable to an authentic televised football match, while remaining readable on an iPad.
- Choose a proven 3D web renderer, model pipeline (glTF/GLB), animation rig, lighting workflow, and performance budgets.
- Establish legally usable original assets and consistent proportions, materials, and visual language.
- Test across iPad Safari and desktop Chrome before locking rendering features.

### Gate B — one convincing stadium scene
- Carefully modeled full-size pitch with subtle turf variations, accurate markings, stadium stands, nets, floodlights, shadows, and convincing exposure.
- Broadcast camera, replay/cinematic camera, and touch-friendly controls.
- No generic capsule-player art in the user-facing quality benchmark.

### Gate C — footballers in motion
- One hero-quality original player and keeper, plus a ball with believable materials.
- Animation state transitions: idle, jog, sprint, turn, plant, pass, shoot, tackle, goalkeeper reaction.
- Performance profiling: draw calls, textures, GPU memory, stable frame pacing.

### Gate D — match vertical slice
- One short, playable sequence with credible movement, ball interactions, and animation.
- Replace the existing public game only after the new 3D slice can be played and reviewed.
- Do not add a large team roster, career mode, or online multiplayer before visual quality and match feel meet the benchmark.

## Quality rules
- A primitive 3D scene can validate tooling but is **not** evidence the intended graphical quality has been achieved.
- Never claim FIFA/EA Sports FC parity based on scaffolding.
- Evaluate on real target hardware; offer performance quality settings.
- Favor a finished, high-fidelity small slice over many incomplete features.
