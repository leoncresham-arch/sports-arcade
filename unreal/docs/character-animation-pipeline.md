# Footballer animation integration — Unreal PC edition

**State:** C++ animation-data bridge and jog/sprint input committed to GitHub. This has not been compiled or exercised in the Unreal editor. No character model or animation assets have been imported.

## Existing code

- `USportsArcadeAnimInstance` reads the owning Character's ground speed, movement direction, yaw turn rate, acceleration, falling state and sprint state.
- `ASportsArcadeCharacter` uses WASD/arrows for test movement and **Left Shift** for sprint; default jog / sprint speeds are 420 / 650 cm/s, configurable in the game character.
- The engine-side animation data is separate from authored player assets. Source code alone will **not** produce believable human animation.

## Asset requirements: first real footballer

All third-party assets must have an identifiable license that permits the intended game use.

- A licensed, human-proportioned skeletal footballer rig, correctly scaled to Unreal centimetres (roughly 170–195 cm depending on character design).
- Mesh LODs and texture/material variants appropriate for desktop performance.
- Unique, original kit (shirt/shorts/socks/boots), with suitable skin, fabric, and hair shading.
- An animation set compatible with or legally retargetable to the chosen rig: idle, forward/side/back jog, sprint, start, braking, turn-in-place, planted kick, dribble contacts, tackle and keeper reactions.
- Clear source of animations and redistribution license; don't assume free-to-view previews imply game-redistribution rights.
- Foot-plant and ball-contact quality checks in Unreal, not just animation thumbnails.

## Unreal editor steps (when a workstation is available)

1. Import a properly licensed FBX/glTF skeletal asset; confirm skeleton, scale, materials and rig mapping.
2. Import and retarget the locomotion clips to the selected target skeleton.
3. In Unreal, create `ABP_SportsArcadePlayer`, **parent class `SportsArcadeAnimInstance`**.
4. Create 2D locomotion Blend Space using `GroundSpeed` and `MovementDirection` as parameters, with authored slow/fast clips.
5. Use `TurnRateDegrees` and `NormalizedAcceleration` for starts, stops and body turns. Implement foot locking/IK where needed.
6. Assign the mesh and `ABP_SportsArcadePlayer` to a child Blueprint of `SportsArcadeCharacter` and use that Blueprint as the level/game-mode default pawn.
7. Verify the signed direction with the actual skeleton's forward axis, then record side/back movement and transitions.
8. Profile at broadcast distance and in medium close-up to judge both performance and visual fidelity.

## Visual quality gate

No graphics preview should be described as photorealistic before the actual rendered player passes these checks:

- Anatomically credible at both broadcast and medium distances.
- Smooth jog-to-sprint, sprint-to-stop, rapid turns and lateral steps without visible foot sliding.
- Convincing weight transfer and hand/torso movement during football actions.
- Fabric, skin, hair, boots and ball materials respond naturally to stadium lighting.
- Verified in a genuine Unreal render on a specified PC, with honest frame-rate data.

## Pending blockers

- No Unreal runtime, PC GPU, or free remotely controlled workstation is connected.
- No legally sourced realistic player or motion asset has been imported.
- Native UE C++ compilation must be performed and any compiler feedback fixed before this is considered engine-tested.
