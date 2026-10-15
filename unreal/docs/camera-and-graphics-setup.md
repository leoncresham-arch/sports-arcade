# Broadcast camera and graphics setup — untested Unreal 5.5 code

These source files are checked into GitHub but **have not been compiled or tested in Unreal**. They do not constitute a graphics preview.

## Integrating the broadcast camera
1. Create or open an authored football stadium level. Set the origin at the pitch centre; set the long pitch axis on X and touchlines on Y.
2. Add an instance of `SportsArcadeBroadcastCamera` to the level (class in `Source/SportsArcade/Public/`).
3. In the level Blueprint or a custom PlayerController, use `Set View Target with Blend` with the camera actor as the new view target.
4. Optionally call `SetFocusActor` on the camera with the ball actor, so it tracks lateral play smoothly.
5. Tune `CameraOffset`, `MaximumLateralTravel`, and `BroadcastFieldOfView` with actual stadium, crowd, lighting, and footballers in place.
6. Check broadcast framing at 16:9 and 16:10, including goalmouths and touchlines. Create additional cinematic cameras later for close-ups and replays.

## Performance and visuals
- Unreal scalability settings are intentionally not hard-locked to Cinematic. Run profiling on a real gaming PC before defining Low / Medium / High / Ultra hardware tiers.
- Use legal, appropriate high-resolution textures / normal maps and physically-based materials for turf, skin, kits, stadium components, and ball.
- Use rigged, properly animated player assets rather than the old web prototype's geometric characters.
- Keep gameplay and visual concerns separate so the native game can support performance settings without changing match physics.
- The browser 3D test remains archival, not the basis of the final art pipeline.

## Acceptance tests when an Unreal workstation becomes available
- C++ compilation succeeds for both Game and Editor targets.
- Authored stadium level opens without missing materials or skeletons.
- `SetFocusActor` tracks the ball without sudden camera jumps; `ClearFocusActor` recentres the shot.
- Pitch markings and proportions are accurate in broadcast view.
- Light, skin and turf tone map consistently between exterior and close-up cameras.
- Capture the native frame rate and GPU usage at a known PC specification.
