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
