from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
SOURCE = (ROOT / "Source/SportsArcade/Private/SportsArcadeAutomationTests.cpp").read_text()

class NativeUnrealAutomationContracts(unittest.TestCase):
    def test_editor_automation_tests_exist(self):
        self.assertIn("#if WITH_DEV_AUTOMATION_TESTS", SOURCE)
        self.assertEqual(SOURCE.count("IMPLEMENT_SIMPLE_AUTOMATION_TEST("), 3)
        self.assertIn("FSportsArcadeLeagueScheduleAutomation", SOURCE)
        self.assertIn("FSportsArcadeCupBracketAutomation", SOURCE)
        self.assertIn("FSportsArcadeTableAutomation", SOURCE)
    def test_native_tests_invoke_domain_logic(self):
        self.assertIn("USportsArcadeFixtures::GenerateLeagueFixtures", SOURCE)
        self.assertIn("USportsArcadeCupFixtures::GenerateNextRound", SOURCE)
        self.assertIn("USportsArcadeLeagueTable::CalculateTable", SOURCE)
