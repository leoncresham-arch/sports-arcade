from pathlib import Path
import unittest

class AIShootingStatsCheck(unittest.TestCase):
    def test_ai_shots_only_in_attacking_third(self):
        source = (Path(__file__).resolve().parents[1] / 'Source/SportsArcade/Private/SportsArcadeTeamAIController.cpp').read_text()
        self.assertIn('Match->RecordShotAttempt(Footballer->TeamIndex)', source)
        self.assertIn('Ball->GetActorLocation().X * AttackSign > 1500.f', source)

if __name__ == '__main__':
    unittest.main()
