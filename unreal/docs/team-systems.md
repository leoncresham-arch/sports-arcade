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

## October 16 gameplay development — passing decisions and control quality
- AI ball carrier now scores forward teammates within range, rejects passes whose two-dimensional lanes intersect opponents within a configurable fixed clearance, and falls back to a direct clearance when no lane qualifies. Uses basic impulses, not a polished possession or passing animation system.
- Corrected Tab switching to search all roster players instead of failing when the nearest player is already user-controlled.
- AI kicks require a plausible vertical distance to the ball as well as horizontal proximity. Source regression checks updated; Unreal native compilation and gameplay balance remain unverified.

- Added per-player LineupSlot (0 goalkeeper through 10) and an opt-in roster setting `bUseExplicitLineupSlots`, so teams can author deterministic positions instead of depending on automatically discovered roster order. This mode requires distinct slots to be configured manually; the default remains array order for compatibility.

## October 16 roster discovery determinism
- When a roster begins with an empty Players array, discovered matching footballers are ordered by shirt number then actor name to avoid actor-iterator-dependent formation indexing. Manually authored roster ordering is left unchanged. Duplicate shirt numbers still need squad-level validation; source checks are not UE runtime proof.
- AI default formation targets now scale from the original 105x68 m pitch to GameMode's configurable match geometry; authored pitch/level and animations must still be updated consistently.

## Native lineup validation
- SportsArcadeTeamRoster now exposes HasCompleteLineup(). It requires exactly eleven valid players belonging to that roster's team, each assigned a unique LineupSlot from 0 through 10. The query does not automatically block or repair a match, and native Unreal compile/play remains pending.


## October 19 — AI team pressing and pass receiver safety
- Nearest automatic team presser excludes footballers currently possessed by a human player controller; this stops human movement from suppressing all team AI pressure. Foreign-world roster candidates are also excluded.
- AI passing decisions now reject tightly marked receivers (within 210 cm of an opponent), with a graduated scoring penalty when an opponent is closer than 450 cm to the intended receiver. Existing pass lane interception checks remain in place.
- Both changes have Python source regression coverage and source CI. True gameplay balance and Unreal navigation behavior remain unverified in-editor.


## October 19 — short support passing extension
- AI off-ball passing permits an outfielder up to 650 cm behind the ball when no higher-scoring forward option is available, reducing forced random clearances. Forward penetration still scores higher.
- Candidate receivers from unrelated Unreal worlds are discarded; receiver marking and corridor occlusion remain factors.
- Python regression checks cover the new source path; balance and player behaviors must still be tested inside Unreal.


## October 20 — preliminary shootout goalkeeper reaction
- When a physical cup penalty has been kicked and awaits explicit referee scoring review, only the defending team's authored goalkeeper (lineup slot 0) bypasses the usual full-time AI pause. The keeper tracks the ball laterally within a 600-cm goal-mouth window and can try a close-contact physics parry.
- Every other AI footballer remains paused. This is a provisional ball-tracking/parry behavior, not full goalie diving, catching, collision fairness, outcome attribution or animation. The referee must still explicitly record scored/missed attempts.
- `GetCupPenaltyDefendingTeam()` provides the currently defending side to AI and Blueprint presentation.
- Source CI checks the state routing and contact code; runtime navigation, movement, and realistic balance remain pending for native UE 5.5 testing.

## October 20 — penalty goalkeeper anticipation

During an active *physical* shootout penalty, the defending keeper's provisional AI now projects the ball's lateral position at the keeper's current X plane for plausible forward-arriving shots (0–1.5 seconds), rather than simply chasing its present Y position. The target remains clamped within 300 cm of the goal centre; existing incoming-shot and close-contact checks still gate physics parries. Outfield AI remains stopped during referee review. AI decision clocks now reject nonfinite/negative simulation frame deltas. Source regression coverage: `unreal/tests/test_penalty_keeper_prediction.py`. This is not a dive animation or verified save system; in-editor tuning, navigation and actual UE compilation are pending.
