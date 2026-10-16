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

## Latest work — Unreal visual-review capture (2026-10-15)

- Created `unreal/Tools/Editor/capture_visual_review.py`, `unreal/docs/authentic-render-capture.md`, and `unreal/tests/test_capture_visual_review.py`.
- Capture code expects a genuine running Unreal Editor and refuses operation without explicit acknowledgment of reviewed assets; produces an *unverified* capture-request record rather than falsely claiming a render.
- **No real image/video from Unreal exists yet.** Do not imply screenshot capture succeeded merely from a passing mock test.
- First real graphics-review footage remains blocked by editor/GPU hardware and actual licensed realistic footballer, kit, turf, stadium and animation imports.

## Latest native integration (2026-10-15): broadcast review camera

- `unreal/Source/SportsArcade/Private/SportsArcadeGameMode.cpp` now selects a level-placed `SportsArcadeBroadcastCamera` for the local controller and optionally tracks the reference `SportsArcadeBall`.
- The behavior is gated by `bUseBroadcastCameraIfPresent` and `bTrackReferenceFootball`; other maps fall back to their ordinary camera.
- Added a CI source-structure test. **This has not been compiled in Unreal or validated visually.**

## Latest Unreal compatibility fix — 2026-10-15

- Updated `unreal/Tools/Editor/setup_visual_review.py` to use `LevelEditorSubsystem` for map creation/saving, replacing older `EditorLevelLibrary` calls.
- Extended `unreal/tests/test_preview_bootstrap.py` mocks and failure test accordingly.
- No genuine Unreal editor run, C++ compile, player asset import, or PC-quality render yet.

## Latest development — UE5 PC renderer baseline

- Updated `unreal/Config/DefaultEngine.ini` with TSR, Lumen global illumination/reflections, distance fields and virtual shadow maps.
- Added `unreal/docs/pc-renderer-baseline.md` with hardware validation checklist and source-level regression test.
- Renderer configuration is **unverified in actual Unreal 5.5**. Do not imply any native PC graphics preview exists.

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

## Continued development — 2026-10-15 (new-chat continuation)
- Hardened native `capture_visual_review.py`: explicit Unreal screenshot API rejection now raises an exception rather than leaving a misleading accepted-request record. Added simulated-editor regression and refreshed docs/README. This is NOT an actual captured render.
- Researched official Adobe Mixamo and Epic Fab license terms; recorded concrete source links and safe integration boundaries in `unreal/docs/free-asset-candidates.md`. These are candidates only: no assets acquired or imported.
- High-priority blocker unchanged: real UE 5.5 editor/Windows GPU and approved high-fidelity football artwork. No photorealistic screenshot has been delivered.

- Follow-up fix: broadcast camera now derives its tracking offset from the level-authored world position in BeginPlay, preventing a fixed-offset jump on first tick; added CI source regression. Native Unreal behavior remains uncompiled/unverified.

- Hardware investigation (same continuation): documented official Unreal host specifications and inability to verify genuinely free UE GPU hosting in `unreal/docs/pc-renderer-baseline.md`. Source CI passed for camera placement correction, but no native compile or renderer verification has occurred.

## Development continuation
- User explicitly requested conventional native PC development to continue without treating missing screenshots as a blocker. Native Unreal engine validation remains deferred, not development.
- Repaired input configuration newline formatting and added J pass/K shoot bindings with range-limited ball impulses.
- Added source regression test. Full UE compilation and realistic football animation remain outstanding.

## 2026-10-15 continuation: PC player endurance
- GitHub push permissions verified and commits resumed.
- Implemented native per-character sprint stamina drain/recovery in Tick, exhaustion cooldown, tunable rates and Blueprint-readable normalized stamina; added source tests. Source-only, not compiled in Unreal. Modern Windows gaming PC remains target; graphics preview is not a development gate.

## Research expansion — complete football gameplay capabilities, 2026-10-15
- User requested research into broad FIFA/EA Sports FC-like player skills and game systems, with original implementation rather than copying source/art/IP.
- Added `unreal/docs/football-capabilities-blueprint.md`, a researched and prioritized catalogue of movement, first touch, dribbling, skill moves, attack, defending, goalkeeping, AI, player attributes, match rules, controls, modes and technical architecture, citing official FC 26/27 materials.
- Blueprint is scope documentation, NOT completed mechanics. Continue implementation in substantial native PC batches without waiting for screenshots.

## Player rating implementation continuation (October 15, 2026)
- Added native USportsArcadePlayerAttributes UActorComponent with 1-99 ratings for pace, stamina, passing, finishing, acceleration, ball control and tackling.
- Integrated pace, stamina, pass and shot scaling in SportsArcadeCharacter, initialized full stamina in BeginPlay; neutral rating 65 is exactly 1x.
- Other ratings are not active yet. Python source checks are not Unreal compilation or in-game simulation.
- User wants substantial conventional high-end Windows PC game implementation without waiting for screenshot preview.

## October 15 continuation: native offline match foundation
- Added native GameMode match clock, score, goal line check and reset after goal, plus source regression test and `unreal/docs/native-match-foundation.md`.
- This detector assumes fixed world-axis pitch positions and discrete ball sampling. It is NOT an official referee system, reliable swept goal collision, halftime implementation or multiplayer game state.
- Continue Unreal PC development without waiting for screenshot. Unreal compilation/playtest remain outstanding.

## October 15/16 continuation — match simulation batch
- Added halftime pause, explicit second-half resume, match reset, normalized clock progress and per-character team index/shirt number.
- Improved goal check with interpolated trajectory position at the goal line, instead of judging only the end-of-frame ball position. Assumes fixed pitch axes and does not replace engine collision verification.
- Added source-level regression tests. Native C++ Unreal compile and gameplay remain unverified. User requests larger development batches without pausing after every minor change.

## Team systems implementation — 2026-10-15/16
- Implemented `SportsArcadeTeamRoster` C++ actor with 11-player capacity, team validation, nearest candidate lookup and 4-3-3/4-4-2 starting location slots.
- Added original field-position enum to footballers and Blueprint-callable local team-member possession switching from GameMode. This is an initial, uncompiled foundation, not automated player input, complete AI, player spawning or online game state.
- New documentation: `unreal/docs/team-systems.md`. Source regression tests cover roster and formation contracts. Continue larger development batches without user approval after every small commit.

- Native keyboard Tab now requests closest eligible teammate possession through GameMode; still requires authored roster/player actors and UE editor testing. It does not yet support cycling direction, animation handoff, or multiplayer authority.

## New native off-ball AI source (October 15/16)
- Added ASportsArcadeTeamAIController class, Unreal AI/navigation dependencies and default AI possession for unpossessed footballers. Periodically requests movement toward roster formation positions, shifted toward live ball locations. Tests cover source wiring; real navmesh/editor behavior remains unverified.
- Requires correctly authored 11-player rosters and pitch navigation volume; no 11v11 simulation, goalkeeper intelligence or animations yet. Updated `unreal/docs/team-systems.md`.

## October 15/16 consecutive AI batch
- Off-ball controller now limits nearest eligible team-player pressure, predicts ball position for short pursuit, applies timed low-level directional ball impulse on contact, keeps goalkeeper inside bounded lateral/advance positions and lets keeper clear close ground/low aerial balls. These are preliminary mechanics, not authentic tactical AI or licensed animations.
- AI movement pauses at halftime/full-time; player switch returns former human pawn to default team AI when possible. CI structural checks only; no Unreal compile or runtime/navmesh validation.

## Consecutive development continuation — team systems
- Extended AI with tunable pressure radius, short-term predicted ball interception, simple opponent marking influence, initial goalkeeper lateral positioning and close-range clearance; paused AI during halftime/fulltime; restored default AI controller to the old pawn after player switching.
- Added optional runtime auto-discovery of authored player actors into each capped team roster, with source checks. Ordering requires later deterministic squad lineup design.
- These are still original prototype tactics and uncompiled UE C++ (not evidence of full-quality FC-like behavior). No screenshot prerequisite.

## October 16 gameplay development — passing decisions and control quality
- AI ball carrier now scores forward teammates within range, rejects passes whose two-dimensional lanes intersect opponents within a configurable fixed clearance, and falls back to a direct clearance when no lane qualifies. Uses basic impulses, not a polished possession or passing animation system.
- Corrected Tab switching to search all roster players instead of failing when the nearest player is already user-controlled.
- AI kicks require a plausible vertical distance to the ball as well as horizontal proximity. Source regression checks updated; Unreal native compilation and gameplay balance remain unverified.

- Added per-player LineupSlot (0 goalkeeper through 10) and an opt-in roster setting `bUseExplicitLineupSlots`, so teams can author deterministic positions instead of depending on automatically discovered roster order. This mode requires distinct slots to be configured manually; the default remains array order for compatibility.
