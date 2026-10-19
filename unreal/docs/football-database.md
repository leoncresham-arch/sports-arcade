# Native football database foundation

October 19, 2026 — Unreal Engine 5.5 C++ source addition.

`USportsArcadeFootballDatabase` is an authored `UDataAsset` with stable `FName` identifiers for nations, clubs, footballers and competitions. It supports separate men's and women's competitions and league/knockout/group-plus-knockout classifications. `ValidateDatabase` returns diagnostic errors for duplicate/empty identifiers, missing countries or clubs, invalid shirt numbers, repeated competition entries and men's/women's category mismatches.

This is a schema foundation, **not** a populated licensed worldwide database or a functioning career/league mode. Initially author original test data in the Unreal Editor. Player portraits, likenesses, club crests, real-world curated rosters and third-party datasets do not belong in this public repo without appropriate rights.

Next integrations: authored DataAsset example; map database club entries into validated home/away lineups; season/fixture generation using stable ids; save-game serialization and migration. Future national teams and federations need their own membership records rather than treating clubs as national teams.

## Match flow added in the same batch
- Goals now stage the opponent's kickoff through existing B restart action rather than returning to free play instantly.
- Resuming halftime stages an away-team kickoff.
- Kickoff staging restores footballers to authored kickoff transforms and prefers slot 9 from the designated team.
- Ball-out and kickoff controls are still **provisional**, not legal FIFA/IFAB restart enforcement or native tested match flow.
