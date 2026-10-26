# Native PC production milestones and efficiency policy — 2026-10-26

## Why the workflow changes
Recent work has overfocused on one-line tactical thresholds, with a source/test/docs commit per minor tweak. Source-contract tests prove that strings exist, not that the Unreal C++ compiles or that football behavior is correct. Stop counting small commits as major feature progress. Retain all existing architecture and improvements.

## Delivery definition
A milestone is done only when its behavior and interfaces are integrated, regression-tested to the extent possible, documented, and source CI passes. Mark native-engine build and playtest verification separately as **pending** until actually run. Do not use source CI as a substitute for runtime verification.

## Next cohesive work packages (in priority order)
1. **11v11 football tactics integration:** one coherent pass across possession recognition, pressing allocation, defensive shape, support triangles, interceptions, pass selection and transition state. Prefer shared methods/data instead of accumulating more disconnected conditions in the AI controller. Add actual algorithmic tests (not just literal-string assertions) wherever logic can be extracted.
2. **Movement/action orchestration:** connect agility, acceleration, sprint, jockey, shielding, fatigue, tackle recovery and locomotion intent behind consistent footballer action-state contracts. Ensure control changes and interruption semantics are documented.
3. **Ball/player interactions:** close the loop among physics first touch, interception, trapping, ball shielding, tackles and ball-out logic; verify ownership and priority with deterministic edge cases and native test scenarios.
4. **Match completeness:** substitutions, tactical controls, set-piece flows, goalkeeping, referee rules, recovery/save progression and scoreboard state validated as a connected match lifecycle.
5. **High-fidelity presentation foundation:** an original art/animation/asset specification, skeletal rigs, animation graph and motion state interfaces, camera/stadium lighting quality gates, performance budgets and licensing inventory. Do not claim FIFA-level graphics without actual integrated assets and visual review.
6. **Windows UE5.5 integration gate:** Unreal build, in-engine automation, scripted gameplay scenarios, visual capture comparison, performance and bug fixing. Source-only development may continue while native execution is unavailable.

## Operating rules
- Work **feature batches**, not arbitrary one-variable tweaks; aim for a clear behavior milestone with several relevant scope items per session.
- Inspect HEAD, existing requirements, touched call sites, and CI *before* changes; avoid reimplementing existing systems.
- Prefer a coherent C++ change with accompanying tests and documentation together. When the connector requires per-file commits, chain them without a separate user-facing completion report per commit.
- Repair failed CI immediately. Distinguish unit/static assertions from actual executable or Unreal tests in all reporting.
- Put durable scope and verification checkpoints in GitHub; avoid repetitive handoff files unless context really requires one.
- Estimate complete-game percentage only from milestone acceptance, not commit counts. The prior ~20% completed is a tentative planning estimate, not an audited measurement.
- Preserve UE5.5, Windows PC, existing repository and architecture, and use no paid services without approval.

## Current verification state (2026-10-26)
Source-only GitHub Actions 'Verify Unreal source scaffold' passed on `2adca569` prior to this policy. Native Windows UE compilation, in-engine gameplay, high-fidelity graphics and performance remain unverified.

## 2026-10-26 — 11v11 AI performance milestone (source implemented)

Added deterministic initial decision phase offsets per valid lineup slot/team (22 unique phases across each 0.3 s decision window) so all controllers do not conduct costly team/opponent scans on their first tick. The phase initializes when a valid lineup pawn exists; regular AI decisions and cooldown timing otherwise remain unchanged. Python property-style scheduling tests validate uniqueness and bounds, and source checks verify the C++ wiring. Native profiling remains pending; this is a performance-preparation milestone, not measured frame-time improvement. Next prioritize shared tactical decision calculation and reduction of repeated squad-wide searches.

## 2026-10-26 — Shared tactical opponent snapshot

Within each valid off-field decision, the controller now enumerates the opposition once into a transient list rather than separately enumerating world actors for midfield carrier-pressure detection and blocked support-outlet checks. Per-use distance, height, velocity and world-validity checks remain intact. This is an initial reduction in redundant searches, not yet a full team-level shared tactical cache; other independent scans remain and Unreal profiling is still needed. Python source contracts were added alongside the C++ implementation.

## 2026-10-26 — Individual real-footballer performance foundation

The existing 1–99 `USportsArcadePlayerAttributes` component now has separately authored vision, long passing, shot power, defensive awareness, composure, strength and weak-foot ability alongside its existing ratings. A bounded contextual factor models skill, stamina and pressure; human-controlled passing and finishing currently incorporate each player's skill and stamina. Pressure inputs are supported by the model but **not yet wired to those actions**. New attributes beyond existing gameplay wiring are schema-only until integrated. All currently default to neutral 65: do **not** represent these as verified real-player ratings. Per-footballer profiles should be drawn from permitted scouting/statistical data and remain independently tunable and attributable, including preferred positions and physical traits. Avoid importing copyrighted datasets or unlicensed likenesses into the public repository. Test source checks and mathematical factor properties; verify actual animation and match tuning later in Unreal.

## 2026-10-26 — Individual player performance profiles

The authored footballer database now stores eight distinct 1–99 gameplay ratings per footballer (acceleration, agility, sprint, stamina, short passing, finishing, ball control and tackling). Existing team roster assignment copies the matched player's values into the live character's `USportsArcadePlayerAttributes` component, which already influences movement and core football actions. Ratings default to neutral 65 and are clamped at binding. This is original, editable football data—not a copy of proprietary FC ratings or a claim of verified current professional-player statistics. Source contracts cover the entire field-to-component mapping. Native Unreal testing and richer player traits/AI differentiation remain follow-on milestones.

## 2026-10-26 — Broader tactical AI performance continuation

Expanded the existing per-decision opposition snapshot to midfield passing-lane obstruction checks, avoiding another full world enumeration in the same tactical decision. Individual footballer attribute integration remains part of the long-term plan but does not supersede team tactics, movement, ball interactions, match completion or graphics priorities. Source regression added; native profiling remains pending.

## 2026-10-26 — Universal baseline and real-world customization

Core design policy: every footballer, team, formation, stadium, match rule, animation state and gameplay system must have sensible generic defaults and work without external real-world data. Optional verified/authored real-world information enriches that baseline rather than becoming a prerequisite. The database now has generic stadium records (30,000-capacity, 105x68 m pitch defaults) and optional club home-stadium and 4-3-3/4-4-2 formation references. Validation rejects duplicate stadiums, missing references, invalid dimensions and unsupported tactical formations. The existing roster now applies an optional authored club formation when binding footballers; otherwise it preserves the team's baseline formation. Stadium records are *data foundations*, not yet spawned 3D stadium meshes or runtime stadium selection. Do not copy proprietary club artwork, stadium assets, player faces or game data without permission. Native UE verification pending.

## 2026-10-26 — Baseline stadium resolver

Added a Blueprint-callable `GetHomeStadiumOrDefault` lookup for club records. A valid authored home stadium takes precedence; an absent, unknown or invalid stadium reference resolves to a generic 30,000-capacity, 105x68 m ground. This is a data resolver and not yet stadium mesh spawning, lighting, crowd presentation or match-game-mode pitch selection. Source regression added; native UE5.5 compilation deferred.

## 2026-10-26 — Stadium data to match pitch integration

`ASportsArcadeGameMode::ConfigurePitchFromHomeStadium()` now explicitly resolves the selected home club's authored stadium or generic baseline and can set game-mode pitch half-length/width in centimetres. It is deliberately **opt-in**, not an automatic level-resize: changing a numeric pitch bound does not resize the level's visual pitch mesh, goal actors, collision volumes, restart markers or stadium models. A level author must align those assets before enabling varying pitch dimensions in a playable level. Source tests verify the resolver connection and units. Native UE5.5 compilation and field-geometry integration are unverified.

## 2026-10-26 — Real-club stadium data completeness

The generic venue remains a resilient fallback for incomplete imports and custom teams, not the intended final presentation for real clubs. Club records may set `bRequireVerifiedHomeStadium` to enforce a nonempty home-stadium reference during catalog validation; referenced IDs must exist in the stadium catalog. This is an explicit authoring constraint, not a claim that existing catalog entries contain researched real-world venues. Real stadium names, geometry, location, club relocations and groundsharing must be verified and maintained with provenance; licensed venue artwork is separate. Native game scene selection still needs integration.

## 2026-10-26 — Ball-control arbitration optimization

The human-controlled dribble path now makes one footballer proximity pass to determine both teammate-priority and opposition-priority ownership instead of separately enumerating all characters twice for each prospective touch. Existing grounded contact-height, shielding margin, same-world and team checks are preserved, with short-circuit only once both claims are found. Source regressions updated to verify the combined logic and prevent accidental automatic steals. GitHub Python source tests passed on commit `4a08e141`. Native UE compilation, functional ball-control behavior and performance impact remain unverified.

## 2026-10-26 — Early manual-touch reachability gate

The manual ball-control path now rejects out-of-range and rearward/nonaligned balls before enumerating footballers for ownership arbitration. This retains the prior eligibility requirements while reducing full-roster work for impossible touch candidates. Regression checks cover gate ordering. Source CI is not a native Unreal behavioral or performance test.

## 2026-10-26 — Loose-ball speed rejection ordering

Moved the existing relative high-speed loose-ball test before expensive teammate/opponent proximity arbitration in player-controlled dribbling. Impossible-to-control balls no longer trigger a squad ownership scan. Preserves the original 1150 cm/s threshold and 0.3 directional test; only the order of checks changes. Added regression check for the early gate. Performance and native behavioral validation remain pending.

## 2026-10-26 — Native executable regression expansion

Added `SportsArcade.Database.HomeStadiumFallback` to the existing Unreal Engine native editor automation suite. Unlike Python source-string contracts, this C++ automation exercises actual stadium lookup and database validation against live `UDataAsset` objects when run in Unreal: absent club uses the 105x68 m baseline; missing required home ground fails validation; an authored valid ground resolves correctly; corrupt pitch dimensions are rejected and fall back safely. Updated both suite-count and integration contracts. **Not yet executed under Unreal Engine 5.5:** GitHub source CI cannot establish native compile or behavioral pass; run `unreal/Tools/Windows/Run-SportsArcadeTests.ps1` when Windows UE is available.

## 2026-10-26 — Completion estimate accountability and integrated gameplay work

The previously communicated 30% completion figure is an **unverified planning estimate**, not a tested milestone result. Later replies incorrectly switched to ~20% without an evidence-based audit. Do not claim either as measured. Retain 30% solely as the user-visible working baseline until a documented feature/acceptance audit establishes an updated figure. Native UE5.5, gameplay, visual quality, and performance must receive substantial weight; source-file or commit counts must not drive completion percentage.

Next development must prioritize cohesive outcomes over per-condition tuning:
- **11v11 integrated gameplay:** possession, pressing, defending, support runs and passing decisions evaluated together with acceptance scenarios.
- **Physical player–ball interactions:** controlled first touch, tackle, interception, shielding and ball-out arbitration validated as connected behavior.
- **Match lifecycle:** formation/club data, kickoffs, set pieces, substitutions, full time and saved progress verified end to end.
- **Presentation integration:** original stadium meshes, real rigged athletes, animation state graph, camera and lighting, with legal asset records.
- **Verification:** expand actual Unreal editor C++ automation, then native Windows builds and game-play testing; source-string tests are insufficient.

Current integration batch: strengthened catalog validation for eight 1–99 footballer attributes, created a native editor automation scenario with valid/invalid profile records, and updated source regression coverage. No native UE execution occurred.

## 2026-10-26 — Integrated 11v11 opponent-observation reuse

The existing AI controller now reuses its per-decision validated opponent snapshot in five additional tactical paths: support-outlet displacement, carrier escape pressure, immediate ball challenge detection, pressured outlet screening and general pass-receiver/interception checks. This removes five redundant full-world footballer traversals from a decision without altering the existing attacking/defensive scoring criteria. Dead-ball suppression and per-use physics/height checks remain active. A dedicated source regression checks snapshot reuse and guard ordering. Runtime movement, interception quality, and performance gains **remain unverified** until Unreal Engine 5.5 compilation and profiling. This is a connected tactical performance step, **not completion of the 11v11 milestone**. Next integrate coherent state classification and acceptance scenarios across possession changes, pressing and recovering formation, without multiplying one-off scalar thresholds.

## 2026-10-26 — Coordinated pressing and cover selection

Primary and secondary AI pressers are now selected together in a single roster pass with a deterministic distance/lineup-slot ordering. The second defender is the next closest eligible AI outfielder after the first; human-controlled players and the goalkeeper are not silently included. The existing restrictions on secondary defensive coverage, pressure radius, lateral stand-off and emergency recovery remain intact. Source-level regression checks protect the one-pass selection and tie-breaking; Unreal editor simulation must still verify pressing swaps, goalkeeper exclusions, controlled-player takeovers and transition timing. This is part of the integrated 11v11 tactics package, not a complete football match milestone.

## 2026-10-26 — Defensive marking conflict integration

Nested alternate-runner coverage checks reuse the existing validated tactical opposition snapshot instead of traversing every world character again for each closer teammate. This preserves nearest-runner priority, competing-assignment checks, and the 75 cm alternative-threat margin while reducing repeated actor enumeration. Added source regression coverage. This is not a new defensive AI mode or proof of CPU improvements: Unreal 5.5 native compile, 11v11 transition playtests, and frame profiling remain pending.

## 2026-10-26 — Live-carrier defensive marking consistency

The marking eligibility gate now uses a nearby, same-world, grounded teammate's actual team attribution, consistent with attacking-support possession classification. Previously a historical last-touch team could suppress defensive marking despite a detached, aerial or contested ball. Autonomous pressers retain priority; the existing contact and distance limits stay in place. Source regression tests have been updated. Full UE5.5 C++ compilation and controlled transition scenarios remain pending; the legacy browser stadium review does not reflect this native gameplay source.

## 2026-10-26 — Extracted tactical selection with native behavioral tests

This session addresses the repeated tiny-patch problem by moving the deterministic primary/cover press ranking into `SportsArcadeTacticalSelection.h/.cpp`, integrating it with the current 11v11 AI controller and adding `SportsArcade.Tactics.PressPair` Unreal Editor automation. The scenario evaluates equal-distance tie-breaking, input order independence, goalkeeper/out-of-range candidate rejection, single-presser fallback and empty-team behavior without needing a level or AI actors. Existing source checks were aligned with the extracted component, and a dedicated C++ implementation was added to conform to the repository's header/source and generated-header conventions. The GitHub source-validation workflow passed on commit `4049c160` but **does not compile or execute** the new C++ automation. The gameplay milestone remains open until the native test is run on Windows UE5.5 and pressing/recovery is observed in 11v11 play. The HTML stadium preview is unrelated to these native improvements and should not be presented as a game-quality sample.

## 2026-10-26 — Integrated pressing roster safety and real native-test scenarios

Follow-on integrated 11v11 work removes the extra roster pass between tactical role selection and actor resolution by constructing an indexed, valid autonomous-outfielder slot map in the controller. Explicit lineup slots and implicit lineup indices share one validated range (1–10); invalid/duplicate slots and human-controlled players are excluded before assignment. The extracted press/cover selector now tolerates repeated slot data and rejects non-finite distances; Unreal C++ automation scenarios exercise deterministic selection, permutation, ties, duplicate entries, non-finite input and empty rosters. Source contracts were updated for the revised production code, and the GitHub Python source workflow has passed at intermediate commits. **Native C++ compilation and Unreal automation execution are outstanding**; these changes have not demonstrated gameplay or frame-time quality. This is a tactical subsystem integration increment, not a full 11v11 acceptance milestone. Next prioritize physical ball reception and gameplay action/movement orchestration along with authentic visual asset integration.

## 2026-10-26 — Shared ball-contact possession authority

The ball actor now exposes `GetControlledPlayer(MaximumPlanarRangeCm, MaximumHeightDifferenceCm)` independently of historical last-touch attribution. It validates the live player, matching world, valid team, finite positions and range, and refuses distant or airborne last-touch ownership. The 11v11 AI now uses this query for attacking support and defensive-marking suppression so they share the same grounded-ball control semantics; loose balls remain contestable. Existing source regressions were updated, with successful source CI at `5ab9952e`. **This is not a finished interception/contact system**; UE5.5 native build, physical ball recovery/keeper tests and playtesting remain unverified. No changes to the legacy browser preview.

## 2026-10-26 — Close-control and AI dribble consistency

The AI ball carrier and teammate press-suppression paths now consume the same `ASportsArcadeBall::GetControlledPlayer(220.f, 115.f)` contract rather than duplicating a historical-last-touch proximity check. That contract validates the current world, last-touch footballer, live team identity, finite positions and grounded contact range. This connects attacking carrier motion and teammate defensive pressing to a single ball-contact authority. The related Python source contracts were updated and source CI passed on `581e9da6`. This is source-level evidence only; Unreal 5.5 C++ compilation, actor/world interaction, contact realism, and PC match behavior remain unverified. No artificial overall percentage change.

## 2026-10-26 — Opponent-counter classification aligned with live ball control

Defensive transition classification now obtains the opponent's current grounded carrier using the ball actor's validated control query. A detached historical last touch or unattributed ball flight no longer automatically qualifies as a driven opposition counter. This preserves ordinary loose-ball recovery while reserving emergency response for actual controlled opposition play. Existing source regression expectations were aligned. Native match testing and Unreal compilation remain separate acceptance stages.

## 2026-10-26 — Ball-control physics precondition hardening

The native ball control authority now fails closed when the collision sphere is absent, not simulating physics, or reports non-finite linear velocity. This prevents tactical AI from trusting contact attribution while the ball is in an invalid physics state. Source-level regression tests cover the shared physics/position validation, the separation from historical last touch and touch-attribution ordering. Unreal native integration and ball reception behavior still require PC testing.

## 2026-10-26 — Kick reach and moving-ball possession classification

The physics-ball kick entry point now checks vertical contact reach in addition to total contact distance before recording a touch; the bound permits reachable elevated goalkeeper contacts while rejecting implausibly high foot kicks. Ball control classification also rejects cases where the ball is moving faster than the nearby last toucher by more than 1500 cm/s in the plane, avoiding false controlled-possession calls as driven passes or ricochets cross the last kicker. The relative velocity is measured against player motion so running carriers are not automatically rejected. Source regression contracts cover vertical reach, relative-speed validation, and touch attribution ordering. Unreal native playtests remain necessary to tune reception thresholds and distinguish volleys/headers with dedicated animation/contact sockets.

## 2026-10-26 — Airborne-contact separation

The ball actor's shared grounded-possession query now rejects vertical ball velocities above 650 cm/s. This prevents fast rising/falling passes and ricochets from being interpreted as settled dribbles simply because the previous kicker remains nearby. The relative horizontal velocity check is retained for moving ball carriers, while historical last-touch tracking is unchanged for match event attribution. Source checks were aligned with the single captured physics velocity and passed at `1a92a131`. This remains part of native gameplay integration rather than a separately completed football mechanic; physical reception and aerial control require Unreal editor testing and animation-specific contact work.
