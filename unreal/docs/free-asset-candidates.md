# Candidate free assets — verify before integrating

This list is **research**, not downloaded or imported Unreal content. Every actual asset must be inspected for license, UE compatibility, skeletal rig, size and game-use rights before it enters the project.

## High-priority candidates

### Epic Games Game Animation Sample
- Information: https://dev.epicgames.com/documentation/en-us/unreal-engine/game-animation-sample-project-in-unreal-engine
- Fab free Unreal category: https://www.fab.com/channels/unreal-engine?is_free=1
- Why: high-quality motion-captured locomotion and Motion Matching, plus retargeting techniques. Useful for physically believable starts, stops, and turning.
- Gap: general-purpose movement, not dribbling, passing, shooting, goalkeeping, or football-specific foot plants.
- Action: verify that the chosen package supports our pinned UE version or plan a deliberate migration. Confirm its license and content redistribution restrictions.

### Epic Games Digital Human
- Listing: https://www.fab.com/listings/0e9c4282-b266-4778-bab0-f60ac923bb9a
- Why: free Unreal-focused example of high-fidelity skin, eyes and hair rendering.
- Gap: a shading reference and bust-focused demonstration; NOT a complete ready-to-play footballer.
- Listing says content is licensed only for Unreal Engine-based products; recheck the full license before use.

### MetaHuman
- Official: https://www.metahuman.com/
- Why: premium-looking digital human creation and rigging.
- Gap: authoring requires a suitable Unreal environment. Needs licensed/original kits and sport-specific animations. Performance of full 11v11 crowds must be profiled.

## Stadiums
Search results for specific realistic stadium kits on Fab include many **paid** products or products without a verifiable free listing. Do not claim these are free. Prefer assembling a unique stadium from legitimate free building assets and authored pitch geometry until a free complete stadium is verified.

## Integration rules
- No asset downloads automatically performed; the current GitHub connection does not install Fab projects or run Unreal.
- Record provider, item URL, author, license and acquisition date inside the asset's metadata, and keep required notices in the game.
- Never commit marketplace assets publicly without checking the precise redistribution terms; a game license often does **not** permit distributing its editable source models publicly.
- An asset's product image is not an in-game Sports Arcade screenshot.
- First finished visual target: one footballer, original kit, properly lit pitch, and a genuine Unreal render.
