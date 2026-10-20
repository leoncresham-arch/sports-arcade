from pathlib import Path
import unittest

class TestKeeperSource(unittest.TestCase):
    def test_incoming_ball_parry(self):
        path = Path(__file__).resolve().parents[1] / 'Source/SportsArcade/Private/SportsArcadeTeamAIController.cpp'
        source = path.read_text()
        self.assertIn('bIncomingShot = BallVelocity.X * TowardsPlay <', source)
        self.assertIn('RawBallVelocity.ContainsNaN()', source)
        self.assertIn('RawBallVelocity.GetClampedToMaxSize(4500.f)', source)
        self.assertIn('const FVector Clearance = bIncomingShot', source)
        self.assertIn('GoalkeeperAnticipationSeconds', source)
        self.assertIn('const float Impulse = bIncomingShot ?', source)

if __name__ == '__main__':
    unittest.main()
