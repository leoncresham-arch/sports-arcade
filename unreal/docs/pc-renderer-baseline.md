# PC graphics renderer baseline (Unreal Engine 5.5)

**Status:** Configured in source, not compiled, rendered or performance-tested. These settings are an initial technical target, not proof that the game looks realistic.

`unreal/Config/DefaultEngine.ini` explicitly selects:
- Temporal Super Resolution (TSR) with `r.DefaultFeature.AntiAliasing=4`.
- Lumen dynamic global illumination via `r.DynamicGlobalIlluminationMethod=1`.
- Lumen reflection method with `r.ReflectionMethod=1`.
- Mesh distance-field generation as a software Lumen prerequisite.
- Virtual shadow maps via `r.Shadow.Virtual.Enable=1`.

These may increase load and shader compilation on PC hardware. Do **not** treat them as settings every target PC must run. Device scalability profiles still need to be benchmarked on real hardware, and UE5.5 compatibility must be confirmed within the editor. Hardware ray tracing is not forced globally.

## First GPU validation checklist
1. Confirm project opens in native UE5.5, shaders compile, and its renderer settings are recognized.
2. Check software Lumen works without dedicated ray-tracing hardware and shadows do not exhibit artifacts.
3. Capture a controlled broadcast framing at 1920×1080, then a close-up of the same scene; note exact GPU/CPU, engine version and frame times.
4. Compare base color, exposure, grass detail, contact shadows, skin rendering, uniform materials and anti-aliasing during movement.
5. Profile frame time using `stat unit` and `stat gpu` (where supported) under a repeatable scene.
6. Compare medium/high/epic scalability profiles; choose a responsible minimum-spec target based on measured data.
7. Use Nanite selectively on genuinely detailed supported stadium/static geometry after import; do not mistake primitive block-out meshes for finished art.

## Source
Epic's Unreal Engine console-variable documentation describes the Lumen global-illumination setting and other rendering controls. Verify the active values in Unreal 5.5 before finalizing.
