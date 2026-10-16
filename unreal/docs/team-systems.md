# Sports Arcade native team foundation

Implemented in C++ (UE 5.5 project, still uncompiled):
- Each SportsArcadeCharacter has TeamIndex (0 = home, 1 = away), ShirtNumber and a football position enum.
- SportsArcadeTeamRoster is a level-authorable actor with a list of up to eleven members. AddPlayer rejects duplicate players, mismatched teams, invalid pointers and roster overflow. GetPlayerCount counts valid team members.
- Each roster offers pitch-coordinate layout slots for 4-3-3 and 4-4-2. Slots represent a **starting formation target** in centimetres, not AI or automatic spawn/positioning. Home attacks +X, away mirrors the X axis. Slot numbering is ordered from keeper through defence/midfield/attack; the formations do not yet enforce player positional roles.
- SportsArcadeGameMode.SwitchToNearestTeammate(team) finds a listed player near the match ball and asks the local PlayerController to possess it, excluding its current pawn. This is a manually exposed Blueprint-callable prototype; not yet an input mapping and not validated with character/AI controllers.

Technical limitations:
- These are not replicated team roster/game-state APIs. Server-authored online selection and controller handoffs need dedicated authoritative design.
- Only authored level roster instances participate, and none are automatically generated. 22 actual player instances, goalkeeper behavior, substitutions, tactics, off-ball AI and player-selection polish remain pending.
- Fixed field orientation and idealized normalized locations are temporary. Improve to formation data assets and team-relative pitch transforms.
- Source structural tests are not a UE build, level playtest, gameplay AI test, or rendering verification.

- Native keyboard Tab now requests closest eligible teammate possession through GameMode; still requires authored roster/player actors and UE editor testing. It does not yet support cycling direction, animation handoff, or multiplayer authority.

## Formation-aware off-ball AI (new source implementation)
- New `SportsArcadeTeamAIController` (C++ `AAIController`) becomes the default controller for footballers without human control. It reevaluates a level-authored player's roster formation slot every 0.3 seconds and requests a pathing move toward it.
- Non-goalkeepers shift their formation target toward the current ball position within capped X/Y distances, then clamp the target to pitch boundaries; keeper slot remains anchored.
- Added Unreal `AIModule` and `NavigationSystem` dependencies.
- **Editor requirements:** level must contain authored teams whose Players arrays point at placed footballers, sufficient navigation mesh covering the pitch, walkable pitch collision and proper player AI possession. This code does not spawn players, author NavMeshBoundsVolume, or manage tactical possession/phases.
- **Limits:** no opponent avoidance strategy beyond engine nav, no player/ball contact decisions, no marking or goalkeeper saves, no confirmed runtime tests or compiled UE. Controlled-player switching needs explicit AI handoff tests.

## Incremental AI behavior batch
- One nearest eligible outfielder can pursue the ball when within a configurable radius, predicting short-range ball travel, while other players retain their shifted formation. On-close-contact AI may kick toward the opposition goal with a cooldown.
- Keeper now shifts laterally, advances within a configurable limit and attempts a basic contact clearance near goal. This is **not** a real keeper-save/dive/handling simulation.
- AI respects match halftime/full-time; switching control back to another footballer attempts to restore AI possession of the former player.
- Current behavior remains simplistic, incomplete, and uncompiled; it requires a working navigable pitch, authored rosters and real Unreal tests.

## Further team gameplay foundations
- Authored team rosters can optionally discover already placed matching-team footballer actors at BeginPlay, respecting the 11-player cap. Lineup order is not guaranteed and requires deterministic squad-slot assignment later.
- Team AI adds simple bounded opponent shadow-marking near defenders, timed ball pursuit/contact, keeper positioning and clearance, and returns relinquished user-controlled pawns to AI when switching.
- This does not yet constitute true opponent-aware possession, passing selection, goalkeeper saves or production tactical decision trees.
