# Sports Arcade — 11v11 offline source-level test map

This test-map generator is **not** a playable or verified 11v11 release. It has never run inside UE5.5. It assembles placed native player and roster actors only.

## Intended use on a UE 5.5 Windows editor machine
1. Open the existing SportsArcade.uproject after a successful C++ build. Enable Python Editor Script Plugin.
2. In Unreal's Python console, execute the existing file at Tools/Editor/setup_offline_match.py (for example via exec of the file contents, with a correct absolute file path).
3. The script creates /Game/Maps/SA_Offline11v11 and refuses to overwrite a previous map. It uses 105x68 m pitch defaults with 22 actors, two rosters, two goals, a ball and basic daylight.
4. Inspect the placed rosters and Player actor fields, add a pitch-wide NavMeshBoundsVolume, verify the native GameMode home-slot-9 possession handoff and clean up any extra spawned default pawn, and add legally usable footballer skeletal meshes and animations.
5. Run native build, PIE and full game tests separately. Static Python mocks do not prove API compatibility or gameplay.

## Known incomplete systems
- No Unreal Editor run or genuine rendered preview has occurred.
- Actors have no realistic player art, animation, kit, collision tuning or legal production assets.
- The script does not create navmesh, kickoff or complete referee restarts. GameMode now searches the home roster for a player explicitly assigned LineupSlot 9, then attempts to possess that actor, but this is uncompiled, unplaytested and may leave an additional unpossessed default pawn in the level; no existing actor is automatically deleted.
- Formation slots are authored through existing roster C++ methods; actual UE Python reflection and property names remain unverified.
- Existing match/AI code is still a provisional offline foundation, not working FIFA-like match play.

## Preflight source tests
- Mock editor checks enforce 22 players, unique per-team slot assignments 0-10, team indices, shirts 1-11, a non-overwriting map, and rejection when native class loading fails.
- Native Unreal Python property reflection is not proven by mocks. The first authentic UE5.5 Editor run must inspect each `set_editor_property` and `get_formation_position` call.

## Read-only level diagnostics
- Run Tools/Editor/validate_offline_match.py inside the UE Editor after opening the test level. It inspects native actor counts (1 ball, 2 rosters, 22 footballers), team roster membership and distinct lineup slots 0-10, logging validation errors without modifying a level.
- Python source checks do not establish that class names or reflected properties resolve correctly in UE 5.5; verify on a real editor host.

## Match television camera
The non-overwriting Editor map generator now places an authored native broadcast camera at a high sideline location and aims it at centre pitch. The GameMode already selects a placed broadcast camera when enabled. Unreal Editor Python camera rotation/reflection and viewport composition remain to be validated on Windows.
