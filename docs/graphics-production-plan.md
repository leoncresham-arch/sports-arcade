# Sports Arcade — Free High-Fidelity Graphics Production Plan

**Status:** Asset research and production requirements only. No asset has been downloaded, imported, rendered, or tested in Unreal Engine. This is not a playable or photorealistic preview.

## Current preview
- Existing browser stadium proof-of-concept: https://leoncresham-arch.github.io/sports-arcade/3d.html
- This has primitive footballer meshes and artificial stadium geometry and **does not meet the production quality target**.
- The native Unreal project is in `unreal/` and currently has source scaffolding, not a rendered environment.

## Free or no-upfront-cost content sources (check individual asset terms before use)

1. **MetaHuman digital humans** — https://www.metahuman.com/ and https://www.metahuman.com/license
   - Candidate for hero-quality realistic faces, skin, hair, and body rigs.
   - MetaHuman is included under standard Unreal Engine licensing, subject to its terms and revenue thresholds.
   - Requires Unreal-capable workstation for authoring/integration; iPad alone cannot run the editor.
   - Football uniforms, performance animation, body variation, and LOD/performance optimization still require work.
2. **Fab free content library** — https://www.unrealengine.com/fabfreecontent
   - Search for licensed football stadium components, PBR surfaces, materials, environment props, crowd systems, and animations.
   - A free listing is not automatically suitable: confirm its exact license, permitted project use, attribution obligations, Unreal version support, and optimization profile before acquiring it.
   - Do not redistribute raw marketplace assets publicly in this source repository unless licensing explicitly permits it.
3. **Unreal Engine** — https://www.unrealengine.com/license
   - Free to begin for eligible developers, with royalties or other terms depending on usage and revenue.
   - PC-first native rendering is the primary target; game streaming is not assumed to be available at zero cost.

## First visual-review deliverable

Create and capture a single original football sequence, **not** a full match:

- Broadcast camera showing full realistic pitch, authentic goal construction, net, and part of detailed stadium.
- One footballer in a unique original kit with anatomically plausible proportions and convincing skin/hair/materials.
- Ball modeled and shaded to professional standards.
- Three transitions: walk/jog to sprint, decelerate/turn, strike ball.
- At least one wide broadcast image and one medium close-up from the same real Unreal scene, plus video or a native build showing animation.
- No synthetic mood image, engine screenshot from another project, or store asset thumbnail can be represented as a rendered Sports Arcade game preview.

## Acceptance checklist

- [ ] Rigged footballer appears human at broadcast distance and medium close-up.
- [ ] Uniform meshes/materials and footwear read as original high-quality football gear.
- [ ] Pitch shows detailed grass response under physically consistent lighting.
- [ ] Shadows contact feet and ball; no floating or interpenetrating meshes.
- [ ] Stadium architecture and scale are credible and original.
- [ ] Camera, exposure, and color grading produce convincing football-broadcast presentation.
- [ ] Animation transitions are natural, without sliding feet or robotic movement.
- [ ] Native render and GPU performance are captured on actual test hardware.
- [ ] Source license for every third-party asset is documented.
- [ ] User has reviewed genuine rendered frames and approved visual direction.

## Constraints / next engineering work

- Do not mistake a file commit for a compiled Unreal project.
- No free always-available GPU workstation or Unreal editor is currently connected.
- Existing Unreal project descriptor targets 5.5; current MetaHuman materials/documentation may have advanced to newer engine versions. Review compatibility and migration deliberately on a test machine before changing engine version.
- Until a compatible PC or accessible sponsored/free GPU environment is available, work can proceed on architecture, sourcing, licensing review, and design, but **cannot yield an honestly verified photorealistic Unreal preview**.
