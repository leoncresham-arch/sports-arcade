# Native Unreal Engine 5.5 automation checks

The source includes three Unreal Editor automation tests in
`Source/SportsArcade/Private/SportsArcadeAutomationTests.cpp`:

- `SportsArcade.Seasons.LeagueSchedule` (double-round scheduling and ordering)
- `SportsArcade.Seasons.CupBracket` (knockout winner advancement)
- `SportsArcade.Seasons.LeagueTable` (points and ranking)

These are real `IMPLEMENT_SIMPLE_AUTOMATION_TEST` C++ tests, intended for Unreal Engine 5.5 Editor. They have **not yet been compiled or executed in Unreal**. The GitHub Actions source-check workflow checks their presence, not their runtime success.

On a Windows PC with Unreal Engine 5.5, first build the existing project using `Tools/Windows/Build-SportsArcade.ps1`. Then use:

```powershell
.\Tools\Windows\Run-SportsArcadeTests.ps1 -EngineRoot "C:\Program Files\Epic Games\UE_5.5" -LogPath "C:\Temp\sports-arcade-native-tests.log"
```

Inspect the output log for each test's pass/fail status, even if Unreal exits with code zero. Do not mark any UE test validated solely from a successful process exit. If Unreal's automation CLI behavior differs for the installed build, adapt the test launcher based on the actual log instead of suppressing failures.

Further work: add in-engine match actor tests, startup/kickoff coverage, SaveGame round-trip tests and UI interaction tests. Never treat the Python source contracts as a replacement for native compilation and runtime verification.
