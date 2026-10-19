# Native Unreal Engine 5.5 automation checks

The source includes five Unreal Editor automation tests in
`Source/SportsArcade/Private/SportsArcadeAutomationTests.cpp`:

- `SportsArcade.Seasons.LeagueSchedule` (double-round scheduling and ordering)
- `SportsArcade.Seasons.CupBracket` (knockout winner advancement)
- `SportsArcade.Seasons.LeagueTable` (points and ranking)
- `SportsArcade.Rules.PenaltyShootout` (alternation and early resolution)
- `SportsArcade.Seasons.CupSaveIntegrity` (bracket consistency, corruption rejection, and tournament completion)

These are real `IMPLEMENT_SIMPLE_AUTOMATION_TEST` C++ tests, intended for Unreal Engine 5.5 Editor. They have **not yet been compiled or executed in Unreal**. The GitHub Actions source-check workflow checks their presence, not their runtime success.

On a Windows PC with Unreal Engine 5.5, first build the existing project using `Tools/Windows/Build-SportsArcade.ps1`. Then use:

```powershell
.\Tools\Windows\Run-SportsArcadeTests.ps1 -EngineRoot "C:\Program Files\Epic Games\UE_5.5" -LogPath "C:\Temp\sports-arcade-native-tests.log"
```

Inspect the output log for each test's pass/fail status, even if Unreal exits with code zero. Do not mark any UE test validated solely from a successful process exit. If Unreal's automation CLI behavior differs for the installed build, adapt the test launcher based on the actual log instead of suppressing failures.

Further work: add in-engine match actor tests, startup/kickoff coverage, SaveGame round-trip tests and UI interaction tests. Never treat the Python source contracts as a replacement for native compilation and runtime verification.


## October 19 cup save integrity extension
- Native CupSaveIntegrity automation checks a four-team cup, rejects incorrect completed-tie counts and round indices, then completes a valid final and checks champion identity.
- Cup save/load, winner mutations, next-fixture UI and progress now use ValidateProgress to avoid accepting or displaying corrupt brackets. The 2–64-team bracket invariant determines expected completed ties and current round.
- Python CI tests source wiring; running the native automation still requires a local Unreal Engine 5.5 Editor build.
