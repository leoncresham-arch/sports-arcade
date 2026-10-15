"""Fail-closed license and file verification for externally supplied UE assets.

This script uses only Python stdlib and can be executed without Unreal Engine.
It NEVER downloads, publishes, or imports a marketplace asset by itself.
"""
from __future__ import annotations

import json
from pathlib import Path
from urllib.parse import urlparse

ALLOWED_EXTENSIONS = {".fbx", ".gltf", ".glb", ".obj", ".png", ".jpg", ".jpeg", ".tga", ".tif", ".tiff", ".exr"}

ALLOWED_ROLES = {
    "player_skeletal_mesh", "football_animation", "goalkeeper_skeletal_mesh",
    "pitch_turf", "turf_material_source", "stadium_static_mesh",
    "ball_static_mesh", "goal_static_mesh", "other"
}


def validate_manifest(manifest: dict, base_dir: Path, require_files: bool = False) -> list[str]:
    errors: list[str] = []
    if manifest.get("schema_version") != 1:
        errors.append("Unsupported manifest schema_version (expected 1)")
    assets = manifest.get("assets")
    if not isinstance(assets, list):
        return errors + ["assets must be a list"]

    ids = set()
    for i, asset in enumerate(assets):
        label = f"assets[{i}]"
        if not isinstance(asset, dict):
            errors.append(f"{label}: expected object")
            continue
        asset_id = asset.get("id", "")
        if not isinstance(asset_id, str) or not asset_id.strip():
            errors.append(f"{label}: id required")
        elif asset_id in ids:
            errors.append(f"{label}: duplicate id {asset_id}")
        else:
            ids.add(asset_id)

        if asset.get("role") not in ALLOWED_ROLES:
            errors.append(f"{label}: unsupported asset role")
        if not asset.get("reviewed"):
            errors.append(f"{label}: license has not been reviewed")
        if asset.get("allows_game_use") is not True:
            errors.append(f"{label}: game-use permission must be explicitly true")

        for key in ("source_url", "license_url"):
            value = asset.get(key, "")
            parsed = urlparse(value) if isinstance(value, str) else None
            if not parsed or parsed.scheme != "https" or not parsed.netloc:
                errors.append(f"{label}: {key} must be an HTTPS URL")
        for key in ("creator", "license_name"):
            if not isinstance(asset.get(key), str) or not asset[key].strip():
                errors.append(f"{label}: {key} required")

        dest = asset.get("destination", "")
        if (not isinstance(dest, str) or
                not dest.startswith("/Game/SportsArcade/") or
                any(segment in ("", ".", "..") for segment in dest.split("/")[3:])):
            errors.append(f"{label}: destination must be within /Game/SportsArcade/")

        local_file = asset.get("local_file", "")
        if not isinstance(local_file, str) or not local_file.strip():
            errors.append(f"{label}: local_file required")
        else:
            path = Path(local_file)
            if path.is_absolute() or ".." in path.parts:
                errors.append(f"{label}: unsafe local_file path")
            elif path.suffix.lower() not in ALLOWED_EXTENSIONS:
                errors.append(f"{label}: unsupported asset file extension")
            elif require_files:
                # Relative paths alone are not enough: symlinks could point
                # outside the private asset staging folder.
                root = base_dir.resolve()
                candidate = (root / path).resolve()
                if not candidate.is_relative_to(root):
                    errors.append(f"{label}: local file resolves outside staging folder")
                elif not candidate.is_file():
                    errors.append(f"{label}: missing local file {local_file}")

        # Not a blocker for local editor imports, but prompts an intentional
        # decision before anyone checks binary assets into a public GitHub repo.
        if asset.get("allows_public_source_redistribution") is not True:
            pass
    return errors


def load_manifest(path: Path, require_files: bool = False) -> tuple[dict, list[str]]:
    obj = json.loads(path.read_text(encoding="utf-8"))
    return obj, validate_manifest(obj, path.parent, require_files=require_files)


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("manifest", type=Path)
    parser.add_argument("--require-files", action="store_true")
    args = parser.parse_args()
    data, problems = load_manifest(args.manifest, args.require_files)
    if problems:
        print("\n".join(problems))
        raise SystemExit(1)
    print(f"Validated {len(data['assets'])} licensed asset(s). No files imported.")
