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
