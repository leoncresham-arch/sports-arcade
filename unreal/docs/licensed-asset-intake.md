# Licensed asset intake — main PC project

**Status:** Source-validated workflow only. No purchased/free licensed 3D models have yet been imported and no native Unreal frame has been rendered. This workflow requires a Windows PC (or another suitable Unreal Editor host).

## Why this matters
The PC game must use believable human models, animations, kits, turf and stadium artwork. A marketplace thumbnail is not our game, and 'free' does not necessarily mean an editable copy of a marketplace asset can be redistributed in a public GitHub repo.

The source repository therefore contains a **manifest template**, a **fail-closed validator**, and an **Unreal Editor importer**, not any third-party binaries.

## Files
- `unreal/asset_pipeline/manifest.example.json` — sample manifest. Deliberately fails validation until real license and file details are entered.
- `unreal/asset_pipeline/validate_manifest.py` — checks source URL, license, creator, approved game use, destination, and safe local path.
- `unreal/Tools/Editor/import_reviewed_assets.py` — Unreal Editor-only import flow for separately acquired assets.
- `unreal/tests/test_asset_manifest.py` — offline checks used by GitHub Actions.

## Local use once Unreal is available
1. Obtain an asset legally, inspect its exact license, check Unreal compatibility, and store it **outside the public repository**.
2. Copy `manifest.example.json` into a private working folder and update the entries with the exact author, license name + URL, source URL, reviewed flag, and relative local file path.
3. Run `python3 unreal/asset_pipeline/validate_manifest.py /path/to/private/manifest.json --require-files`. An unrevised example intentionally fails.
4. In Unreal Editor with the Python Editor Script Plugin enabled, use the Python console. Adapt these absolute paths to your private workstation:
   ```python
   import sys
   sys.path.insert(0, r"C:\\path\\to\\sports-arcade\\unreal\\Tools\\Editor")
   import import_reviewed_assets
   import_reviewed_assets.main(r"C:\\private-assets\\manifest.json")
   ```
   This uses the module's real `__file__` to locate our validation library; it must be tested against the installed Unreal Python API before relying on it.
5. Review every imported asset: rig, mesh, scaling, LODs, skeleton retargeting, textures, PBR materials, performance, and post-import warnings.
6. Assemble the visual review map, capture a **genuine native Unreal rendering**, and publish only footage/screenshots licensed for display on the iPad.

## Important restrictions and limitations
- No importer can independently determine if a license is genuine, whether an attribution is complete, or if redistribution is legally permitted. A human must check source terms.
- `allows_public_source_redistribution` is recorded for future GitHub decisions; it is not required for import into a private working copy, and **assets without this permission must not be committed as editable source binaries in the public repo**.
- This is a generic Unreal `AssetImportTask` scaffold and cannot configure every rig/material/animation import correctly without an Unreal runtime and target asset.
- The validator uses a local manifest path; do not treat it as a remote downloader. No automated third-party downloads occur. It now rejects unsupported file types and files resolving outside the staging folder (including symlinks).
- Once hardware is available, Unreal compilation and plugin/Python API compatibility must be tested before claiming this pipeline works end to end.

## Preview milestone
For the first meaningful visual comparison, acquire and inspect: one highly realistic original footballer rig, original uniform/boots, high-quality locomotion animations, PBR turf and a convincing stadium environment. A native rendered still and short video should be accessible from Safari, with a description of the actual hardware, assets and graphics settings.
