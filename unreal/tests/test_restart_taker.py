from pathlib import Path
import unittest

class RestartTakerContracts(unittest.TestCase):
    def test_taker_assignment_and_lifecycle(self):
        root = Path(__file__).resolve().parents[1]
        mode = (root / 'Source/SportsArcade/Private/SportsArcadeGameMode.cpp').read_text()
        header = (root / 'Source/SportsArcade/Public/SportsArcadeGameMode.h').read_text()
        self.assertIn('PendingRestartTaker', header)
        self.assertIn('float ProvisionalRestartImpulse = 1500.f;', header)
        self.assertIn('FindRestartTaker(PendingRestartTeam, PendingRestartLocation)', mode)
        self.assertIn('!IsValid(PendingRestartTaker)', mode)
        self.assertIn('PendingRestartTaker = nullptr;', mode)
        self.assertIn('PendingRestartType == ESportsArcadeRestartType::GoalKick', mode)
        self.assertIn('Candidate->LineupSlot == 0', mode)
        self.assertIn('PendingRestartTaker->TeamIndex == PendingRestartTeam', mode)
        self.assertIn('(PendingRestartTeam == 0 || PendingRestartTeam == 1)', mode)
        self.assertIn('bAutoKickProvisionalRestart', mode)
        self.assertIn('MatchBall->KickByPlayer(PendingRestartTaker, Direction, ProvisionalRestartImpulse, 210.f)', mode)
        self.assertIn('if (!PendingRestartTaker->SetActorLocation(TakerPosition', mode)
        self.assertIn('const FVector Direction = PendingRestartType', mode)
        self.assertIn('TActorIterator<ASportsArcadeCharacter> It(GetWorld())', mode)

if __name__ == '__main__':
    unittest.main()
