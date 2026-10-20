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

### October 20 — penalty parry eligibility

A provisional keeper parry is now disallowed when the ball is farther than 35 cm beyond the keeper plane, outside the implemented 220 cm reach above the keeper actor origin, or has nonfinite location/velocity telemetry. Invalid telemetry also stops keeper path requests. Existing cooldown, incoming-shot and close-contact checks remain. These guardrails do not simulate diving, collision volumes, handling, or actual Unreal physics saves; engine compilation and match tests are still pending.

## October 20 — goalkeeper positioning batch (normal play and physical penalties)

Both goalkeeper pathways now use conservative, ball-velocity-based anticipation. During physical penalties, the defending goalkeeper only pursues a shot that is moving toward the defended goal; a backwards-moving or stalled ball returns the target toward the goal centre instead of producing an unnecessary lateral chase. Tracking is bounded by the actual GameMode half-goal width, less a 55 cm safety margin (and a 300 cm cap). Save eligibility continues to require an approaching ball, keeper proximity and reachable height, with editor-tunable minimum incoming speed, maximum parry height and post-plane overrun allowance.

In ordinary matches the keeper can predict an approaching shot at the keeper's X plane for up to 1.5 seconds; otherwise it retains conservative positional tracking. Regular match lateral targets also obey configured goal width. Both paths refuse nonfinite ball telemetry rather than feeding it into navigation. Source regression tests: `test_penalty_keeper_prediction.py` and `test_goalkeeper_match_anticipation.py`.

**Validation boundary:** These are source-only behavior and safety contracts. Real UE5.5 C++ compilation, animation, physics save quality, keeper navmesh movement and packaging remain unverified; tests have not been executed by a native Unreal editor. This is not a complete goalkeeper handling, catching or diving system.

## October 20 — off-ball decision integrity

The existing team AI controller now rejects nonfinite ball location or velocity before field-player pressing, formation-shift, passing, and marking decisions, stopping pending navigation instead of submitting invalid targets or ball impulses. Primary-pressure selection excludes goalkeeper slot zero consistently whether the roster uses explicit lineup slots or authored roster order. Opponent actors used in passing-lane evaluation and marking must belong to the controller's current Unreal world. These safeguards are covered by `unreal/tests/test_team_ai_ball_integrity.py` source-contract tests; they do not establish runtime behavior or gameplay balance.

**Next implementation priorities:** split tactical responsibility from navigation, implement coordinated team possession/defensive lines and pressure support, add in-engine unit/functional tests, and tune against a genuine Unreal 5.5 PC playtest. Continue without treating unavailable PC engine testing as a source-code blocker.

AI passing/marking candidate filtering now also skips teammate or opponent actors whose world positions contain NaN values, protecting passing-lane segment calculations, receiver scoring, and opponent marking targets from corrupt spatial data. The static integrity test covers these guards; native AI behavior still requires a real engine test.

## October 20 — bounded off-ball support and defensive recovery

Outfield AI now makes a small forward shift for the team most recently attributed a ball touch, or a small recovery shift if the opponents last touched it. This is a lightweight approximation of possession, **not** an authoritative possession state: a deflection, clearance or loose ball can misclassify it. Adjustable defaults are 220 cm attacking support and 150 cm defensive recovery, clamped to 500 cm; pitch bounds remain enforced after tactical movement. Existing primary-presser behavior overrides this shape shift. Source contracts: `unreal/tests/test_team_ai_shape_shift.py`. Unreal runtime evaluation, player coordination and true possession arbitration remain future work.

## October 20 — near-ball passing-lane support

The existing non-goalkeeper team AI now considers a lateral outlet shift when an attributed teammate last touched the ball and the support runner is within 24 m. It searches nearby opponents around the intended formation target; if one is close, it offsets laterally away from that opponent using editor-tunable settings (`PassingSupportLateralShiftCm`, 350 cm default; `PassingSupportOpponentSearchCm`, 1000 cm default). Offsets are bounded and final target coordinates remain pitch-clamped. The primary ball presser's chase target still overrides the support adjustment. Invalid opponent locations or teammates are ignored in the associated geometry/primary-pressure selection.

This uses last-touch attribution only as a weak proxy for possession, not a ball-control certainty. It does not establish advanced passing-pattern coordination, offside awareness or animation. Added source contracts in `test_team_ai_shape_shift.py` and `test_team_ai_ball_integrity.py`; Unreal Editor runtime playtests and navigation/physics balancing are still outstanding.

## October 20 — coordinated second-defender shadow pressure

When the opposing team was last credited with touching the ball, the second-closest autonomous outfielder may support the primary presser. It targets a bounded position behind and to one side of the ball rather than duplicating the primary pressure point. This is optional tunable source behavior: `SecondaryPressureRadiusCm` defaults to 1700 cm and `SecondaryPressureStandOffCm` to 450 cm. The selection excludes the primary presser, goalkeeper, invalid/world-mismatched pawns and human-controlled footballers. Match field boundaries still clamp the final navigation target. Source-contract coverage lives in `unreal/tests/test_team_ai_secondary_press.py`.

Last-touch attribution is not genuine possession tracking. This is an initial coordinated defending heuristic requiring real Unreal 5.5 compilation, navmesh tests, tactical gameplay balance, and future possession arbitration.

## Defensive line continuation — October 20

Slots 1–4 now use bounded recovery/advance relative to formation positions when the opponent last touched the ball. Defaults: 650 cm retreat and 1300 cm advance. Defenders only become a secondary presser when the ball is deep in their own half. This is provisional AI; native compilation and balance testing remain pending. Source tests: `test_team_ai_defensive_line.py`, `test_team_ai_secondary_press.py`.

### October 20 — lateral defensive compactness

Defender slots 1–4 now also limit their lateral movement relative to the authored formation position when the opposition was most recently credited with a touch. The `DefensiveLineLateralDisplacementCm` editor setting defaults to 650 cm, with a 1500 cm upper clamp. The constraint runs alongside existing forward/recovery limits and precedes pressure/marking overrides; therefore emergency pressing or later marking can still pull a defender away from the bounded formation target. Python source-contract assertions cover the setting and target calculation, not UE compilation or gameplay performance.

## October 20 — provisional attacking forward runs

When a team was most recently credited with touching the ball and play is in the attacking half, authored advanced lineup slots 9 and 10 can make bounded forward runs when they are not already far beyond the ball. The editor-tunable `ForwardRunOffsetCm` defaults to 650 cm and is clamped to 1500 cm. Final targets remain pitch-clamped. The player-selection logic still relies on weak last-touch attribution rather than verified possession, and does not implement offside awareness or tactical combination runs. A source-only regression test is in `unreal/tests/test_team_ai_forward_runs.py`; Unreal compilation and on-pitch evaluation are pending.

Attacking lineup slots 9 and 10 now use opposing lane offsets while making their provisional runs, rather than converging on a single line. `ForwardRunLaneSeparationCm` defaults to 250 cm and is bounded to 800 cm. These offsets are still superseded by nearby ball-pressing logic and final pitch-bound checks; they do not implement offside awareness or sophisticated run timing.

## October 20 — predicted receiver passing

During AI ball contact, candidate receivers are now led using a short 0.25-second velocity projection (receiver ground speed capped at 650 cm/s). The forward scoring, selected pass direction and opponent passing-lane segment use the predicted position. Invalid receiver velocity is excluded. This is a provisional movement heuristic, not a ball-flight solver; timing, interception and Unreal compilation require runtime testing. Source regression: `unreal/tests/test_team_ai_receiver_leading.py`.

## October 20 — distance-scaled AI pass strength

Candidate AI passes now use the selected receiver's predicted distance to choose a bounded impulse rather than the same power for short and long passes. The initial formula is 900 + 0.24 × receiver distance (in cm), clamped from 1000 to 1800 kg·cm/s; fallback clearances remain 1700. The primary presser still checks reach height before attempting ball contact. Source regressions in `unreal/tests/test_team_ai_receiver_leading.py` cover the calculation. Physics tuning, actual completion rates, and Unreal Engine 5.5 compilation are unverified.

## October 20 — distance-aware ball interception

Primary outfield AI pressers now scale short-horizon ball interception prediction using distance to the ball and an initial assumed 500 cm/s travel speed. Lead time remains bounded by the configured ball-prediction interval and a 0.5-second ceiling. This avoids over-leading the ball at close range. The algorithm remains a simple kinematic heuristic, not a pathfinder travel-time or collision solver. Source contract: `unreal/tests/test_team_ai_interception_lead.py`; actual native Unreal compilation and tactical playtesting remain pending.

Interception's assumed presser speed is now an Unreal-editor setting, `PresserTravelSpeedCmPerSec` (500 cm/s default, runtime clamped to 100–1000 cm/s). This is not a measurement of the actual pathfinding pawn movement speed. Regression checks were updated accordingly.

## October 20 — teammate possession-aware pressing gate

Primary AI pressure is suppressed when a *different*, valid teammate is the ball's recorded last toucher and stays within 220 cm of the ball. The attributed ball carrier remains eligible to act; invalid last-toucher coordinates are ignored. This avoids some unhelpful immediate teammate-on-teammate pressure, but it is a simple last-touch/proximity heuristic, not authoritative possession tracking or competitive AI. Source regression coverage: `unreal/tests/test_team_ai_possession_pressure.py`; UE5.5 gameplay validation pending.

Defensive opponent marking is now skipped while the player's own team is the recorded last toucher, so marking does not immediately override its attacking support and split-channel forward-run targets. Unknown last touch still permits marking. This remains a heuristic rather than a confirmed possession model; the regression test above covers the gating condition.

## October 20 — loose-ball possession approximation

Formation attack/recovery shifts, forward runs and secondary-pressure heuristics now treat last-touch attribution as possession only while the recorded toucher is a valid actor in the same world and remains within 300 cm of the ball. Once the ball moves farther away, the decision falls back to unknown possession rather than extending a stale tactical assumption. This does not yet implement robust contested possession, interceptions or transition timers. Existing marking and teammate proximity checks retain their separate conditions. Source regression: `unreal/tests/test_team_ai_possession_proximity.py`. Unreal engine build and runtime testing remain outstanding.

Defensive marking also resumes when the attributed teammate has moved beyond the 300 cm control radius, rather than treating any historical last touch as continuous possession. A matching source regression was added to `test_team_ai_possession_proximity.py`.

## October 20 — controlled ball-carrier pressing exclusion

The primary presser now excludes itself when it is the recorded last toucher and remains within 220 cm of the ball under the proximity-validated attribution. This avoids treating a footballer actively controlling the ball as an opponent to chase. Separate suppression for a nearby teammate remains. Loose-ball recovery continues when last-touch proximity is lost. Source-only regression: `test_team_ai_possession_pressure.py`. Ball-carrier dribbling and native match integration remain unfinished and require in-editor testing.

Secondary supporting defenders now retain their shadow-pressure target instead of having it overwritten by the subsequent generic marking stage. The `bIsSecondaryPresser` flag is local to each AI decision and prevents that conflict without changing regular marking eligibility. Source regression checks were added to `test_team_ai_secondary_press.py`; UE navigation playtesting remains pending.

## October 20 — initial autonomous ball-carry touch

An AI outfielder who is the recent, nearby ball toucher can now apply a modest forward physics-based controlled touch while the existing action cooldown is ready. The touch requests 380 cm/s toward the opposing goal, uses the ball's existing `ApplyControlledTouch` contact and height checks, and sets a shortened cooldown after a successful touch. This is a basic forward dribble, not contextual shielding, skill moves, collision-aware dribbling or guaranteed control. Source regression: `unreal/tests/test_team_ai_ball_carrier_dribble.py`; native gameplay, controller movement and ball-contact tuning remain unverified.

A self-attributed ball carrier also sets a short attacking movement target ahead of the ball (280 cm) and protects that target from the generic marking stage between touch impulses. This remains a simple forward-carry heuristic pending Unreal movement and collision testing.

## October 20 — opponent-aware controlled dribble

While a valid AI ball carrier is near its attributed touch, it now scans opposing outfield actors within 500 cm of the ball and biases its next controlled touch laterally away from the nearest defender in or near the forward path. The forward component remains, while the lateral component is limited to 0.6 in the desired-velocity vector; existing physics contact and cooldown protections still apply. This is an early avoidance heuristic, not obstacle-aware pathfinding, shielding, feints or professional-quality dribbling. Source regression: `unreal/tests/test_team_ai_ball_carrier_dribble.py`. Real Unreal 5.5 playtests are pending.

### October 20 — dribble and movement alignment

When the AI ball carrier selects a lateral escape touch around a nearby defender, its navigation target now shifts 180 cm into that same lane. The desired touch velocity is also moderated from 380 cm/s in open space to 280 cm/s when avoiding a nearby defender. These are provisional heuristics, not collision-proof shielding or animations; source tests were expanded and native UE gameplay evaluation remains pending.

### October 20 — dribble direction consistency

The controlled ball carrier now checks its lateral escape direction on each AI decision, including frames when the ball-touch cooldown is active. Both its navigation target and subsequent controlled touch use the same nearest-defender escape direction, so a player no longer straightens its running path between touches. The duplicated opponent scan was removed from the touch action. Regression coverage updated in `unreal/tests/test_team_ai_ball_carrier_dribble.py`; this remains unverified in Unreal gameplay.

AI avoidance now prefers an infield steering side if a projected 350 cm lateral escape would run within 250 cm of the pitch touchline. The same adjusted side is reused by both runner movement and its next controlled touch. Source-contract tests were extended; pitch-specific gameplay validation is pending.

### October 20 — frontal dribble challenge

When a defender is within 150 cm immediately ahead of a controlled ball, AI holds the next forward touch rather than pushing the ball straight into that defender. The carrier continues evaluating an avoidance lane, and existing action cooldown remains in effect. This is an early possession-protection heuristic, not a complete pass-versus-dribble decision or shielding animation. Source regression contracts in `unreal/tests/test_team_ai_ball_carrier_dribble.py`; native compilation and runtime testing are pending.

Dribble frontal challenge distance: `DribbleChallengeRadiusCm` defaults to 150 cm and is clamped to 50–400 cm when used. Tune it during native playtesting.

### October 20 — challenged-carrier outlet passing

When a defender is directly challenging the attributed ball carrier, the AI can attempt a short emergency pass to the nearest same-team outfielder within 350–2200 cm rather than freezing and holding its forward dribble. The candidate is checked for actor validity, world, team and finite location; the pass requires reachable ball height and the existing ball-contact safeguards. A successful release triggers the action cooldown. This is preliminary and does not yet check outlet passing corridors or tactical pass scoring. Regression contract: `unreal/tests/test_team_ai_ball_carrier_dribble.py`.

Emergency outlets now exclude teammates whose passing segment passes within 160 cm of an opponent, including opponents at the receiving point. This uses a two-dimensional nearest-segment check, not a timed interception simulation; source-contract tests were updated accordingly.

Pressure outlet selection now ranks unblocked candidate passes by forward progress and distance rather than always selecting the nearest teammate. The ball impulse is distance-adjusted (950 + 0.22 times outlet distance, clamped 1000–1550) to better support both short and longer emergency releases. This remains an untuned heuristic requiring UE gameplay tests. Source regression coverage was updated in `test_team_ai_ball_carrier_dribble.py`.

Pressure outlets also project moving receiver velocity 0.20 seconds ahead, with horizontal velocity capped at 650 cm/s. The projected point is used for candidate distance, interception corridor and release direction; invalid receiver velocities are excluded. This is not a full time-of-flight solver and must be tuned in UE5.5.

Pressure outlet scoring now adds a penalty for projected receiving positions within 450 cm of either sideline, helping prefer central outlet options when otherwise comparable. The score remains a heuristic and can still select a wide pass when other choices are worse.

Normal AI pass selection also penalizes predicted receiver locations within 400 cm of the sideline. This joins existing opponent-proximity and progress scoring, without disallowing productive wide passes outright. Regression coverage: `test_team_ai_receiver_leading.py`.

Defender interception prediction now uses a zero-velocity fallback if ball velocity is nonfinite, keeping the pursuit target finite without modifying ball physics. The interception source-contract test checks this safeguard. Native movement and engine tests still need execution.

AI forward dribble requests now check the ball is within the same 115 cm vertical reach boundary enforced by `ApplyControlledTouch`, preventing futile high-ball attempts at the caller. Source regression contracts were updated; native match behavior remains untested.

Primary AI defender interception targets are additionally clamped to within a 100 cm infield buffer on both pitch axes, even if a velocity-based projected ball position crosses the sidelines or goal line. Source regression coverage is in `test_team_ai_interception_lead.py`.

## October 20 — predictive defensive marking

General outfield marking now aims toward an opponent's projected position using a 0.20-second velocity lead capped at 650 cm/s. Invalid opponent velocity is treated as stationary, and existing marking influence and final pitch bounds remain. This is a short-horizon heuristic rather than defensive tracking/pathfinding. Source checks: `unreal/tests/test_team_ai_predictive_marking.py`; Unreal gameplay and compilation validation remain pending.

The marking point also maintains an initial 120 cm goalside offset relative to the opponent's predicted movement, before existing blend and pitch clamping. Source regression updated; this offset is not yet dynamically tuned by defensive formation or tackling state.

General marking now ignores a nearest opponent positioned more than 900 cm behind the ball in the team's defensive orientation, reducing pursuit of players away from the immediate threat. This is an approximation and must be checked in formation playtests.

Goalside marking separation is now scaled to 12% of the marker-to-attacker distance, clamped to 80–180 cm, rather than a single 120 cm offset. This is an initial tunable behavior subject to engine playtests.

Marking threat filtering retains opponents deep in the defending team's own half (beyond 45% of half-pitch length) even when more than 900 cm behind the ball; this prevents losing track of goal-threatening attackers during loose-ball recovery.

The defensive marking search radius is now runtime-clamped to 0–3000 cm; very large misconfigured values cannot create unconstrained opposition searches. The existing Unreal setting remains editable.

When several valid players are markable, a deep goal-threatening opponent receives a modest 15% squared-distance weighting in opponent selection. This prioritizes urgent defensive cover without abandoning the search-radius limit.

Goal-area attackers are weighted more heavily in defensive selection: opponents within 1600 cm of the defending goal line receive a 0.75 squared-distance multiplier in addition to the deep-threat priority. This is preliminary spatial scoring, subject to native playtests.

The goalmouth threat bonus is restricted to attackers in the central 1800 cm of pitch width, avoiding over-prioritization of wide attackers near the end line. Source tests check both spatial gates.


### October 20 — standing tackle contest safety

User-initiated standing tackles now commit their recovery cooldown on input, including whiffs; previously a miss could be immediately retried. The tackle chooses the closest finite, in-range, forward-facing ground ball rather than taking the first ball in actor iteration order. It refuses a ball with a recorded friendly last touch and also refuses a ball currently within 115 cm horizontally and vertically of another friendly footballer, even if the last attributed touch belongs to an opponent. Successful contact continues to use `KickByPlayer` and tackling ratings. This proximity heuristic is not a possession authority or foul/referee decision. Added `unreal/tests/test_standing_tackle_recovery.py` source contracts. Native Unreal compile and collision playtests remain unverified.

### October 20 — fatigue-sensitive footwork

Low stamina now scales both character top speed and acceleration smoothly from 72% at empty capacity to full movement performance at 40% or greater of available stamina. This works alongside existing pace and acceleration ratings and the sprint-exhaustion lockout. Jockey movement uses 2800 cm/s² braking deceleration instead of the standard 2100 cm/s², for tighter defensive stopping without changing normal run braking. Added `unreal/tests/test_fatigue_movement.py`; no native Unreal compile or player animation validation yet.

### October 20 — manual/AI controlled-touch arbitration

The automatic walking-dribble touch in `SportsArcadeCharacter::Tick` now runs for player-controlled pawns only; AI pawns continue to execute controlled touches through their existing tactical controller and cooldown rules. For local human ball contact, the character also checks whether a friendly footballer is closer to the grounded ball within a 115 cm horizontal/vertical contact envelope. If so, the manual dribble impulse is skipped so simultaneous foot proximity does not force a contested friendly takeover. These are heuristics, not a server-authoritative possession system; source tests are in `unreal/tests/test_manual_dribble_arbitration.py`, and UE native playtests are pending.

### October 20 — directional defensive contests

Standing-tackle impulses now bias the ball away from its most recent opposing toucher when that opponent remains valid. A 65% defender-forward / 35% away-from-opponent blend is normalized, with a forward-facing fallback for degenerate or excessively backward directions. Existing finite contact validation, friendly-control exclusion and missed-tackle cooldown remain. This is not a physical body-contact or referee foul simulation. Source contracts updated in `test_standing_tackle_recovery.py` and `test_project_structure.py`; native UE5.5 verification is pending.

Holding defensive jockey input now suppresses automatic walking-dribble contact on the local player, avoiding unintended ball impulses while positioning defensively. Explicit pass, shot and tackle controls retain their separate contact checks. Source tests updated; native feel and animation tests remain outstanding.

Directional standing-tackle deflection now checks that the previously attributed opposing player remains in the same world and within 220 cm horizontally of the challenged ball. Distant historical last touches no longer skew the deflection vector; the fallback is the defender's forward direction. Source tests updated; native gameplay still requires Unreal validation.
