# Native match scoring foundation (Unreal 5.5 source)

This adds an initial **offline match loop** to the existing SportsArcadeGameMode.

- Match clock advances in Tick and stops after two configurable half lengths (default 300 real seconds per half). There is **not yet a halftime pause or referee state**.
- HomeGoals/AwayGoals and ElapsedMatchSeconds are Blueprint-readable. RecordGoal allows simple score updates while a match is running.
- The first SportsArcadeBall found in the level is observed. Its center crossing beyond X = +/- (5250 cm + ball radius), while inside the 7.32 m goal mouth and under the 2.44 m crossbar, triggers a point and resets the ball.
- The X-axis assumption uses the fixed 105 m construction pitch and default goal positions. Moving or resizing the field requires a proper authored goal-line volume/transform abstraction.
- This is a **starter score detector, not official full-ball swept collision**: fast moving balls could pass between discrete samples, and rotated goals / own goals / restarts / halftime / out-of-bounds / networking are not handled. A robust future implementation will use per-goal sensors with continuous crossing logic, team orientation and event-driven referee state.
- No automatic goalkeeper or 11v11 AI exists yet. The score system must be engine-compiled, played and validated before calling it functional gameplay.

## Next steps

Extract match clock and refereeing into testable game state for replication, add start/halftime/full-time phases, team ownership and configurable goal detectors, verify physics crossing under varying frame rates, then wire original scoreboard UI.

## Later source updates
- Added halftime pause and explicit ResumeSecondHalf, match reset and normalized HUD progress. These are game-mode methods, not yet a full referee-controlled kickoff or halftime UI.
- Goal detection now interpolates the ball trajectory at the goal plane so diagonal shots are assessed at the crossing rather than at the last sampled endpoint. This is still sampled per frame (no Chaos continuous collision proof or ball-out event), assumes global-axis field orientation and fixed goal dimensions.
- Footballers carry TeamIndex and ShirtNumber data, but team roster assignment, control switching and own-goal attribution are still pending.

## October 16 source integration
- Match emits OnGoalScored (home score, away score, simple own-goal flag) for future Blueprint scoreboards and presentation. LastGoalTouchPlayer records the last attributed player contact; bLastGoalWasOwnGoal assumes home scores on positive X and away on negative X, with last-touch team 0/1.
- Ball exposes GetLastTouchTeamIndex (INDEX_NONE when unknown). Last touch means most recent **attributed** kick or controlled touch, not proof of possession, assist, scorer or foul attribution. The goal event is source-tested but no Blueprint widget/UE runtime event test exists.
- Controlled human input, dribble and ball striking are blocked at halftime and full time; seven player ratings now modulate acceleration, pace, stamina, pass/shot power, dribble target speed and standing tackle impulse. All balance remains provisional.

## October 15 continued controls and phase handling
- H resumes play from halftime using `ResumeSecondHalf`; R resets the offline exhibition using `ResetMatch`. These use the existing native Character input bindings and existing GameMode, not new architecture.
- On entering halftime the match ball resets to the centre, clearing its touch record and velocity; at full-time the ball stays in place but its linear/angular velocity is zeroed through `StopMotion`.
- These are preliminary intermission and result handling; no authentic kickoff/restarts, referee checks, UI or native engine validation. The reset command is deliberately available during play for prototype testing.

## October 16 — preliminary ball-out stoppage
- After evaluating goals, GameMode detects a ball fully beyond the fixed 105x68 m pitch bounds and broadcasts `OnBallOutOfPlay(ExitLocation, LastTouchTeam)`; it stops ball velocity and match clock while awaiting a restart.
- `B` invokes `ResumeAfterBallOut`, resetting the ball to centre temporarily. This is deliberately NOT a legal throw-in, corner, goal kick or referee restart, and diagonal boundary sampling is provisional.
- Human and team AI actions are guarded by the ball-out state; native Unreal playtest remains pending.

## Typed boundary restarts (October 16)
- Touchline crossings award provisional throw-ins; endline crossings classify corners and goal kicks by last attributed team and defending side. Unknown last-touch endline exits become Undetermined rather than fabricating possession.
- B positions the ball at the recorded in-pitch restart point, clearing motion and contact history. It does not implement a lawful throw animation, assigned taker, defending distance or live set-piece procedure. Fixed world-axis geometry and frame sampling remain provisional.

## October 16 match-stoppage refinements
- Scored goals on both goal lines immediately exit the detector, preventing a second out-of-bounds event for a valid goal. Boundary/goal decisions now precede advancing the match clock for that frame.
- Awarded restart team is restricted to known home/away attribution; otherwise restart stays Undetermined. A goalkeeper AI first-pass parry recognizes incoming ball velocity and redirects near-contact shots laterally. These require Unreal playtesting and remain far short of fully animated/refereed saves.


## October 19 — knockout phase presentation and penalty preflight
- `GetCurrentMatchPhase` now distinguishes a drawn cup awaiting extra time, the two extra-time periods and interval, a draw awaiting penalties and an active shootout. Kickoff/stoppage phases retain priority while a period is live; a recorded winner returns FullTime.
- Penalty attempts now prevalidate the bound knockout fixture index and both selected club identities before consuming a kick, preventing stale bracket references from progressing a shootout.
- Regression coverage: `test_cup_resolution_phases.py` and `test_cup_match_integration.py`. These remain source-contract checks, not compiled Unreal runtime coverage.


## October 19 — Blueprint match clock and period contract
- `GetMatchClockMinutes()` reports 0–90 normalized football minutes from configurable regulation duration, then 90–120 during extra time. Stoppages and interval preserve the recorded elapsed counters.
- `GetMatchPeriodNumber()` reports first/second regulation (1/2) and first/second extra-time (3/4) based on the authoritative period state. The HUD should use the separate match-phase enum for restarts, shootouts and finished results.
- Cup shootout creation now validates fixture position and club identities before creating a shootout, complementing per-attempt preflight. Python source contracts run in CI; UE runtime remains untested.


## October 19 — restart taker ownership and UI readiness
- The existing provisional restart now refuses to teleport a footballer owned by a different world. Eligibility checking is consolidated in `CanResumePendingRestart()`, shared by gameplay and future Blueprint match HUD interactions. It checks match stoppage, restart classification, award team, valid ball and taker, taker team and world ownership.
- Added `test_restart_taker_world.py`; updated the older `test_restart_taker.py` source contract to assert the same guard. The current restart remains provisional and not an officiated throw-in/corner/goal kick.
- This does not prove native Unreal gameplay behavior; verify movement, possession and set-piece locations in Editor when available.


## October 19 — explicit manual restart gameplay API
- When `bAutoKickProvisionalRestart` is disabled, `ResumeAfterBallOut` positions the taker and ball but keeps the match clock stopped. The match no longer silently resumes without a real ball contact.
- A new BlueprintCallable `TakeManualRestart(KickDirection)` is available after staging. It requires a valid awarded taker, same-world contact, a nonzero kick direction and successful physics impulse before clearing stoppage and broadcasting the resume event.
- The internal `bManualRestartStaged` flag prevents taking a manual restart before the ball is placed; cleared with the pending restart state. These remain provisional offline kicks, not a complete licensed referee/set-piece animation system.
- Python source regression tests cover manual/automatic paths, action ordering and failure handling. Runtime Unreal 5.5 validation is still needed.


## October 19 — keyboard-accessible manual restarts
- For levels configured with `bAutoKickProvisionalRestart=false`, press **B** to stage the awarded taker and ball while maintaining the referee stoppage, then **M** to take the restart using the taker's current facing direction. Alternatively, UI Blueprint calls `CanTakeManualRestart` and `TakeManualRestart(FVector)` can supply a deliberate direction.
- A repeated B cannot reposition an already-staged manual ball. Invalid zero/nonfinite vectors are rejected; successful attributed physics contact is required before match play resumes.
- Default automatic mode still works with B only. Neither option substitutes for legal throw-in hand animations, player separation or full referee review.


## October 19 — finite-impulse ball physics guards
- Native ball kick, player-attributed kick and controlled dribble APIs now reject nonfinite force, range and velocity inputs before requesting Chaos physics impulses. Failed attributed kicks must not assign a last-touch player.
- This is a defensive C++ integrity change, covered by source tests; actual ball movement and engine API compatibility must still be confirmed in UE 5.5.

- Manual restart eligibility also checks that the staged ball remains within 60 cm horizontally of the official restart mark. Drifting balls must be restaged rather than kicked from an incorrect location.

- If the prepared dead ball drifts more than 60 cm, pressing B again restages it at the restart mark; otherwise repeated B presses leave a correctly staged ball alone. An invalid drifted ball cannot be struck by M until it has been restaged.


## October 20 — referee review for unknown-touch boundary exits
- `AwardUndeterminedRestart(AwardedTeam)` is a Blueprint-callable explicit decision for a stopped out-of-play ball whose last attributed team is unknown; it does not guess an award automatically.
- Touchline exits become throw-ins; endline exits become a goal kick for the defending side or corner for the attacking side. Goal kick awards prefer the authored goalkeeper.
- Invalid team values, non-unknown stoppages, play-in-progress and missing ball are rejected. Missing eligible taker retains the unresolved restart and original referee mark for later retry.
- Starting any new kickoff clears old staged manual set-piece state. These are offline rules foundations, not full officiating or legal restart animations.


## October 20 — restarted dead-ball adjudication and placement hardening
- The Blueprint `OnRestartAwarded` event fires only when an unknown-touch referee award has a valid taker; menus can update restart details without treating uncertain attribution as fact.
- Provisional automatic corner kicks now aim inward toward the penalty area at either goal end instead of following team attack direction out of play.
- Restart eligibility rejects nonfinite referee marks and marks outside the configured pitch length/width, as well as 3D ball drift from staged positions.
- Source contracts cover event order, corner direction and restart location constraints; Unreal runtime rules and authored presentation still need validation.


## October 20 — keyboard referee decisions for the Windows prototype
- For a stopped undecided out-of-play event, F5 awards possession to home and F6 to away. The GameMode infers provisional throw-in/corner/goal-kick from crossing side and the selected side; it does not automatically fabricate the last touch.
- Then press B to stage the awarded restart, followed by M if manual mode is enabled. Referee decisions are rejected unless the restart is undetermined and an eligible taker exists.
- The registered Blueprint OnRestartAwarded event is broadcast after a successful decision. UI can inspect PendingRestartType/Team/Location/Taker.
- These are debugging/prototype keyboard controls rather than complete match officiating or menu UX.


## October 20 — opt-in physical knockout shootout sequence
- A drawn knockout tie still requires completed extra time. With `bRequirePhysicalPenaltyKicks` enabled, `StageCupPenaltyKick()` positions the next awarded kicker behind a penalty spot and places the ball at rest; `CanStageCupPenaltyKick()` exposes eligibility to Blueprint.
- `KickStagedCupPenalty(Direction)` requires the proper team, finite forward-oriented direction, valid player/ball and a ball close to its referee-assigned spot. A valid attributed physics impulse marks that attempt as taken.
- `IsCupPenaltyAwaitingResult()` lets UI prompt a referee to record scored/missed outcome through `RecordCupPenaltyAttempt`. The impulse does not automatically count as a goal; this is an interim manual ruling until physics/keeper/referee goals are implemented.
- `GetStagedCupPenaltyTaker()` gives camera and presentation Blueprints the prepared footballer. Staged references and mark are cleared on reset, mode changes, shootout start and recorded result.
- Removed duplicated GameMode penalty method definitions and added a source-contract regression to prevent reintroduction. Source CI is not an Unreal Engine native build.


## October 20 — Windows keyboard physical penalty trial
With the opt-in `bRequirePhysicalPenaltyKicks` setting enabled and a tied cup match finished after extra time:
1. **P** stages the next eligible kicker and ball.
2. **O** strikes with the staged taker's facing direction; the trajectory must point toward the goal and not be excessively high or downwards.
3. **F7** explicitly records a scored penalty, or **F8** records a missed attempt, after the strike. This consumes that turn and enables staging the next one.
The result is adjudicated manually for now; ball impulses and keeper outcomes are not automatically matched to official scoring. Source tests cover the input bindings, state transitions and safeguards, not actual Unreal gameplay.


## October 20 — physical shootout taker rotation
- The opt-in physically staged shootout attempts a deterministic 11-player lineup-slot rotation, preferring slots 9, 10, 8, 7, 6, 5, 4, 3, 2, 1, then 0 (goalkeeper). No authored footballer is used again until the cycle wraps at the 12th attempt for the same team.
- The prototype falls back to a valid nearby teammate when the selected lineup slot is not authored; full roster/eligibility enforcement, injuries, send-offs and substitutes remain future referee work.
- The current Unreal source and Python contracts support the arrangement but actual positional staging and match balance require UE5.5 Editor validation.
