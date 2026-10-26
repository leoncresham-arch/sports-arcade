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
