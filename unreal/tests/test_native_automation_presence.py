from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
SOURCE = (ROOT / "Source/SportsArcade/Private/SportsArcadeAutomationTests.cpp").read_text()

RUNNER = (ROOT / "Tools/Windows/Run-SportsArcadeTests.ps1").read_text()

class NativeUnrealAutomationContracts(unittest.TestCase):
    def test_windows_native_runner(self):
        self.assertIn("UnrealEditor.exe", RUNNER)
        self.assertIn("Automation RunTests SportsArcade", RUNNER)
        self.assertIn("Automation Test Queue Empty", RUNNER)
        self.assertIn("Version.MinorVersion -ne 5", RUNNER)

    def test_editor_automation_tests_exist(self):
        self.assertIn("#if WITH_DEV_AUTOMATION_TESTS", SOURCE)
        self.assertEqual(SOURCE.count("IMPLEMENT_SIMPLE_AUTOMATION_TEST("), 5)
        self.assertIn("FSportsArcadeLeagueScheduleAutomation", SOURCE)
        self.assertIn("FSportsArcadeCupBracketAutomation", SOURCE)
        self.assertIn("FSportsArcadeTableAutomation", SOURCE)
        self.assertIn("FSportsArcadePenaltyShootoutAutomation", SOURCE)
        self.assertIn("FSportsArcadeCupSaveIntegrityAutomation", SOURCE)
        self.assertIn("Incorrect round index rejected", SOURCE)
    def test_native_tests_invoke_domain_logic(self):
        self.assertIn("USportsArcadeFixtures::GenerateLeagueFixtures", SOURCE)
        self.assertIn("USportsArcadeCupFixtures::GenerateNextRound", SOURCE)
        self.assertIn("USportsArcadeLeagueTable::CalculateTable", SOURCE)
        self.assertIn("Shootout->RecordAttempt", SOURCE)
        self.assertIn("Home second shooter slot", SOURCE)
        self.assertIn("Twelfth home kick wraps to opening striker", SOURCE)
        self.assertIn("Twelfth away kick also wraps to opening striker", SOURCE)
        self.assertIn("GetNextKickerLineupSlot()", SOURCE)
        self.assertIn("Invalid attempt ordering cannot choose a kicker", SOURCE)
        self.assertIn("Goals cannot exceed attempts", SOURCE)
