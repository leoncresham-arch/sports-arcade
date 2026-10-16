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
- The script does not create navmesh, kickoff or complete referee restarts. GameMode now attempts to possess home roster slot 9 automatically, but this is uncompiled, unplaytested and may leave an additional default pawn in the level.
- Formation slots are authored through existing roster C++ methods; actual UE Python reflection and property names remain unverified.
- Existing match/AI code is still a provisional offline foundation, not working FIFA-like match play.
