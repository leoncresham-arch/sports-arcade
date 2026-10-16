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
