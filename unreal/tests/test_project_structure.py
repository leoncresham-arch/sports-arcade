#!/usr/bin/env python3
"""Fast, zero-dependency checks for the Unreal source-only project.

Not an Unreal compiler, editor test, rendering test or gameplay test.
Run from repo root: python3 -m unittest discover -s unreal/tests -v
"""
import json
import pathlib
import re
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]
SOURCE = ROOT / "Source" / "SportsArcade"


def read(relative):
    return (ROOT / relative).read_text(encoding="utf-8")


class UnrealProjectStructureTests(unittest.TestCase):
    def test_project_descriptor_is_valid_and_declares_module(self):
        descriptor = json.loads(read("SportsArcade.uproject"))
        self.assertEqual(descriptor["FileVersion"], 3)
        self.assertTrue(any(m["Name"] == "SportsArcade" and m["Type"] == "Runtime"
                            for m in descriptor["Modules"]))
        self.assertTrue((ROOT / "Source" / "SportsArcade" / "SportsArcade.Build.cs").is_file())

    def test_target_files_and_build_module_are_present(self):
        self.assertTrue((ROOT / "Source" / "SportsArcade.Target.cs").is_file())
        self.assertTrue((ROOT / "Source" / "SportsArcadeEditor.Target.cs").is_file())
        module = read("Source/SportsArcade/SportsArcade.Build.cs")
        self.assertIn('"Engine"', module)
        self.assertIn('"InputCore"', module)

    def test_gameplay_headers_have_generated_include_and_implementation(self):
        for header in sorted((SOURCE / "Public").glob("*.h")):
            source = (SOURCE / "Private" / (header.stem + ".cpp"))
            self.assertTrue(source.exists(), str(source))
            text = header.read_text(encoding="utf-8")
            generated = f'{header.stem}.generated.h'
            self.assertIn(f'#include "{generated}"', text)
            includes = re.findall(r'^#include\s+["<]([^">]+)[">]', text, re.M)
            self.assertEqual(includes[-1], generated,
                             f"{header.name}: generated header must be the last include")

    def test_game_mode_uses_correct_character(self):
        mode = read("Source/SportsArcade/Private/SportsArcadeGameMode.cpp")
        self.assertIn("ASportsArcadeCharacter::StaticClass()", mode)

    def test_animation_bridge_and_input_agree(self):
        header = read("Source/SportsArcade/Public/SportsArcadeAnimInstance.h")
        cpp = read("Source/SportsArcade/Private/SportsArcadeAnimInstance.cpp")
        character = read("Source/SportsArcade/Private/SportsArcadeCharacter.cpp")
        mapping = read("Config/DefaultInput.ini")
        for field in ["GroundSpeed", "MovementDirection", "TurnRateDegrees",
                      "NormalizedAcceleration", "bIsMoving", "bIsFalling", "bIsSprinting"]:
            self.assertIn(field, header)
            self.assertIn(field, cpp)
        self.assertIn('BindAction(TEXT("Sprint")', character)
        self.assertIn('ActionName="Sprint"', mapping)
        self.assertIn('Key=LeftShift', mapping)

    def test_camera_can_track_and_clear_focus(self):
        header = read("Source/SportsArcade/Public/SportsArcadeBroadcastCamera.h")
        cpp = read("Source/SportsArcade/Private/SportsArcadeBroadcastCamera.cpp")
        for name in ("SetFocusActor", "ClearFocusActor"):
            self.assertIn(name, header)
            self.assertIn(name, cpp)
        self.assertIn("MaximumLateralTravel", cpp)
        self.assertIn("VInterpTo", cpp)

    def test_visual_asset_catalog_has_required_asset_slots(self):
        header = read("Source/SportsArcade/Public/SportsArcadeVisualAssetSet.h")
        source = read("Source/SportsArcade/Private/SportsArcadeVisualAssetSet.cpp")
        for name in ("PlayerMesh", "GoalkeeperMesh", "IdleAnimation",
                     "JogAnimation", "SprintAnimation", "TurfMaterial",
                     "GoalFrameMesh", "GoalNetMaterial", "SourceAndLicenseNotes"):
            self.assertIn(name, header)
        self.assertIn("HasCoreVisualReferences", source)
        candidates = read("docs/free-asset-candidates.md")
        self.assertIn("Game Animation Sample", candidates)
        self.assertIn("Digital Human", candidates)

    def test_project_does_not_claim_compiled_or_rendered_artwork(self):
        readme = read("README.md").lower()
        self.assertRegex(readme, r"not been (?:opened, )?compiled|not been opened, compiled")
        self.assertIn("no stadium", readme)


if __name__ == "__main__":
    unittest.main()
