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

    def test_high_end_pc_rendering_baseline_is_explicit(self):
        from configparser import ConfigParser
        config = ConfigParser(interpolation=None)
        config.optionxform = str
        config.read(ROOT / "Config" / "DefaultEngine.ini", encoding="utf-8")
        renderer = config["/Script/Engine.RendererSettings"]
        self.assertEqual(renderer["r.DefaultFeature.AntiAliasing"], "4")
        self.assertEqual(renderer["r.DynamicGlobalIlluminationMethod"], "1")
        self.assertEqual(renderer["r.ReflectionMethod"], "1")
        self.assertEqual(renderer["r.GenerateMeshDistanceFields"], "True")
        self.assertEqual(renderer["r.Shadow.Virtual.Enable"], "1")
        self.assertNotIn("r.RayTracing", renderer)

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

    def test_pass_and_shoot_input_contract(self):
        mapping = read("Config/DefaultInput.ini")
        self.assertIn('ActionName="Pass",Key=J', mapping)
        self.assertIn('ActionName="Shoot",Key=K', mapping)
        source = read("Source/SportsArcade/Private/SportsArcadeCharacter.cpp")
        self.assertIn("StrikeNearbyBall", source)
        self.assertIn("TActorIterator<ASportsArcadeBall>", source)
        self.assertIn("NearestBall->KickByPlayer(this, Forward, ImpulseStrength, BallControlReachCm)", source)

    def test_native_footballer_ratings(self):
        header = read("Source/SportsArcade/Public/SportsArcadePlayerAttributes.h")
        cpp = read("Source/SportsArcade/Private/SportsArcadeCharacter.cpp")
        self.assertIn("int32 ShortPassing", header)
        self.assertIn("int32 Finishing", header)
        self.assertIn("PlayerAttributes->SprintSpeed", cpp)
        self.assertIn("PlayerAttributes->Stamina", cpp)
        self.assertIn("PlayerAttributes->ShortPassing", cpp)
        self.assertIn("PlayerAttributes->Finishing", cpp)

    def test_formation_ai_is_connected(self):
        header = read("Source/SportsArcade/Public/SportsArcadeTeamAIController.h")
        source = read("Source/SportsArcade/Private/SportsArcadeTeamAIController.cpp")
        footballer = read("Source/SportsArcade/Private/SportsArcadeCharacter.cpp")
        build = read("Source/SportsArcade/SportsArcade.Build.cs")
        self.assertIn("AAIController", header)
        self.assertIn("MoveToLocation(Target", source)
        self.assertIn("GetFormationPosition(Slot)", source)
        self.assertIn("FormationAdvanceFactor", source)
        self.assertIn("Nearest == Footballer", source)
        self.assertIn("PressureRadiusCm", header)
        self.assertIn("GoalkeeperLateralRangeCm", source)
        self.assertIn("GoalkeeperAdvanceCm", source)
        self.assertIn("BallPredictionSeconds", source)
        self.assertIn("ActionCooldownRemaining", source)
        self.assertIn("if (Ball->KickByPlayer(Footballer, Direction", source)
        self.assertIn("if (Ball->KickByPlayer(Footballer, Clearance", source)
        self.assertIn("Ball->KickByPlayer(Footballer, Clearance, 2300.f", source)
        self.assertIn("Ball->GetActorLocation().Z) < 135.f", source)
        self.assertIn("Ball->GetActorLocation().Z) < 175.f", source)
        self.assertIn("MarkingSearchRadiusCm", source)
        self.assertIn("MarkingInfluence", source)
        self.assertIn("bIsPrimaryPresser = true", source)
        self.assertIn("!bIsPrimaryPresser && IsValid(Ball)", source)
        self.assertIn("BestPassScore", source)
        self.assertIn("FVector2D::DotProduct", source)
        self.assertIn("Ball->KickByPlayer(Footballer, Direction, BestPassScore", source)
        self.assertIn("Match->bHalfTime || Match->bMatchFinished", source)
        self.assertIn("AIControllerClass = ASportsArcadeTeamAIController::StaticClass()", footballer)
        self.assertIn("PlacedInWorldOrSpawned", footballer)
        self.assertIn('"AIModule"', build)
        self.assertIn('"NavigationSystem"', build)

    def test_native_dribble_and_tackle_contract(self):
        ball = read("Source/SportsArcade/Private/SportsArcadeBall.cpp")
        footballer = read("Source/SportsArcade/Private/SportsArcadeCharacter.cpp")
        mapping = read("Config/DefaultInput.ini")
        self.assertIn("ApplyControlledTouch", ball)
        self.assertIn("Player->GetActorLocation().Z - GetActorLocation().Z", ball)
        self.assertIn("LastTouchPlayer = Player", ball)
        self.assertIn("TouchCooldownRemaining", footballer)
        self.assertIn("DribbleTouchRangeCm", footballer)
        self.assertIn("TackleBall", footballer)
        self.assertIn("TackleCooldownRemaining", footballer)
        self.assertIn("StrikeCooldownRemaining", footballer)
        self.assertIn("if (NearestBall->KickByPlayer(this", footballer)
        self.assertIn("StrikeCooldownRemaining = FMath::Max(StrikeCooldownSeconds", footballer)
        self.assertIn("LastTouch->TeamIndex == TeamIndex", footballer)
        self.assertIn("if (It->KickByPlayer(this", footballer)
        self.assertIn("FMath::Abs(ToBall.Z) <= 120.f", footballer)
        self.assertIn("KickByPlayer(this", footballer)
        self.assertIn("LastTouchPlayer = Player", ball)
        self.assertIn('ActionName="Tackle",Key=L', mapping)

    def test_team_roster_and_player_switching_contract(self):
        header = read("Source/SportsArcade/Public/SportsArcadeTeamRoster.h")
        source = read("Source/SportsArcade/Private/SportsArcadeTeamRoster.cpp")
        character = read("Source/SportsArcade/Public/SportsArcadeCharacter.h")
        mode = read("Source/SportsArcade/Private/SportsArcadeGameMode.cpp")
        self.assertIn("Players.Num() >= 11", source)
        self.assertIn("DiscoverPlacedPlayers()", source)
        self.assertIn("bDiscoverPlacedPlayers", header)
        self.assertIn("TActorIterator<ASportsArcadeCharacter>", source)
        self.assertIn("Player->TeamIndex != TeamIndex", source)
        self.assertIn("GetClosestPlayer", header)
        self.assertIn("ESportsArcadePosition", character)
        self.assertIn("SwitchToNearestTeammate", mode)
        self.assertIn("Controller->Possess(BestPlayer)", mode)
        self.assertIn("PreviouslyControlled->SpawnDefaultController()", mode)
        self.assertIn("for (ASportsArcadeCharacter* Candidate : It->Players)", mode)
        self.assertIn('ActionName="SwitchPlayer",Key=Tab', read("Config/DefaultInput.ini"))
        self.assertIn("BindAction(TEXT(\"SwitchPlayer\")", read("Source/SportsArcade/Private/SportsArcadeCharacter.cpp"))
        self.assertIn("ESportsArcadeFormation", header)
        self.assertIn("bUseExplicitLineupSlots", header)
        self.assertIn("int32 LineupSlot", character)
        self.assertIn("Footballer->LineupSlot", read("Source/SportsArcade/Private/SportsArcadeTeamAIController.cpp"))
        self.assertIn("GetFormationPosition", source)
        self.assertIn("Slots433[11]", source)
        self.assertIn("Slots442[11]", source)

    def test_native_team_metadata(self):
        header = read("Source/SportsArcade/Public/SportsArcadeCharacter.h")
        self.assertIn("int32 TeamIndex", header)
        self.assertIn("int32 ShirtNumber", header)

    def test_character_stamina_system(self):
        header = read("Source/SportsArcade/Public/SportsArcadeCharacter.h")
        source = read("Source/SportsArcade/Private/SportsArcadeCharacter.cpp")
        for field in ("CurrentStamina", "MaxStamina", "SprintDrainPerSecond", "StaminaRecoveryPerSecond", "StaminaToResumeSprint"):
            self.assertIn(field, header)
            self.assertIn(field, source)
        self.assertIn("void ASportsArcadeCharacter::Tick(float DeltaSeconds)", source)
        self.assertIn("Super::Tick(DeltaSeconds)", source)
        self.assertIn("GetStaminaNormalized", source)

    def test_match_clock_score_and_goal_line_contract(self):
        header = read("Source/SportsArcade/Public/SportsArcadeGameMode.h")
        source = read("Source/SportsArcade/Private/SportsArcadeGameMode.cpp")
        for field in ("HalfDurationSeconds", "ElapsedMatchSeconds", "HomeGoals", "AwayGoals", "bMatchFinished", "RecordGoal"):
            self.assertIn(field, header)
        self.assertIn("ASportsArcadeGameMode::Tick(float DeltaSeconds)", source)
        self.assertIn("MatchBall->BallRadiusCm", source)
        self.assertIn("RecordGoal(true)", source)
        self.assertIn("RecordGoal(false)", source)
        self.assertIn("LastGoalTouchPlayer = MatchBall->GetLastTouchPlayer()", source)
        self.assertIn("LastGoalTouchPlayer = nullptr", source)
        self.assertIn("MatchBall->ResetToCentre()", source)
        self.assertIn("if (bMatchFinished || bHalfTime) return;", source)
        self.assertIn("bMatchFinished || bHalfTime) return false", source)
        self.assertIn("ResumeSecondHalf", source)
        self.assertIn("bHalfTime = true", source)
        self.assertIn("CrossesGoal(Threshold)", source)
        self.assertIn("CrossesGoal(-Threshold)", source)
        self.assertIn("PreviousBallPosition + Travel * T", source)
        self.assertIn("ResetMatch()", source)
        self.assertIn("GetMatchProgress() const", source)

    def test_native_review_mode_activates_broadcast_camera(self):
        header = read("Source/SportsArcade/Public/SportsArcadeGameMode.h")
        mode = read("Source/SportsArcade/Private/SportsArcadeGameMode.cpp")
        self.assertIn("virtual void BeginPlay() override;", header)
        self.assertIn("bUseBroadcastCameraIfPresent", header)
        self.assertIn("bTrackReferenceFootball", header)
        self.assertIn("TActorIterator<ASportsArcadeBroadcastCamera>", mode)
        self.assertIn("TActorIterator<ASportsArcadeBall>", mode)
        self.assertIn("SetFocusActor(*It)", mode)
        self.assertIn("SetViewTargetWithBlend(ReviewCamera, 0.0f)", mode)
        self.assertIn("if (!IsValid(ReviewCamera))", mode)

    def test_camera_can_track_and_clear_focus(self):
        header = read("Source/SportsArcade/Public/SportsArcadeBroadcastCamera.h")
        cpp = read("Source/SportsArcade/Private/SportsArcadeBroadcastCamera.cpp")
        for name in ("SetFocusActor", "ClearFocusActor"):
            self.assertIn(name, header)
            self.assertIn(name, cpp)
        self.assertIn("MaximumLateralTravel", cpp)
        self.assertIn("VInterpTo", cpp)

    def test_broadcast_camera_keeps_level_authored_starting_position(self):
        header = read("Source/SportsArcade/Public/SportsArcadeBroadcastCamera.h")
        cpp = read("Source/SportsArcade/Private/SportsArcadeBroadcastCamera.cpp")
        self.assertIn("virtual void BeginPlay() override;", header)
        self.assertIn("CameraOffset = GetActorLocation() - PitchCentre;", cpp)
        self.assertIn("Target + CameraOffset", cpp)
        self.assertNotIn("FVector(0.f, -8900.f, 3900.f)", header)

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

    def test_pitch_dimensions_and_markings(self):
        header = read("Source/SportsArcade/Public/SportsArcadePitch.h")
        source = read("Source/SportsArcade/Private/SportsArcadePitch.cpp")
        self.assertIn("10500.f", header)
        self.assertIn("6800.f", header)
        for dimension in ("915.f", "1650.f", "550.f", "1100.f"):
            self.assertIn(dimension, source)
        self.assertIn("AddCircle(0.f, 0.f, 915.f", source)
        self.assertIn("Lines->SetupAttachment(SceneRoot)", source)
        self.assertIn("Surface->SetupAttachment(SceneRoot)", source)
        self.assertIn("FVector Position(Middle.X, Middle.Y, 0.5f)", source)

    def test_goal_frame_and_corner_markings(self):
        goal_header = read("Source/SportsArcade/Public/SportsArcadeGoal.h")
        goal_source = read("Source/SportsArcade/Private/SportsArcadeGoal.cpp")
        pitch_source = read("Source/SportsArcade/Private/SportsArcadePitch.cpp")
        self.assertIn("732.f", goal_header)
        self.assertIn("244.f", goal_header)
        self.assertIn("InternalWidthCm", goal_source)
        self.assertIn("InternalHeightCm", goal_source)
        self.assertIn("FramePieces->ClearInstances()", goal_source)
        self.assertIn("Corner quarter-arcs", pitch_source)
        self.assertIn("XSide * 100.f", pitch_source)
        self.assertIn("YSide * 100.f", pitch_source)

    def test_daylight_rig_is_used_by_editor_preview(self):
        header = read("Source/SportsArcade/Public/SportsArcadeLightingRig.h")
        source = read("Source/SportsArcade/Private/SportsArcadeLightingRig.cpp")
        preview = read("Tools/Editor/setup_visual_review.py")
        for field in ("KeyLightLux", "SkylightIntensity", "SunDirection"):
            self.assertIn(field, header)
            self.assertIn(field, source)
        for component in ("UDirectionalLightComponent", "USkyLightComponent",
                          "USkyAtmosphereComponent", "UExponentialHeightFogComponent"):
            self.assertIn(component, header)
        self.assertIn('resolve_project_class("SportsArcadeLightingRig")', preview)
        self.assertIn('spawn(editor, lighting_class', preview)

    def test_ball_physics_contract_and_preview_placement(self):
        header = read("Source/SportsArcade/Public/SportsArcadeBall.h")
        source = read("Source/SportsArcade/Private/SportsArcadeBall.cpp")
        preview = read("Tools/Editor/setup_visual_review.py")
        for value in ("BallRadiusCm = 11.f", "BallMassKg = 0.43f",
                      "void Kick(", "void ResetToCentre()"):
            self.assertIn(value, header)
        for value in ("SetSimulatePhysics(true)", "bUseCCD = true",
                      "AddImpulse(", "TeleportPhysics"):
            self.assertIn(value, source)
        self.assertIn('resolve_project_class("SportsArcadeBall")', preview)
        self.assertIn('spawn(editor, ball_class', preview)

    def test_project_does_not_claim_compiled_or_rendered_artwork(self):
        readme = read("README.md").lower()
        self.assertRegex(readme, r"not been (?:opened, )?compiled|not been opened, compiled")
        self.assertIn("no stadium", readme)


if __name__ == "__main__":
    unittest.main()
