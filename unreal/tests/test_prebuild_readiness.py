"""Prebuild checks for tasks 1-6. Static evidence, not a native UE build."""
import json
import pathlib
import re
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]
SOURCE = ROOT / "Source"
MODULE = SOURCE / "SportsArcade"

class PrebuildReadiness(unittest.TestCase):
    def test_01_unreal_descriptor(self):
        data = json.loads((ROOT / "SportsArcade.uproject").read_text())
        self.assertEqual(data["EngineAssociation"], "5.5")
        self.assertEqual(data["FileVersion"], 3)
        self.assertEqual([(x["Name"], x["Type"]) for x in data["Modules"]], [("SportsArcade", "Runtime")])

    def test_02_plugins_match_required_modules(self):
        data = json.loads((ROOT / "SportsArcade.uproject").read_text())
        enabled = {x["Name"] for x in data.get("Plugins", []) if x.get("Enabled")}
        self.assertIn("EnhancedInput", enabled)
        build = (MODULE / "SportsArcade.Build.cs").read_text()
        for dependency in ("EnhancedInput", "AIModule", "NavigationSystem", "InputCore", "Engine"):
            self.assertIn('"' + dependency + '"', build)

    def test_03_build_targets(self):
        for name, kind in (("SportsArcade.Target.cs", "Game"), ("SportsArcadeEditor.Target.cs", "Editor")):
            contents = (SOURCE / name).read_text()
            self.assertIn("TargetType." + kind, contents)
            self.assertIn('ExtraModuleNames.Add("SportsArcade")', contents)
            self.assertIn("EngineIncludeOrderVersion.Unreal5_5", contents)

    def test_04_header_source_pairs_and_case(self):
        headers = sorted((MODULE / "Public").glob("*.h"))
        self.assertGreaterEqual(len(headers), 10)
        for header in headers:
            with self.subTest(header=header.name):
                self.assertTrue((MODULE / "Private" / (header.stem + ".cpp")).is_file())

    def test_05_engine_api_review_gates(self):
        ai = (MODULE / "Private" / "SportsArcadeTeamAIController.cpp").read_text()
        mode = (MODULE / "Private" / "SportsArcadeGameMode.cpp").read_text()
        self.assertIn("#include \"EngineUtils.h\"", ai)
        self.assertIn("#include \"GameFramework/CharacterMovementComponent.h\"", mode)
        self.assertIn("MoveToLocation(Target", ai)
        self.assertIn("ETeleportType::TeleportPhysics", mode)

    def test_07_windows_build_script_present(self):
        script = ROOT / "Tools" / "Windows" / "Build-SportsArcade.ps1"
        self.assertTrue(script.is_file())
        source = script.read_text()
        self.assertIn("SportsArcadeEditor", source)
        self.assertIn("Win64", source)
        self.assertIn("Build.bat", source)
        self.assertIn("UnrealVersionSelector", source)

    def test_06_generated_headers_last(self):
        for header in sorted((MODULE / "Public").glob("*.h")):
            content = header.read_text()
            matches = re.findall(r'^\s*#include\s+["<]([^">]+)[">]', content, re.M)
            with self.subTest(header=header.name):
                self.assertTrue(matches)
                self.assertEqual(matches[-1], header.stem + ".generated.h")
                self.assertEqual(content.count(header.stem + ".generated.h"), 1)

if __name__ == "__main__":
    unittest.main()
