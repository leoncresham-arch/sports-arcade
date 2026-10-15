# Sports Arcade — football pitch foundation

**Status:** source-level geometry scaffold committed. Not compiled, editor-tested, rendered, or photorealistic.

## Pitch dimensions

The `ASportsArcadePitch` actor defaults to:
- Full playing surface: 105 m x 68 m, centred at (0,0) in XY; 1 Unreal Unit = 1 cm.
- Centre circle radius: 9.15 m.
- Penalty area depth: 16.5 m; full width 40.32 m.
- Goal area depth: 5.5 m; full width 18.32 m.
- Penalty spot: 11 m from goal line.
- Painted lines: 12 cm, adjustable up to 15 cm.

These are standard football dimensions, but match and stadium asset dimensions must be checked together. Goal structures, flags, pitch corner arcs, pitch surface detail, camera rails, stadium architecture, lighting, collisions and net simulation are **not yet implemented in this actor**.

## Unreal integration

1. Open the native Unreal project on a compatible PC, compile the C++ game.
2. Drag an instance of `SportsArcadePitch` into a football level at the origin.
3. Assign an original legally usable PBR turf material to `TurfMaterial`.
4. Assign an opaque/unlit field-paint material to `MarkingMaterial`.
5. Replace the cube-based turf scaffold with production authored mesh/material as needed; keep the pitch coordinates aligned for collision and animation.
6. Verify material UV scale, drawing order, penalty arcs, corner markings and grass/paint surface contact in actual rendered frames.
7. Test lighting and optimize draw calls/instancing before expanding to a full stadium.

A simple engine cube surface with procedural lines is **not** the final grass solution. It is a measurement-accurate placement aid so high-quality assets can be introduced without redesigning pitch coordinates later.

## Camera coordinate convention

Long touchlines parallel to X, goal lines at X = ±5250 cm, camera side along Y. The broadcast camera actor should use the same origin and axis conventions.

## Tests

`python3 -m unittest discover -s unreal/tests -v` validates key source-level constants and component hierarchy. The tests do not substitute for Unreal compilation, material integration or visual testing.
