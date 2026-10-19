# Sports Arcade — Native PC Football Capability Blueprint
Research date: 2026-10-15. Status: DEVELOPMENT REQUIREMENTS, not implemented functionality.

## Design principle
Create an original Unreal Engine 5.5 PC football simulation with comparable breadth of football capabilities to leading modern sports titles. No EA source code, proprietary skill-move implementation, licensing, player likenesses, branding, teams or animations may be copied. Build original mechanics and use individually licensed assets. High-end modern Windows gaming PC is target; preview availability must not block source development.

## External references (mechanics research only)
- Official FC 27 gameplay: https://www.ea.com/games/ea-sports-fc/fc-27/features
- Official FC 27 PC feature research: https://www.ea.com/games/ea-sports-fc/fc-27/news/pitch-notes-fc27-pc-deep-dive
- Official FC 27 player ability description: https://www.ea.com/tr/games/ea-sports-fc/fc-27/news/what-are-playstyles
- Official FC 26 gameplay fundamentals deep dive: https://www.ea.com/es-mx/games/ea-sports-fc/fc-26/news/pitch-notes-fc26-gameplay-deep-dive
- EA official 2026 release: https://news.ea.com/press-releases/press-releases-details/2026/EA-SPORTS-FC-27-Is-Available-Now-Bringing-New-Ways-to-Play-The-Worlds-Game/default.aspx

## Player action catalogue (required capabilities, NOT completed features)
1. Locomotion: walk, jog, sprint, acceleration, braking, planting, turns, strafing, lateral jockey, backpedal, jumping, landing, momentum, injuries and fatigue.
2. First touch and retention: receive ground/air passes, trap with feet/chest/thigh, directional first touch, tight dribble, sprint dribble, shielding, stumble recovery, juggles.
3. Skill moves: step-over, ball roll, body feint, drag back, fake shot, fake pass, roulette, heel flick, stop-and-turn, scoop turn, elastico, reverse elastico, rainbow flick, nutmeg, backheel, aerial flair. Gate by player technique, momentum, context and animation availability.
4. Distribution: short and long ground passes, through balls, lofted through balls, chipped passes, driven passes, crossing and low crossing, outside-foot passes, one-touch passes, headers, keeper throws/punts.
5. Finishing: placement, power, finesse/curl, chips, long-range, half-volleys, volleys, headers, diving headers, bicycle kicks, deflections, first-time finishes, penalties.
6. Defense: jockey, contain, manual shoulder positioning, standing and sliding tackles, interceptions, passing-lane blocks, aerial challenges, defensive headers, pressing, teammate pressure, marking, recovery.
7. Goalkeeping: positioning, catches, dives, blocks, parries, punches, deflections to safety, sweeper-keeper, 1v1 charging, crosses, distribution, penalty selection.
8. AI: off-ball runs, overlaps/underlaps, support triangles, defensive shape, covering, interceptions, spatial planning, set piece routines, keeper decisions, tactics and difficulty.
9. Match control: controlled player switching, assisted/manual options, set-piece aim/power/spin, substitutions, tactical quick commands, multiple camera modes.

## Player data model
Player archetype, position(s), preferred foot, height, mass, weak-foot ability, skill-move tier, injury status, fatigue, morale and individual traits. Core attributes:
- Mobility: acceleration, sprint speed, agility, balance, reaction, turning.
- Technique: first touch, ball control, dribbling, skill execution, composure.
- Attack: finishing, shot power, curve, long shots, volleys, heading, offensive positioning.
- Distribution: short pass, long pass, vision, crossing, set-piece delivery.
- Defense: interception, defensive positioning, marking, standing/sliding tackle, aerial awareness.
- Physical: stamina, strength, jumping, aggression, durability.
- Goalkeeping: reaction, handling, diving, positioning, sweeping, footwork, distribution.
All attributes require documented ranges and explicit evaluation formulae; specialize ability through original 'talents' rather than copying other games' names or designs.

## Simulation and animation architecture
- Server-authoritative-ready ball and match state even during offline development.
- Character actor + attribute component + endurance component + action/ability state machine + animation interface.
- Ball free-flight physics distinct from foot-control contacts; deliberate impulses and possession windows, no unrealistic permanent attachment to feet.
- Attack/defend animations state-driven with montage / motion warping and contact events, IK foot plants, realistic acceleration and collision.
- AI tactical director defines formation, duty, target space and pass options, individual agents decide movement.
- Match referee/rules state machine: throw-ins, corners, goal kicks, kickoffs, offside, fouls, advantages, free kicks, cards, penalties, added time, substitutions, extra time and shootouts.
- Replay and deterministic/event-recording foundations; record input, possession/touch, goals, fouls and restart decisions for debugging.
- PC-specific: remappable keyboard/controller controls, graphics/scalability, camera settings, accessibility, save/load, local single-player first.

## Original gameplay modes (long-term)
Exhibition match; local friendlies; tournament/cup; league season; player career; manager career; training arena; skill challenges; local co-op; online exhibition; club/team competitive modes. Do not begin with monetization/card loot mechanics.

## Milestones / priority (build in substantial batches)
P0 — Engineering stability: correct existing inputs and native C++ structural issues; clear action/ball APIs, testable attribute and rules logic; CI checks.
P1 — Real football fundamentals: grounded locomotion and stamina, possession-aware first touch, passing/shooting selection, simple defensive actions, match clock and score.
P2 — 11v11 offline: team and player selection, formations, goalkeeper, AI support/defense, match rules/refereeing and switching.
P3 — Animation and art: licensed footballer rig and kits, physical foot-ball contact, advanced animation blends and retargeting, professional stadium/turf/lighting materials, original skill-move animation library.
P4 — Depth and presentation: player attributes, contextual advanced skills, fatigue/injuries, tactical AI, set pieces, crowd, commentary if rights available, broadcasts, replay, menus, accessibility.
P5 — Modes and multiplayer: seasons/career, online synchronization, anti-cheat, persistence and performance profiling.

## Current verified state
Existing UE5.5 C++ sources have basic walk/jog/sprint, early stamina tick, J pass and K shoot, basic ball impulses, starter pitch/goals/lighting and broadcast camera. No native Unreal compilation or editor playtest; no licensed photoreal player assets integrated. GitHub CI checks Python/source contracts, not running UE. A feature listed above is NOT implemented merely because this blueprint contains it.

## Testing standards
Per-feature Python/domain tests where logic is extractable, source contract tests as intermediate guardrails, eventual UE5 Automation Tests and native C++ build. Record implementation status with evidence. Do not wait for visual screenshots to continue game development.

## Approved expanded full-game scope — worldwide football and modes (October 2026)

Sports Arcade targets worldwide club and national-team coverage across football-playing countries, men's and women's football where feasible. Coverage must be data-driven (country / federation / league / season / club / squad / player / competition / fixtures / standings / transfers / kits). Actual clubs, crests, players, likenesses and competition marks must not be copied without appropriate rights; use original fictional defaults and only integrate licensed data when available. Avoid paid licensing commitments without user approval.

Required modes beyond the initial offline match foundation:
1. Kick-off / exhibition: club or national teams, difficulty, stadium and rules.
2. League season: tables, fixtures, promotion/relegation and save progression.
3. Domestic cup and continental tournament: group and knockout formats.
4. National international cup / qualifiers and custom international tournaments.
5. Manager career: squad selection, transfers, contracts, scouting, budgets and multi-season progression.
6. Player career: create player, progression, transfers, selection and milestones.
7. Custom clubs/teams and player editor, kits, squad management and tournaments.
8. Practice arena, training exercises and skill challenges.
9. Local multiplayer and controller support.
10. Online friendlies, seasons and cooperative clubs (later release; server architecture and anti-cheat required).
11. Original squad-building mode as a later expansion, without assuming paid packs, gambling mechanisms or licensed cards.

Make each country's league depth configurable; don't promise every real-world club in the first release. Keep the initial milestone as authentic Unreal native compile and playable exhibition, with shared data contracts that support expanded modes without rewriting core gameplay.
