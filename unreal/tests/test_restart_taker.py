from pathlib import Path
import unittest

class RestartTakerContracts(unittest.TestCase):
    def test_taker_assignment_and_lifecycle(self):
        root = Path(__file__).resolve().parents[1]
        mode = (root / 'Source/SportsArcade/Private/SportsArcadeGameMode.cpp').read_text()
        header = (root / 'Source/SportsArcade/Public/SportsArcadeGameMode.h').read_text()
        self.assertIn('PendingRestartTaker', header)
        self.assertIn('FindRestartTaker(PendingRestartTeam, PendingRestartLocation)', mode)
        self.assertIn('!IsValid(PendingRestartTaker)', mode)
        self.assertIn('PendingRestartTaker = nullptr;', mode)

if __name__ == '__main__':
    unittest.main()
