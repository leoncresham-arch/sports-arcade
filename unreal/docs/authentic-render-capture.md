# Authentic PC graphics review — capture workflow

**Not yet performed.** No screenshots or photorealistic Unreal frames have been captured. A real Unreal Editor host with functioning GPU and approved high-fidelity art is required.

## New capture helper
- Script: [`capture_visual_review.py`](../Tools/Editor/capture_visual_review.py)
- It looks for the broadcast camera in the live Unreal Editor scene and requests a high-resolution screenshot using Unreal's `AutomationLibrary.take_high_res_screenshot` API.
- The operator must explicitly set `approved_assets=True`; this is an acknowledgment, **not automated license proof**.
- It saves a **capture request record** locally. A request log is **not** evidence that a screenshot was produced.
- The screenshot itself is managed by Unreal's screenshot system. Its actual output path, camera framing, resolution and completeness must be checked on the PC.
- The screenshot helper is deliberately not wired into ordinary GitHub Actions, which have no Unreal Editor or GPU.

## When an Unreal-equipped computer becomes available
1. Compile the project; note engine version and fix actual C++ errors.
2. Run `setup_visual_review.py` in Unreal Editor, then import and integrate legally reviewed 3D footballers, kit, stadium, turf and animations.
3. Open the preview map and confirm the broadcast camera label **SA | Broadcast Camera**.
4. In Unreal's Python console, import the script from its local source folder and call:
   ```python
   import sys
   sys.path.insert(0, r"C:\\path\\to\\sports-arcade\\unreal\\Tools\\Editor")
   import capture_visual_review
   capture_visual_review.capture_review(r"C:\\private-review-output", approved_assets=True)
   ```
5. Inspect Unreal's actual screenshot file and verify the scene includes real footballer/kit/turf materials, not a bare construction map.
6. Review framing, animation continuity, and measured FPS on known hardware.
7. Publish verified screenshots/video through a rights-appropriate, iPad-accessible route; **do not publish licensed source assets to the public GitHub repository**.

## Acceptance criteria
The milestone is met only when a real UE-rendered image or interactive/video capture of the actual Sports Arcade scene is available and visually approved. Source code, mock tests, concept art and marketplace thumbnails do not count.
