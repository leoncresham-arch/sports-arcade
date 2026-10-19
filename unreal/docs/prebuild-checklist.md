# Windows Unreal Engine 5.5 prebuild readiness — tasks 1–6

Scope: source-level checks only. Passing Python/GitHub CI is **not** an Unreal Header Tool run, a C++ compile, a Windows link or an Editor launch.

1. Project configuration — checked: .uproject FileVersion 3, EngineAssociation 5.5, runtime module SportsArcade; both Game and Editor target files reference Unreal 5.5 include order. **Static verified**.
2. Plugin configuration — checked: EnhancedInput enabled in project; EnhancedInput, AIModule, NavigationSystem, Engine and InputCore declared as module dependencies. Plugins bundled with the target engine installation need native validation. **Static verified**.
3. Module dependencies — checked Build.cs and source target contracts by CI tests; module engine API availability and transitive linkage remain unverified. **Static verified**.
4. Headers and sources — existing project structural suite plus prebuild tests scan Public header to Private implementation pairs. This does not prove every class declaration or symbol resolves at compile time. **Static verified**.
5. Unreal API compatibility — reviewed critical use of AI navigation, actor iterators, movement component and physics teleport includes. Version-specific signatures, Python Editor APIs and reflection require UE 5.5 compilation/editor execution. **Partial; cannot sign off native compatibility**.
6. Unreal Header Tool conventions — verify each public gameplay header includes its matching generated header last and exactly once; UHT parsing and generated file output are unverified until Windows build. **Static verified**.

## Evidence and execution
- `unreal/tests/test_prebuild_readiness.py` runs six distinct domain checks with `python3 -m unittest discover -s unreal/tests -v`.
- GitHub Actions `unreal-source-check.yml` executes the source suite and Python Editor script syntax checks on Ubuntu.
- No Unreal binary or Windows compiler is connected to this GitHub-only environment. Tasks 7–12 (Windows toolchain/project generation/compile/errors/rebuild/startup) cannot be claimed completed.

## Native Windows execution sequence (when hardware is available)
1. Install matching UE 5.5 C++ capable Engine and Visual Studio Windows C++ workload.
2. Right-click `SportsArcade.uproject`, generate IDE project files.
3. Build the SportsArcadeEditor Win64 Development target; capture the first UHT/compiler error and resolve all failures.
4. Launch the editor, run the 11v11 map bootstrap in a test project, validate level, controllers, physics and camera, then package and run a local Windows development build.
5. Commit corrections alongside proof: engine version, target, compiler exit status and tested map, without falsely labeling source-only CI a native build.
