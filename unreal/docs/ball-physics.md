# Native football physics foundation

The `ASportsArcadeBall` Unreal C++ actor provides a sphere collision body with physics simulation, configurable size-5-ball diameter/mass, a directional kick impulse method, and a reset to its initial play location.

**Not compiled or tested in Unreal.** The engine-default reference sphere is not production football art. This code is not a validated football flight, dribbling or collision model.

## Intended integration
1. Compile the C++ project on a native Unreal-capable PC.
2. Place a Ball actor initially at (0, 0, 11 cm) above the pitch origin; adjust for turf collision.
3. Replace the sphere visual mesh with a licensed or original high-quality ball mesh, UVs, normal maps and PBR material.
4. Set an authored Physical Material for restitution/friction and profile Chaos collision.
5. Validate correct mass, radius, collision channels, frame-rate independence and the effect of continuous collision detection.
6. Hook `Kick(Direction, StrengthKgCmPerSec)` to actual player contact and kicking animations. No invented default shot velocity is specified.
7. Establish possession, foot IK contact, drag, spin/Magnus, goal-line rules, replays and server-authoritative multiplayer simulation as separate development milestones.

## Caveats
- The Unreal reference Sphere mesh is approximately 100 cm wide; scaling `BallRadiusCm / 50` is only correct for the engine basic shape.
- `ResetToCentre` returns to **the actor's starting location**, not necessarily world origin. The name is retained for gameplay intent; place the actor at the centre spot for kickoff.
- The ball's physics must be tested against the pitch and production goal nets inside Unreal; static source checks cannot validate the behavior.

## October 20 — ball placement and contact integrity

Player-attributed kicks now reject nonfinite player/ball positions; controlled touches also reject nonfinite physics velocity. Referee ball placement rejects nonfinite target coordinates, and resetting to the initial spawn position refuses invalid spawn coordinates. Source regression contracts are in `unreal/tests/test_ball_contact_position_integrity.py`. These guards are committed source only, not an Unreal Engine compile or physics playtest.

## October 20 — corrupted angular simulation state

Player-attributed kicks and controlled dribble touches reject nonfinite Chaos linear or angular velocity before adding impulse or updating last-touch attribution. This is a defensive simulation integrity check, not a replacement for runtime physics diagnostics. Regression contracts are in `unreal/tests/test_ball_contact_position_integrity.py`; native Unreal tests remain pending.

The general `Kick` entry point now also rejects nonfinite current linear and angular physics velocities before adding impulse, protecting callers that do not use player-attributed contact. Source regression coverage was extended accordingly.
