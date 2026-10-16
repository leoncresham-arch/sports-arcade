"""Capture a genuine Unreal Editor visual-review frame.

Runs ONLY inside an Unreal Editor session, after an actual map and reviewed
assets exist. This script has never been executed in Unreal as of creation.
Outputs are local to the editor host, not uploaded to GitHub automatically.
"""
from pathlib import Path
import json
import time
import unreal

CAMERA_LABEL = "SA | Broadcast Camera"


def find_broadcast_camera():
    editor = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    if editor is None:
        raise RuntimeError("EditorActorSubsystem unavailable; run in Unreal Editor")
    for actor in editor.get_all_level_actors():
        if actor.get_actor_label() == CAMERA_LABEL:
            return actor
    raise RuntimeError(
        "Broadcast camera not found. Open the Sports Arcade visual-review map "
        "and confirm the scene-setup script has been run."
    )


def capture_review(output_dir, width=1920, height=1080, approved_assets=False):
    """Request native UE screenshot; does NOT guarantee an image is produced.

    approved_assets is deliberately explicit. It indicates source art has been
    visually/legally reviewed, not something that can be inferred from Python.
    """
    if not approved_assets:
        raise RuntimeError(
            "Refusing to label a render as an art review until the caller "
            "explicitly confirms the imported assets have been reviewed."
        )
    if not (640 <= width <= 7680 and 360 <= height <= 4320):
        raise ValueError("Unsupported review image dimensions")

    camera = find_broadcast_camera()
    target_dir = Path(output_dir).expanduser().resolve()
    target_dir.mkdir(parents=True, exist_ok=True)
    filename = "sports-arcade-unreal-review.png"

    # Unreal's AutomationLibrary requests a screenshot from a running editor
    # viewport. Do not assume the output exists until a person verifies it.
    result = unreal.AutomationLibrary.take_high_res_screenshot(
        width, height, filename, camera=camera
    )
    if result is False:
        raise RuntimeError(
            "Unreal rejected the screenshot request; no capture was queued. "
            "Check the running viewport, camera and output permissions."
        )

    record = {
        "project": "Sports Arcade",
        "capture_kind": "Native Unreal Editor screenshot request",
        "generated_at_unix": int(time.time()),
        "requested_resolution": [width, height],
        "camera_label": CAMERA_LABEL,
        "approved_assets_claimed_by_operator": bool(approved_assets),
        "screenshot_request_return": str(result),
        "request_accepted": result is not False,
        "verification": "UNVERIFIED: open the actual screenshot in the editor host",
        "important": (
            "A screenshot request is not confirmation of a rendered image. "
            "Do not publish as genuine footage until file existence, content, "
            "source licenses, image authenticity and appearance are inspected."
        ),
    }
    out = target_dir / "sports-arcade-capture-request.json"
    out.write_text(json.dumps(record, indent=2) + "\n", encoding="utf-8")
    unreal.log("[SportsArcade] Screenshot requested. Verify UE screenshot output.")
    unreal.log("[SportsArcade] Capture request notes: " + str(out))
    return record


if __name__ == "__main__":
    raise RuntimeError(
        "Import this module in Unreal Editor and call capture_review("
        "output_dir, approved_assets=True) only after real art review."
    )
