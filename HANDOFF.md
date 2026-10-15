# SPORTS ARCADE — HANDOFF / CONTINUITY NOTES

Last updated: 2026-10-14/15 (UTC). Read the current GitHub tree and Actions status before making assumptions.

## Mission and user requirements
- Build **Sports Arcade**, an original high-quality photorealistic 3D football/soccer game inspired by the *visual standard* of modern FIFA / EA Sports FC, without copying licensed game art/code/brands.
- **Primary target is native Windows PC Unreal Engine**, not a browser prototype. The user prioritizes realistic graphics before expansive gameplay.
- User is working **only from an iPad**, wants **free development**, and expects the assistant to make GitHub changes directly; do not assign manual GitHub work to them.
- User wants to **preview progress from iPad**. The browser showcase is only a companion, not a substitute for genuine Unreal graphics. Do not promise a photorealistic preview until an actual Unreal engine render is available.
- The user specifically asked: **If a handoff to a new chat is ever needed, post one**. This document is a persistent repository handoff; when context transfer is necessary, give a standalone user-visible handoff message pointing here and recapping the status.
- User dislikes repeatedly reading descriptions of source-only improvements when no graphics preview is delivered. Prioritize a credible plan to obtain Unreal rendering capability and legal high-quality human/stadium assets.

## GitHub and links
- Account: `leoncresham-arch`
- Repo: https://github.com/leoncresham-arch/sports-arcade (public, branch `main`).
- Unreal source: https://github.com/leoncresham-arch/sports-arcade/tree/main/unreal
- iPad **browser stadium review**: https://leoncresham-arch.github.io/sports-arcade/stadium-review.html
- Previous crude browser stadium: https://leoncresham-arch.github.io/sports-arcade/3d.html
- GitHub Pages deployment workflow: `.github/workflows/pages.yml`.
- Free Unreal source validation: `.github/workflows/unreal-source-check.yml`.
- User reports ordinary Markdown links work on iPad; avoid special UI Copy/Open controls which failed in earlier chats.

## Committed native Unreal project state
- UE 5.5 C++ project scaffold in `unreal/SportsArcade.uproject`.
- Native actor sources under `unreal/Source/SportsArcade/`: `SportsArcadeCharacter`, `SportsArcadeGameMode`, `SportsArcadeAnimInstance`, `SportsArcadeBroadcastCamera`, `SportsArcadePitch`, `SportsArcadeGoal`, `SportsArcadeLightingRig`, `SportsArcadeVisualAssetSet`, and `SportsArcadeBall` (basic sphere physics/mass/kick impulse, still uncompiled).
- Camera, pitch, goals, lighting and reference ball are intended to be placed by `unreal/Tools/Editor/setup_visual_review.py`.
- Unreal level script syntax gets validated by CI. **The script has never been run inside Unreal.** No UE editor, compiler, GPU, PC workstation or connected cloud Unreal environment is available now.
- `unreal/tests/test_project_structure.py` contains Python static/source contract checks (NOT C++ compilation or rendering).
- Docs include `docs/graphics-production-plan.md`, `unreal/docs/visual-preview-build.md`, `unreal/docs/character-animation-pipeline.md`, `unreal/docs/free-asset-candidates.md`, `unreal/docs/ball-physics.md`.
- No actual licensed realistic rigged players, animations, turfs, stadiums, or materials have been imported, and no photorealistic Unreal preview exists.
- The browser stadium review is procedural, deliberately stylized and iPad compatible. It is **not Unreal content**, and should not be represented as the finished PC game's quality.
- Do not carry over the discarded `index.html` browser gameplay prototype as design requirements.

## Important engineering caveats
- Source is **uncompiled**; apparent UE method/signature compatibility is not evidence of a successful build.
- Free stock GitHub Actions Ubuntu runners cannot be assumed to include Unreal Engine. Do not claim they can deliver native UE builds.
- Third-party assets need documented license and redistribution rights. Do not put proprietary Fab downloads/MetaHuman files into a public source repo without license review.
- Work completed purely in GitHub may still be helpful, but the critical blocker is an **actual Unreal render on GPU-equipped hardware**. Treat this as the top delivery risk, not as a reason to endlessly add scaffolds.
- Verify GitHub Actions via repository Actions API and confirm the latest relevant run status; avoid saying 'passed' based on a previous commit.

## Latest work: asset intake (2026-10-15)
- Added `unreal/asset_pipeline/validate_manifest.py`, `manifest.example.json`, Unreal-only `Tools/Editor/import_reviewed_assets.py`, and `tests/test_asset_manifest.py`.
- Added [licensed-asset intake documentation](unreal/docs/licensed-asset-intake.md). No actual third-party models or animations imported.
- CI tests validate the import manifest but DO NOT run Unreal or confirm an actual render. Always inspect the latest Actions result for the current commit.

## Latest maintenance (2026-10-15): asset intake review
- Hardened `unreal/asset_pipeline/validate_manifest.py` to reject unsupported asset extensions, destination traversal and symlink escape outside private staging. Expanded CI regression tests.
- Corrected `unreal/docs/licensed-asset-intake.md` with a concrete Unreal Python-console module-import example.
- This improves asset intake readiness but **does not prove native Unreal import or compile success**; the realistic PC preview remains blocked on an actual Unreal workstation and licensed art.

## Latest native PC work — October 15, 2026
- Removed conflicting world-position assignment from `SportsArcadeBroadcastCamera`'s C++ constructor so level-authored camera placement stays authoritative.
- Added `unreal/tests/test_preview_bootstrap.py`: simulated Unreal-editor integration tests for the native preview bootstrap script, actor placements, overwrite protection and missing C++ classes.
- Static/mock tests passed in GitHub CI run: https://github.com/leoncresham-arch/sports-arcade/actions/runs/38825081777
- **No actual Unreal Editor has been run**; the real PC visual preview and asset import remain blocked on access to a compatible GPU/editor host.

## Immediate next steps
1. Confirm new ball/scene source checks pass; fix failures if not.
2. Research and verify **genuinely available free** Unreal-friendly realistic footballer / animation / stadium assets and record licenses and UE version compatibility. Do not mistake listing images for actual Sports Arcade graphics.
3. Explore **viable, genuinely free and legal** GPU/editor execution options, with no invented cloud entitlements. If none exists, communicate that blocker honestly.
4. When UE hardware is accessible, compile source; resolve API issues; import proper assets; execute setup script; capture authentic preview footage and measurements and make it iPad-viewable.
5. Obtain user visual feedback on the authentic footage; only then expand into 11v11 gameplay/networking.

## Working method for follow-up chats
- Use GitHub connector tools through `functions.exec`, discover `mcp__GitHub__fetch_file`, `update_file`, `create_file`, and `fetch`.
- Fetch existing file and its blob SHA before updating; do not overwrite other changes.
- Update `unreal/README.md` to reflect meaningful new native development.
- Distinguish committed work, CI/static checks, actual Unreal compile, and authentic on-device rendered preview at all times.
