from pathlib import Path
import unittest

AI = Path(__file__).resolve().parents[1] / 'Source/SportsArcade/Private/SportsArcadeTeamAIController.cpp'

class LineupSafetyTests(unittest.TestCase):
    def test_outfielder_selection_ignores_goalkeeper_slot(self):
        code = AI.read_text()
        self.assertIn('for (ASportsArcadeCharacter* Candidate : Roster->Players)', code)
        self.assertIn('Candidate->LineupSlot == 0) continue;', code)
        self.assertNotIn('for (int32 Index = 1; Index < Roster->Players.Num(); ++Index)', code)
    def test_invalid_lineup_stops_navigation(self):
        code = AI.read_text()
        self.assertIn('if (MembershipIndex == INDEX_NONE || Slot < 0 || Slot > 10)', code)
        self.assertIn('StopMovement();\n        return;', code)
    def test_invalid_opponents_not_marked(self):
        code = AI.read_text()
        self.assertIn('if (!IsValid(*Opp)', code)
        self.assertIn('if (!IsValid(*It) || It->TeamIndex', code)

if __name__ == '__main__':
    unittest.main()
