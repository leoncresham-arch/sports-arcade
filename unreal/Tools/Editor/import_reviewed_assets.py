"""Import previously licensed local asset files inside Unreal Editor only.

Requires Unreal Editor + Python Editor Script Plugin. Not executed or tested
in this project. Validate source license terms separately from this script.
"""
from pathlib import Path
import sys
import unreal

TOOLS = Path(__file__).resolve().parents[2] / "asset_pipeline"
sys.path.insert(0, str(TOOLS))
from validate_manifest import load_manifest  # noqa: E402


def main(manifest_path):
    path = Path(manifest_path).resolve()
    data, errors = load_manifest(path, require_files=True)
    if errors:
        raise RuntimeError("License/file validation failed:\n" + "\n".join(errors))

    tools = unreal.AssetToolsHelpers.get_asset_tools()
    for entry in data["assets"]:
        source = (path.parent / entry["local_file"]).resolve()
        task = unreal.AssetImportTask()
        task.set_editor_property("filename", str(source))
        task.set_editor_property("destination_path", entry["destination"])
        task.set_editor_property("automated", False)
        task.set_editor_property("save", True)
        task.set_editor_property("replace_existing", False)
        tools.import_asset_tasks([task])
        imported = task.get_editor_property("imported_object_paths")
        if not imported:
            raise RuntimeError(f"No objects imported from {source}; check Unreal import log")
        unreal.log(f"[SportsArcade] Imported {entry['id']}: {imported}")

    unreal.log("[SportsArcade] Asset imports attempted. Mesh, material, skeleton, "
               "animation and license integration MUST be reviewed manually.")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        raise RuntimeError("Pass an absolute path to a reviewed manifest JSON")
    main(sys.argv[1])
