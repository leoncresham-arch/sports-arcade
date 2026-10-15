# Sports Arcade — native Unreal graphics review level

**State: script committed, never executed in Unreal.** This is a tool for building an initial review scene later, not a screenshot or playable build.

## The preview we actually need

Our first visual-quality acceptance target is a **real Unreal-rendered frame and short motion capture** containing:

1. One authentic-looking original footballer, correctly rigged and dressed in an original kit.
2. A convincingly shaded pitch, calibrated daylight / stadium lighting and goals.
3. A broadcast camera shot and a medium close-up.
4. Natural stop/turn/jog motions, without floating feet or visible sliding.
5. Named PC hardware and genuine FPS measurement.

A screenshot of the editor's standard cubes, an AI-generated concept image, or a marketplace thumbnail does not count as a Sports Arcade Unreal preview.

## What this script prepares

The [editor preview bootstrap script](../Tools/Editor/setup_visual_review.py) is intended to create `/Game/Maps/SA_VisualReview` from inside Unreal Editor.

It uses the C++ `SportsArcadePitch`, `SportsArcadeGoal`, `SportsArcadeBroadcastCamera` actors, plus a reusable native `SportsArcadeLightingRig` actor containing sunlight, skylight, sky atmosphere and subtle fog. The rig has adjustable directional sunlight intensity, sun angle and ambient intensity. It intentionally does **not** try to fake a player with block geometry. It avoids overwriting an existing review map.

No third-party assets, photorealistic materials, MetaHumans, animations, or stadium meshes are bundled.

## How to run when a Windows computer with Unreal is available

1. Open `SportsArcade.uproject` in Unreal Engine and compile the native C++ classes.
2. Enable the **Python Editor Script Plugin** and restart the editor if prompted.
3. Verify that the `SportsArcadeLightingRig` actor is compiled and exposed to the editor. Open Unreal's Python console and execute the script:
   `exec(open(r"<absolute-repo-path>/unreal/Tools/Editor/setup_visual_review.py", encoding="utf-8").read())`
4. If APIs differ in the actual Unreal version, fix them against editor error messages instead of claiming the script has run successfully.
5. Assign legal PBR turf and pitch-line materials; review shadow contact and scale.
6. Import legally acquired character mesh, kit, proper animations, stadium, goal nets and ball.
7. Position the actual broadcast camera as a view target, capture real UE screenshots and a short animation sequence.
8. Validate performance on a known PC and share footage for visual approval.

**This setup script cannot run in GitHub Actions' ordinary free Ubuntu runner** because Unreal Editor and an appropriate GPU are not installed there. The CI only syntax-checks the Python file. It must not be reported as an Unreal build or graphics validation.

## Timing

There is no reliable date for a realistic preview while Unreal-capable hardware and an asset-import route are unavailable. Once those are secured, produce a single-player stadium visual slice *before* attempting a full 11v11 match.

### Lighting review

The new `SportsArcadeLightingRig` C++ actor is the starting point for consistent daylight visual comparisons. Its sun lux level, sky brightness and sun direction can be tuned in the editor; it does not replace a stadium roof shadow setup, physically validated exposure or night floodlights. Review first-pass values under calibrated exposure in an actual Unreal render, and compare skin, grass, shadows and kit fabrics at broadcast and close-up camera distances. No such render has yet been produced.
