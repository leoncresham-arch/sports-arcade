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

Standing tackles now consume a configurable 5 stamina points on the attempt, including a missed challenge, clamped at zero. Combined with cooldown and low-stamina foot-speed/acceleration penalties this discourages repeated lunges. The balance value is provisional until physical UE playtests. Source contracts cover debit-before-contact and cooldown behavior.

### October 20 — endurance-scaled defensive force

A standing tackle now calculates effective strength from **pre-attempt** stamina relative to the footballer's rating-adjusted capacity: a clamped 60%-100% multiplier reaching maximum at 35% stamina. The existing 5-point attempt cost remains, even on a miss, and the rating-driven tackle force is still applied separately. This is provisional gameplay tuning, with source regressions in `test_standing_tackle_recovery.py`; physics and animation balance require native UE verification.

Standing-tackle candidate screening now ignores a ball moving horizontally faster than 1200 cm/s when the velocity points away from the defender, and rejects nonfinite ball velocities. It does not block slower contests or fast incoming balls. This is an approximate reach filter; native Unreal physics/contact testing remains necessary.

### October 20 — contested human dribble touch

The local player's automatic dribble touch now skips a ball when an opponent in the same world is within 95 cm horizontally / 115 cm vertically of the ball and is closer to it than the human-controlled player. This supplements friendly proximity arbitration and makes possession challenges depend on spatial advantage rather than allowing automatic proximity steals. Explicit tackling remains separate. The filter is heuristic, not authoritative possession or animation-level contact. Source regression coverage is in `test_manual_dribble_arbitration.py`; native UE gameplay remains unverified.

Manual pass/shot/chip candidate selection now excludes nonfinite, vertically unreachable (>120 cm) and behind-the-player balls before choosing the nearest valid target. This avoids choosing a nearest but unkickable reference ball while ignoring another valid candidate. Final `KickByPlayer` checks remain authoritative for contact distance and physics validity. Source contract updated; UE playtest pending.

Manual pass/shot/chip now rejects nonfinite strength, treats nonfinite player movement input as absent, falls back to facing, and refuses a nonfinite or zero resultant strike direction. These are defensive numerical checks, not a guarantee of physical correctness. Source contracts updated, with actual UE5.5 engine validation pending.

### October 20 — manual strike numerical validation

Manual pass, shot and chip inputs now validate finite strike strength, movement input, player location, player facing and resultant aim before applying a ball impulse. Invalid movement input falls back to valid facing, while invalid facing prevents selection outright. Source regression tests were updated to match guarded normalization and contact screening. Source CI is not native Unreal compilation or gameplay validation.

### October 20 — finite manual dribble telemetry

Manual controlled-touch selection now validates the ball actor and finite human location, velocity and facing before proximity scans and impulse requests. A degenerate horizontal facing vector is rejected. These guards avoid invalid movement telemetry causing uncontrolled contacts; they do not substitute for UE native testing. The source-only regression is `unreal/tests/test_manual_dribble_arbitration.py`.

Successful standing tackles now impose a 0.35-second local automatic dribble-touch cooldown, preventing a defender from immediately undoing the ball deflection on the next frame. Misses retain their tackle cooldown and stamina cost but do not impose this extra touch delay. Covered by `test_standing_tackle_recovery.py`, pending UE gameplay validation.

### October 20 — contested dribble actor validation

Manual possession-proximity scans now reject invalid teammate and opponent actor references and players from a different world before reading their position. This hardens level transition and actor teardown edge cases, while retaining spatial contest arbitration. Regression contracts in `test_manual_dribble_arbitration.py`; native Unreal playtesting still pending.

### October 20 — consistent sprint release

Releasing sprint now recomputes movement speed using the same rated stamina/capacity and 72%-100% fatigue scaling as the regular movement tick. It also preserves jockey speed while jockey is held instead of momentarily switching to normal jog. Source tests cover immediate sprint-release behavior; no Unreal runtime input validation yet.

### October 20 — deliberate manual dribble movement

Human automatic dribble touches now require a valid world, finite velocity and more than 100 cm/s horizontal movement. The former 20 cm/s effective threshold could trigger minor ball impulses during nearly stationary player movement. This keeps explicit passes, shots and tackles available while preventing incidental slow foot-contact automation. Source regression added; UE playtesting and tuning remain pending.

### October 20 — directional player-controlled dribbling

Automatic human dribble contacts now require the player to travel toward the ball (positive 0.2 planar dot-product threshold) as well as face it (existing 0.35 threshold). This prevents backwards or sideways momentum from nudging a ball in front of the mesh and better separates foot-contact intent from facing. Source checks in `test_manual_dribble_arbitration.py`; native movement and animation alignment remains unverified.

Player-controlled automatic dribbling now evaluates all valid, reachable, forward-travel-aligned balls and applies its controlled impulse to the closest eligible candidate rather than the first actor returned by world iteration. Existing friend/opponent contact arbitration and cooldown still apply. This eliminates non-deterministic ball choice when authored maps contain multiple ball actors; source regressions passed on the code/test checkpoint, and native UE runtime tests remain necessary.

Standing tackle teammate-protection scans now validate each teammate actor and its owning world before reading its team and location. This aligns defensive contact arbitration with the manual dribble validation and guards against stale actors during resets. Source regression extended; native UE behavior still awaits compilation/playtesting.

### October 20 — loose ball versus last-touch distinction

The standing-tackle friendly-last-touch gate now checks for a valid same-world teammate within 135 cm horizontally and 115 cm vertically of the ball. The previous unconditional last-touch exemption could prevent an eligible defender from tackling a loose ball long after its teammate had touched it. The separate nearest-friendly-control safeguard still runs. This remains a proximity heuristic, not a referee-grade possession state; source regressions added and UE runtime validation outstanding.

Primary AI pressing now caps the finite ball velocity used for interception lead prediction at 3500 cm/s (retaining nonfinite velocity fallback). This avoids extreme physics spikes commanding an exaggerated future intercept before pitch-bound clamping. Ball physics itself remains unchanged; lead tuning and movement need UE native playtests. Source test updated in `test_team_ai_interception_lead.py`.

### October 20 — pitch-bound AI receiving decisions

Both routine AI teammate passes and emergency challenged-carrier outlet passes now reject receiver positions projected beyond the pitch limits (100 cm interior margin on X/Y). Existing receiver velocity lead, lane screening and touchline-risk scoring remain. This prevents selecting an out-of-play target before evaluating pass quality, without making every wide pass illegal. Defensive standing tackles now also verify the ball actor remains valid before evaluating contact. Python source contracts added/updated; native Unreal compile and playtesting remain outstanding.

### October 20 — finite AI receiving locations

Regular and emergency outlet AI passes now reject any nonfinite extrapolated receiving target before touchline and goal-line bounds screening. The existing bounded receiver-velocity lead and pass-lane checks remain. Corresponding source tests have been extended; native Unreal compilation and match simulation remain outstanding.

Emergency AI outlets now reject degenerate or nonfinite pass displacement vectors before assessing receiver position and marking. Existing bounds, pressure and cooldown logic remain in place. Source test updated; native UE gameplay testing remains outstanding.

Regular AI teammate passing now rejects degenerate or nonfinite pass displacements before scoring projected receivers. This matches the emergency outlet guard and retains existing receiver lead, pitch bounds, marking and action-cooldown safeguards. Source regression updated; UE5.5 runtime verification remains pending.

### October 20 — defensive AI teammate possession height gate

AI primary presser suppression for a teammate's recently touched ball now requires the previous teammate to belong to the same game world and to remain within 115 cm vertically of the ball, in addition to the existing 220 cm horizontal control radius. This prevents airborne or stale-world touch attribution from indefinitely stopping loose-ball recovery. Source regression extended; native UE runtime tests pending.

Secondary AI pressing now clamps its lateral support/shadow target to the playable pitch (100 cm inner margin). This prevents the second defender from trying to navigate outside the touchlines when the ball is near a boundary. The existing presser selection and stand-off spacing remain unchanged. Source regression added; native UE navigation testing remains pending.

### October 20 — anticipatory second defender

The secondary AI presser's support/shadow destination now leads the ball for at most 0.25 seconds using horizontally bounded 1600 cm/s velocity, rather than always positioning off the ball's current location. Nonfinite velocity falls back to zero, and the established playable-pitch clamping still applies. This is early coordinated pressing behavior, not a complete tactical defense simulation. Added static regression coverage in `test_team_ai_possession_pressure.py`; UE native compilation and playtesting remain pending.

Secondary AI pressing now maintains a minimum 120 cm lateral shadow displacement around predicted ball position, while keeping its existing standoff, ball anticipation and pitch clamping. This encourages a second defender to block a side escape rather than entirely converge on the primary defender's route. Source regression updated; real UE navigation and balance are unverified.

### October 20 — secondary pressing infield containment

When the predicted ball position is within 650 cm of a touchline, the supporting AI defender now shades its lateral positioning towards the pitch interior. Away from the sidelines, its existing same-side support behavior remains. Ball lead, minimum 120 cm shadow and boundary clamping are preserved. Source regression updated; UE navigation/gameplay verification is pending.

### October 20 — coordinated defender intercept safeguards

Secondary pressing now anticipates inward-moving balls by shading toward the central escape lane, including when a ball starts away from the touchline; predictions are rejected when nonfinite. Primary press interception navigation likewise checks the predicted destination before issuing a movement target. These changes retain the existing anticipation speed caps, lateral support separation and pitch clamping. Regression tests cover both paths; actual Unreal 5.5 compilation, animation/contact alignment and match-play balancing remain unverified.

Goalkeeper incoming-shot prediction now caps validated ball speed at 4500 cm/s before estimating goal-line arrival and lateral interception, limiting exaggerated keeper shifts under extreme finite physics impulses. The raw velocity is still screened for nonfinite data. Added a goalkeeper regression assertion; real UE keeper movement and saves remain unverified.

### October 20 — defender prediction refinement

Secondary AI support only treats a ball as moving inward when it is at least 500 cm off center and has more than 150 cm/s lateral inward velocity, avoiding oversized reactions to tiny movement. The nearest ground presser shortens its prediction horizon to 0.12 seconds while the ball is more than 150 cm above its own position, reducing over-aggressive pursuit of aerial extrapolations. These are provisional source-level heuristics awaiting Unreal playtesting; corresponding regression contracts were updated.

### October 20 — rising-ball defensive anticipation

Both the primary presser and supporting second defender now shorten aerial ball prediction further: when the ball is more than 150 cm above the defender and rising faster than 100 cm/s, their extrapolation horizon falls to zero instead of projecting an implausible foot-level interception. Descending airborne footballs retain bounded short lead, while grounded trajectories keep the previous prediction window. Source regressions have been adjusted; native Unreal physics, animation and pathfinding tests remain outstanding.

### October 20 — goal-side marking lane cover

Off-ball AI marking now biases a defender's goal-side shadow laterally toward the centre when the tracked attacker is wide. The correction starts beyond 1100 cm from the centre line, scales gradually and is bounded by the existing marking influence and final pitch clamps. This is a tactical source-level heuristic, not physics-based man marking. Tested by `test_team_ai_possession_pressure.py`; native Unreal navigation and match-play tuning remain outstanding.

### October 20 — off-ball marking telemetry guard

Off-ball AI now rejects a nonfinite opponent-predicted marking point before computing goal-side and central-lane cover or issuing a navigation request. This guards against invalid composed location/velocity data. Source regression updated; native Unreal compilation and movement testing remain outstanding.

### October 20 — urgent goalmouth marking

When a marked opponent approaches the defending goal, the off-ball AI reduces its generic lateral infield-shading adjustment (bounded between 35% and 100% of the normal shift), prioritizing direct goal-side coverage. Existing target interpolation and pitch limits remain. This is a source-level tactical heuristic awaiting native Unreal navigation and playtesting.

### October 20 — direct-run defensive assignment batch

Off-ball marking now increases the goal-side offset by up to 45 cm when a marked opponent is running directly toward the defending goal (horizontal velocity threshold 150 cm/s), within the existing 80–180 cm total bounds. When deciding which nearby opponent to mark, the defender also favors direct runs toward goal within 2500 cm of the goal line, applying an 0.82 priority factor to their distance score (only for valid finite velocity). Together with the goalmouth-sensitive central-shading reduction, these changes improve the tactical foundation while retaining the current AI controller and pitch constraints. Python source contract tests were updated; native UE5.5 compilation and gameplay behavior remain unverified.

### October 20 — central and diagonal runner defense

Off-ball defenders now weight actively advancing central attackers (within 1000 cm of midfield's Y axis) more urgently when selecting an opponent to mark. A tracked attacker moving rapidly towards goal and diagonally infield triggers an additional 75 cm infield coverage shift (with final navigation pitch bounds retained). This develops direct-run recognition beyond the preceding goal-side adjustment. Source-contract tests cover both decisions; native Unreal runtime behavior remains unverified.

### October 20 — combined movement, control, tactical AI batch

Item 1 (team AI): opponent threat weighting now preserves the actual maximum marking radius; urgency scoring cannot draw a defender to a physically distant opponent. Item 2 (movement): player braking increases on opposing travel and intended input directions using a configurable turn-braking coefficient, preserving jockey behavior. Item 3 (ball control): controlled touches now scale with player travel speed (bounded 0.65–1.15), multiplied by the existing BallControl rating and a configurable touch scale. These are incremental implementations toward the three larger scopes, not completion claims. Static source tests added; Unreal compilation, frame tuning, animations and playtesting are still required.

Additional ball-retention behavior: automatic human controlled touches now reject fast-moving balls receding from the footballer (relative speed over 1150 cm/s and departing trajectory); standing challenges and deliberate strikes remain independent. Added source regression. These are not substitutes for animation-accurate contact and Unreal playtesting.

Coordinated AI team-possession support and second-pressure gating now require the last attributed toucher to be within 220 cm horizontally and 135 cm vertically of the ball; airborne/separated last-touch history no longer claims active control. Primary presser can still recover loose balls, and support assignments retain existing boundaries. Regression coverage added.

### October 20 — midfield attacking support

AI central midfield slots 5–8 now shift up to 450 cm forward to create a short passing option when their team has proximity-validated possession in the attacking half and the player is within 2400 cm of the ball. This adds a bounded transition movement without changing formation assignment or requiring a new tactical director. Source test added; UE movement/navmesh and passing evaluation are pending.

### October 20 — manual shield controls and close-touch pacing

The existing human footballer now exposes an editable/Blueprint-readable shielding state via IsShieldingBall, engaged with Left Control and cleared on release or kickoff reset. Shielding suppresses sprinting, caps running to the configurable jockey pace and reduces automatic close-dribble touch impulse to 55% of the normal value. This is an input/gameplay foundation only: opponent body collisions, animation-based shielding, physical ball protection, fouls and native controls must still be implemented and tested in Unreal.

### October 20 — agility and planted turn responsiveness

Character movement now uses an original editable 1–99 Agility attribute to scale character rotation rate, with fatigue reducing turning responsiveness; yaw turn rate is capped 260–760 degrees/s. A new source regression verifies rating connection. Visual foot planting, directional animation blends and physics-tuned acceleration still need genuine Unreal implementation and PC playtesting.

### October 20 — turning-ball control and midfield pass availability

Manual controlled touches now blend current travel direction (75%) and player facing (25%), improving directional touch continuity while turning. Attacking midfield support targets also remain at least 175 cm behind the ball in the current attacking direction, avoiding an overrun that removes an easy supporting pass. Regression contracts added for both paths. This advances items 1 and 3, but neither system is complete without Unreal runtime tests and animation/contact integration.

### October 20 — shield handling pass

Shielding now adopts the same controlled-braking behavior as jockeying. Manual controlled touches also bias toward player-facing direction while shield is held (55% facing, 45% travel), instead of using the normal 25% facing / 75% travel weighting. Regression assertions updated; the behavior remains an uncompiled, animation-agnostic foundation.

Midfield attacking support now maintains a minimum 250 cm lateral separation where possible from its team's carrier, alternating the side by lineup slot to offer an elementary passing triangle while maintaining the existing 175 cm rear support line. Source regression added; formation/pathfinding balance is not verified in Unreal.

Midfield passing-triangle support now preserves a player's established side of the ball when widening a narrow passing lane. For near-central alignment it still alternates by lineup slot. This corrects a case in which fixed slot-side assignment could pull a midfielder across the carrier and shrink the lateral lane. Source regression updated; runtime team tactics remain unverified.

### October 20 — fatigue-aware controlled touches

Manual controlled-touch steering now scales travel-versus-facing blend by the player's normalized remaining stamina, reducing momentum-dominant dribbling when exhausted, while preserving the established shielding bias. Movement stamina rating affects the denominator; the blend remains bounded. Static regression updated, native Unreal physics/animation tests outstanding.

### October 20 — infield midfielder outlet near touchline

When the ball carrier is within 600 cm of a touchline, a supporting midfielder now positions 350 cm toward the pitch center to give a safer inside passing option rather than spreading beyond the sideline. Other midfield support remains on the player's existing side where possible. Tests assert the boundary-specific branch; native UE tactical validation pending.

Touchline support correction: the midfielder's 350 cm inward outlet is now applied irrespective of the earlier 250 cm lateral separation test. Wide carriers therefore receive the same central option even when midfield formation already happens to be offset to the side. Source regression updated; UE runtime validation remains pending.

### October 20 — movement turn braking and close shielding cadence

Directional turn braking now varies continuously with the angle between velocity and requested movement, rather than switching suddenly at a ninety-degree threshold. Manual shielding now triggers smaller close-ball touches at 70% of the normal touch interval (minimum 0.05 seconds), with the previously lowered touch force retained. Source regression updated; UE5.5 compilation, animation and runtime collision balance are outstanding.

Coordinated midfielder support additionally clamps its lateral target within 1100 cm of the slot's scaled formation Y position. This prevents cross-field attacking support from unintentionally pulling a midfielder out of their tactical corridor while retaining the earlier behind-ball and infield-outlet priorities. Source regression expanded; actual native AI movement and pathfinding still require verification.

Shielding possession now also applies a 25 cm margin when arbitrating a nearby opponent's attempted automatic touch. A clearly closer opponent still wins this foot-contact check, and the existing vertical/contact/forward-travel restrictions remain. This is provisional gameplay logic rather than full body-contact simulation. Regression added.

### October 20 — deliberate stationary first touch

Player control now includes an explicit Receive action bound to I. Unlike automatic dribbling (which requires player movement), deliberate receiving looks for the nearest grounded ball within the configurable controlled-touch radius, rejects excessively fast passes (above 1800 cm/s) and invalid telemetry, checks the ball is reasonably within the player's facing hemisphere, and defers to opponents or teammates with clearly superior nearby foot contact. When eligible, it uses the existing physics-based ApplyControlledTouch to slow the ball toward the facing direction and applies a cooldown. Shielding decreases the desired output speed further. This is source code only; chest/thigh traps, aerial receiving, foot contact animation and Unreal runtime validation remain outstanding.

### October 20 — larger gameplay controls follow-up

Deliberate receiving now considers ball velocity relative to the footballer's own finite movement, allowing practical moving interceptions while retaining the existing 1800 cm/s relative-speed limit, contested touch checks, and cooldown. Planted braking now scales with running speed so slow shuffles are not over-braked. Midfield support performs a final behind-ball depth check after formation channel adjustments, protecting the minimum 175 cm passing option. Source regression tests updated. No UE5.5 native compile, motion-capture/foot-contact animation, or Windows match playtest has been performed.

### October 20 — receiving reach and transition response

Deliberate receiving now requires full 3D ball-to-player distance within the controlled-touch reach in addition to contact-height checks, avoiding impossible touches on elevated passes. The central-midfield forward-support shift is suspended during fast backward-moving passes (ball velocity more than 450 cm/s against attacking direction), preventing a surge in the wrong phase of possession. Regression contracts added. These changes still require native Unreal evaluation.

Shielding also now scales commanded movement acceleration to 72% during active play, on top of existing speed, braking and fatigue constraints. This helps provide a controlled footing baseline prior to implementing physically verified player-body contact and animation-based shielding.

### October 20 — moving reception and fast switching

Explicit first touch now blends player-facing direction (65%) with the receiver's movement direction (35%) to cushion a moving reception into travel while preserving stationary control and existing reach/contestation limits. Midfielder forward-support surges are also held on rapid lateral ball switches (over 1100 cm/s sideways), allowing the formation to reset before joining play. Source regressions updated; native Unreal contact physics, animation and tactical tuning remain outstanding.

First-touch defensive contest comparison now applies the full 25 cm spatial margin to the opponent's *distance* instead of adding squared distances, so only a meaningfully nearer opponent blocks the reception. Source regression covers the corrected units.

### October 20 — shielding endurance

Active, moving shielding now draws a tunable 4 stamina units per second instead of recovering endurance while holding off challenges. Stationary shielding and inactive match phases keep normal recovery; sprinting continues to use its separate larger drain. Existing fatigue-dependent movement and control scale from the resulting stamina. Static source regression added; physical balance remains unverified in UE.

### October 20 — pressured possession and shielding endurance

The midfield forward support offset now pauses when an opponent is within 220 cm of the ball, keeping close passing support during immediate defensive pressure. Moving shields only drain while stamina is available, and the reduced shielding acceleration no longer applies when endurance reaches zero. Static source tests cover both cases. Native UE5.5 compilation, animation/collision and gameplay tests remain outstanding.

### October 20 — stamina-gated shielding and rated reception

Automatic close-control touch force and touch cadence now apply the shielding bonus only while stamina remains above zero; first-touch cushioning similarly loses its shielding advantage when exhausted. Explicit reception additionally uses the footballer's BallControl attribute with a bounded rating factor (0.6–1.4). Source tests updated; native Unreal gameplay remains unverified.

### October 21 — controlled receiving and defensive pressure

Deliberate first-touch direction now blends toward running direction proportionately to player travel speed, reducing unintended dribble direction changes from nearly stationary velocity noise. Midfielder pressure evaluation now requires opposing footballers to be within 135 cm vertically of the ball as well as 220 cm horizontally, avoiding irrelevant airborne contact attribution. Static regression checks updated. Native UE5.5 compilation and match validation remain outstanding.

Deliberate receiving additionally rejects high-speed balls travelling away from the receiver (relative speed above 450 cm/s and departing direction), avoiding implausible foot-traps of already-escaping passes. Source regression extended; UE contact timing still needs real playtesting.

### October 21 — sprint pivot response

Max sprint pace now temporarily decreases during acute reversals or heavy pivots, with the penalty bounded and partially mitigated by the footballer's Agility rating. Jog, jockey and shielding speeds remain on their independent paths, and existing turn braking and fatigue modifiers still apply. Static regression added; native compile, animations and tuning are pending.

### October 21 — pivot momentum and closing threats

Acute sprint pivot speed reductions now scale by footballer momentum rather than applying full penalty as the footballer begins moving. The midfield carrier-pressure check now disregards clearly receding defenders (moving away above 350 cm/s) while retaining vertical contact and 220 cm proximity constraints. Static regression contracts updated. Unreal compile and runtime verification remain outstanding.

Explicit receiving now adjusts output touch speed according to player-relative incoming ball speed, bounded at 65–100% of the previous target. Faster passes receive a softer forward touch; the physics impulse and contact radius remain constrained by the existing ball component. Added regression coverage, pending native Unreal evaluation.

### October 21 — ball-relative pressing and touch absorption

Midfielder support pressure now evaluates defender closing movement relative to the current ball motion, in addition to distance, height and clear recession; this reduces false pressure decisions during lateral ball movement. First-touch output now separates skill-sensitive cushioning from the incoming-speed factor: higher BallControl ratings generate a more restrained settling touch. All changes are source-level and still require Unreal native compile and playtests.

Exhausted manual shield carriers no longer receive the special reduced max-walk-speed movement profile; alongside previously disabled acceleration and touch advantages, this keeps shielding behavior consistently endurance-gated. The existing fatigue speed modifier still applies. Regression updated.

### October 21 — finite physics guard and movement turn severity

The deliberate receiving action now verifies that a selected ball still exists and its relative velocity remains finite before producing a touch. Midfield support classification likewise guards against non-finite ball-relative defender motion. Momentum-based sprint pivot penalties now explicitly account for acute reversals. Static source regressions expanded; native Unreal compilation and gameplay tests remain open.

### October 21 — cornering and progressive midfield runs

Footballer acceleration now reduces smoothly through sharp momentum-bearing turns, with Agility rating moderating the penalty. Shield-specific braking is disabled upon exhaustion. Attacking midfield forward shifts now scale gradually with available depth rather than snapping to the full forward offset as soon as the carrier gains possession. Source tests cover the changes; true UE 5.5 compile, foot placement and match AI verification remain unavailable in the current environment.

### October 21 — controlled-touch physics impulse

The shared native ball physics controlled-touch path now caps each velocity correction between 180 and 280 cm/s according to incoming ball horizontal speed. This keeps slow first touches softer, still permits a firmer cushioning impulse on fast passes, and preserves finite-data and player-contact guards. The change is shared by manual reception, human dribble and AI controlled touches; source regression added. The gameplay balance and frame-to-frame feel must be tested in Unreal/Chaos before accepting as finished.

Ground-foot receiving now also rejects balls with over 420 cm/s vertical speed. Volleys, header/chest/thigh trapping and grounded-vs-airborne control selection require dedicated animation and future native UE simulation. A source regression covers this deliberate boundary.

### October 21 — contested receiving and touch ownership

The ground-foot receiving path now introduces a 20% touch-speed penalty when a nearby defender is within 250 cm at viable contact height, modelling a rushed touch without silently transferring possession. The physics component now refuses to update last touch for impulses below 1 cm/s horizontal correction. Regression checks were added. This remains a gameplay foundation and requires native UE collision, animation and match testing.

Midfield support runners now move their passing outlet laterally away from an opponent occupying the target within 190 cm, with clearance bounded by the authored midfield corridor. This is a positional heuristic, not full pathfinding or passing-lane raycast analysis. Source regression added.

### October 21 — exhausted first touch and blocked passing outlets

Receiving under pressure now scales imperfect cushioning by current endurance (1.12–1.35 multiplier rather than a fixed rushed-touch factor). Midfield support only accepts an opponent-avoidance relocation if the final clamped target provides at least 175 cm clearance; a corridor boundary can otherwise negate the suggested sidestep. Regression tests updated.

### October 21 — close-control shielding and sprint release

The manual controlled-touch direction now moves slightly away from the nearest opposing challenger within 260 cm during effective shielding (75% original intended direction plus 25% away from threat). Standard dribbling remains unchanged. On sprint release, depleted stamina now correctly prevents the exhausted shielding pace benefit. Source regression tests have been updated.

### October 21 — adaptable shielding and passing lane support

Shielded close dribbling now steers away from a defender with strength proportional to their proximity instead of a fixed 25 percent blend. Midfielders also disregard opponents clearly sprinting away from the selected supporting outlet, reducing unnecessary channel changes. Regression source contracts added.

Hard-cut acceleration now receives a bounded additional endurance penalty (up to 12% at low stamina); straight-line acceleration retains its existing fatigue calculation. This provides a separate distinction between sprint endurance and sharp-cut agility. Source regression added.

### October 21 — shielding skill and support pitch limits

Protective dribbling touches now incorporate the player's BallControl attribute in the bounded opponent-avoidance blend (4–36%). Midfield outlet sidesteps clamp to within 180 cm of the sidelines after corridor selection, keeping the support target on the playing surface. Source regression tests updated.

### October 21 — supporting passing-lane obstruction

Supporting central midfield targets now check opposing footballers along the straight ball-to-target pass segment (145 cm screening radius and 135 cm vertical tolerance) and may offset their target laterally by 240 cm while respecting both the formation corridor and touchline margins. This complements existing target-point clearance. Shielding direction also scales with BallControl rating. Static source regressions have been extended; gameplay integration testing is a separate later phase.


## October 21 — coordinated defensive marking ownership

The existing nearest-threat marking now arbitrates each potential opposing runner against the defending roster. An eligible autonomous outfielder yields that particular mark when a valid same-world AI teammate is closer to the runner; equal-distance ties use the authored lineup slot. The keeper and human-controlled pawns are not assigned automatic outfield marking ownership. If no runner is assigned, the defender stays on the existing formation / defensive-line target rather than blindly converging on another defender's assignment. The actual goal-side position, infield protection, finite-target checks, pressing and second-press behaviors are unchanged.

This is a lightweight deterministic local decision, **not** a full centralized tactical marking coordinator. It does not guarantee global one-to-one matching, account for every direct pressing assignment, or make offside/line tracking authoritative. Python source-contract checks in `unreal/tests/test_team_ai_mark_assignment.py` protect the guards; only native UE gameplay tests can validate actual defensive spacing.

### Marking allocation refinement

A defender no longer automatically yields a runner to a closer teammate if that teammate has an alternative opponent more than 75 cm closer. This is intended to reduce uncovered secondary runs in two-attacker situations while preserving distance-based default ownership and fallback formation coverage. This local heuristic is not globally optimal assignment and can still produce clashes; coordinated tactics and UE playtests are outstanding.

### Deterministic pressers

Primary and secondary pressing selection now uses authored lineup slots to break squared-distance ties within 1 cm², rather than actor-array traversal order. This stabilizes which autonomous player closes down the ball in nearly equidistant situations. The change leaves the human-control exclusion, keeper exclusion, pressing cooldown and formation rules intact. It does not yet implement pressing triggers or a fully tactical pressing coordinator.

### Out-of-pitch opponent marker safety

Defensive marking ignores opposing actor coordinates more than 150 cm beyond the configured pitch X/Y boundaries, preventing stray actors or provisional out-of-bounds sequences from drawing an automated marker away from the active defensive shape. Normal marking, pressing, and referee stoppage precedence remain unchanged. Regression: `tests/test_team_ai_mark_assignment.py`; UE gameplay verification remains pending.

### Custom pitch navigation bounds

Final outfield AI navigation targets now derive nonnegative playable half-extents from the configured match pitch. This avoids reversed clamp bounds when a compact/custom pitch is smaller than the 150 cm safety inset. Full-size regulation fields retain their existing inset. Source contract checks were expanded; native navigation behavior still requires Unreal Editor validation.

### Bounded off-ball navigation horizon

The AI controller now caps the distance of each newly requested outfield navigation target to 1800 cm from the player's current horizontal position, before applying final pitch constraints again. This keeps long formation recovery and attacking support paths subject to periodic tactical reassessment, rather than committing the controller to a distant one-step target. It does not change player teleporting, velocity, animation or pathfinding implementation. Source regression protects the decision target; genuine UE navigation results remain untested.

### Grounded team navigation target

Final AI movement requests now use the current footballer's vertical coordinate after computing formation, cover and bounded planar targets. This prevents a stale or elevated formation Z value from becoming a raw navigation request. Unreal navigation projection, slopes and physical step handling remain engine responsibilities and need native testing. Regression: `tests/test_team_ai_mark_assignment.py`.

### Defensive-half planning interval

The bounded local AI navigation target now uses a shorter 1350 cm horizon when the ball is in the team's defensive half, and the previous 1800 cm horizon while attacking. This increases the frequency of tactical reconsideration during defensive recovery without changing the existing decision timer or pawn movement speed. The feature is source-verified only; UE5.5 navigation still needs gameplay validation.

### Boundary-aware transition horizons

The shorter defensive-half AI planning horizon now applies only when the ball has finite coordinates within the configured pitch dimensions. Stray, out-of-bounds ball samples do not trigger the defensive transition selector. This remains a source-verified tactical heuristic, not a compiled Unreal match simulation.

### Rapid counterattack recovery

When the ball is valid, inside a team's defensive half, and moving toward its goal faster than 650 cm/s along the pitch axis, the AI local movement target horizon is further reduced from 1350 to 1050 cm. This is a conservative tactical re-planning heuristic; established attack horizons and goalkeeper movement remain unchanged. It is not yet a coordinated counter-press or a UE gameplay-validated behavior.

The rapid counterattack trigger also excludes a nearby same-team last-touch carrier, avoiding emergency recovery decisions during an attributed teammate's intentional backward carry or pass. A loose ball may still activate the goalward-motion trigger because touch history alone cannot determine current possession.

### Grounded possession evidence for emergency recovery

Rapid counterattack recovery is suppressed for attributed friendly control only when the last-touch player is on the same team and world, within 220 cm horizontally and 135 cm vertically of the ball. An old aerial touch no longer counts as control merely because of horizontal proximity. This is a conservative source heuristic, not an authoritative possession state; native playtests remain deferred.

### Touch ownership in rapid defensive transitions

The accelerated defensive navigation horizon now requires either an opponent-attributed last touch, an absent last-touch player, or invalid last-touch ownership. A former friendly last touch with a separated ball no longer alone counts as an opponent-led counterattack. This is deliberately conservative and will need true possession-state tracking and Unreal gameplay validation.

### Rising aerial ball transition guard

A ball rising faster than 250 cm/s vertically no longer triggers the shortest emergency ground-counter positioning horizon, even if its horizontal trajectory points toward goal. Standard defensive shape remains available while the aerial sequence develops. The threshold is provisional pending native Unreal gameplay testing.

### Descending airborne counterattack guard

Emergency ground-counter navigation also waits when the ball is more than 135 cm above or below the current defender's contact plane. This complements the existing rising-velocity exclusion and reduces classification of long descending clearances as controlled ground breaks. Gameplay tuning and animation-aware ball height remain for native UE validation.

### Pressure-sensitive AI dribbling lane

An AI carrier with a detected frontal opponent now expands its lateral escape target from 180 cm up to 320 cm as that threat closes from 500 cm toward contact. This influences movement navigation rather than adding an instantaneous lateral ball impulse. Existing sideline avoidance and challenged-carrier pass outlet logic remain. Source-regression coverage lives in `tests/test_team_ai_ball_carrier_dribble.py`; tuning and path feasibility require future native Unreal gameplay tests.

### Ground-height awareness in dribble escape

The AI ball carrier excludes opposing actors more than 150 cm vertically from the ball when choosing the nearest frontal defender for its ground dribble escape lane. Existing opponent distance scaling and sideline steering stay in place. The threshold is a source-level heuristic pending Unreal-native movement and ball contact tests.

### Final touchline check for adaptive dribbling

After scaling the AI carrier's lateral escape offset by opposing-player proximity, the controller now checks the actual proposed lane against the configured pitch's touchline clearance. If the proposed lane would exceed that margin, the carrier shifts toward the pitch interior. Small/custom pitches use a nonnegative clearance bound. Source-level regression only; Unreal pathing and dribble quality remain subject to native validation.

### Clamped dribble sidestep

The pressure-adaptive AI carrier escape now clamps its actual lateral displacement to the available interior touchline margin, rather than only reversing direction before applying the full step. This prevents a large sidestep on narrow or edge-adjacent fields. Existing final pitch clamping remains a fallback; source regression updated to reflect the new displacement calculation.
