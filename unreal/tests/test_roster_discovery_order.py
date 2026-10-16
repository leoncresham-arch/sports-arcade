from pathlib import Path
import unittest

class RosterOrderChecks(unittest.TestCase):
    def test_auto_discovery_sort_and_manual_preservation(self):
        src = (Path(__file__).resolve().parents[1] / 'Source/SportsArcade/Private/SportsArcadeTeamRoster.cpp').read_text()
        self.assertIn('const bool bRosterWasEmpty = Players.IsEmpty();', src)
        self.assertIn('if (bRosterWasEmpty)', src)
        self.assertIn('Players.Sort(', src)
        self.assertIn('A->ShirtNumber < B->ShirtNumber', src)
        self.assertIn('A->GetName() < B->GetName()', src)

    def test_lineup_completeness_validation(self):
        src = (Path(__file__).resolve().parents[1] / 'Source/SportsArcade/Private/SportsArcadeTeamRoster.cpp').read_text()
        self.assertIn('bool ASportsArcadeTeamRoster::HasCompleteLineup() const', src)
        self.assertIn('Players.Num() != 11', src)
        self.assertIn('Slots[Player->LineupSlot]', src)

if __name__ == '__main__':
    unittest.main()
