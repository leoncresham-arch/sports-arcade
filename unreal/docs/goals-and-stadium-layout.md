# Regulation goals and stadium scene layout

**Status:** C++ construction scaffold only. No Unreal compile, editor scene, net simulation, goal collision, realistic shaders, authored stadium mesh, or real graphics render exists yet.

## Coordinate plan

- Pitch origin (0,0,0), touchlines parallel to X.
- Goal lines at X = -5250 cm and +5250 cm on a standard 105 m pitch.
- Place two `SportsArcadeGoal` actors with their local goal plane X=0 coinciding with those goal lines. The frame Y axis spans the goalmouth, Z points up.
- Nominal clear width between inner post faces: **7.32 m**.
- Nominal clearance under crossbar: **2.44 m**.
- This construction reference assumes centred goals at Y=0.
- Make the stadium, nets, advertising boards and broadcast camera respect the same coordinate system.

## Visual-production requirements

- Replace the cube-based goal scaffolding with original or properly licensed smooth cylindrical/post meshes with rounded edges and realistic gloss/roughness.
- Model the net topology as an authored UV-ready mesh or simulation asset; don't substitute a solid plane or pretend the guide frame is production art.
- Add net collision and ball interaction **after** the stadium, physics-ball and animation systems are available for engine-side testing.
- Avoid net tessellation and per-thread/per-strand meshes that create excessive rendering overhead. Test deformation, replays, lighting and shadows on PC hardware.
- Author a believable goal-line tunnel, pitch surrounds, signage and crowd architecture after a single well-lit broadcast hero shot proves the turf/player asset quality.
- Check football geometry against the actual IFAB Laws of the Game when finalizing markings and crossbar/post rules.

## Future UE editor verification

1. Compile Unreal project.
2. Place pitch at origin and goals at pitch endpoints.
3. Check frame proportions, net clearances, realistic model fit and player collision.
4. Replace construction cubes with production goal assets.
5. Capture authenticated UE screenshots at wide and close-up angles.
6. Confirm frame budget and pixel-quality targets on a named PC configuration.
