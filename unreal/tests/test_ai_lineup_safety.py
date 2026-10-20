from pathlib import Path
import unittest

AI = Path(__file__).resolve().parents[1] / 'Source/SportsArcade/Private/SportsArcadeTeamAIController.cpp'

class LineupSafetyTests(unittest.TestCase):
    def test_outfielder_selection_ignores_goalkeeper_slot(self):
        code = AI.read_text()
        self.assertIn('for (ASportsArcadeCharacter* Candidate : Roster->Players)', code)
        self.assertIn('Candidate->LineupSlot == 0 ||', code)
        self.assertNotIn('for (int32 Index = 1; Index < Roster->Players.Num(); ++Index)', code)
    def test_human_controlled_player_does_not_suppress_ai_pressing(self):
        code = AI.read_text()
        self.assertIn('Candidate->GetWorld() != GetWorld()', code)
        self.assertIn('!Cast<ASportsArcadeTeamAIController>(Candidate->GetController())', code)

    def test_pass_receiver_pressure_is_evaluated(self):
        code = AI.read_text()
        self.assertIn("ReceiverSeparation < 210.f", code)
        self.assertIn("ReceiverPressurePenalty = FMath::Max(", code)
        self.assertIn("Distance * 0.25f - ReceiverPressurePenalty", code)
        self.assertIn("Delta.X * AttackSign < -650.f", code)
        self.assertIn("Teammate->GetWorld() != GetWorld()", code)

    def test_goalkeeper_only_reacts_during_physical_penalties(self):
        code = AI.read_text()
        self.assertIn("Match->IsCupPenaltyAwaitingResult()", code)
        self.assertIn("Keeper->TeamIndex != Match->GetCupPenaltyDefendingTeam()", code)
        self.assertIn("Keeper->LineupSlot != 0", code)
        self.assertIn("MoveToLocation(Target, 60.f", code)
        self.assertIn("Ball->KickByPlayer(Keeper, Parry", code)
        self.assertIn("Ball->GetVelocity().X * GoalwardSign > 175.f", code)
        self.assertIn("bIncomingPenalty && ActionCooldownRemaining <= 0.f", code)
        self.assertLess(code.index("Match->IsCupPenaltyAwaitingResult()"),
                        code.index("Match->bMatchFinished || Match->bBallOutOfPlay"))

    def test_invalid_lineup_stops_navigation(self):
        code = AI.read_text()
        self.assertIn('if (MembershipIndex == INDEX_NONE || Slot < 0 || Slot > 10 ||', code)
        self.assertIn('StopMovement();\n        return;', code)
    def test_invalid_opponents_not_marked(self):
        code = AI.read_text()
        self.assertIn('if (!IsValid(*Opp)', code)
        self.assertIn('if (!IsValid(*It) || It->TeamIndex', code)

    def test_roster_absence_and_invalid_team_stop_navigation(self):
        code = AI.read_text()
        self.assertIn('if (!IsValid(Roster))', code)
        self.assertIn('Footballer->TeamIndex < 0 || Footballer->TeamIndex > 1', code)

    def test_destination_arrival_suppresses_path_spam(self):
        code = AI.read_text()
        self.assertIn('FVector::DistSquared2D(Footballer->GetActorLocation(), Target)', code)
        self.assertIn('FMath::Square(125.f)', code)
        self.assertIn('MoveToLocation(Target, 125.f', code)
        self.assertIn('if (!IsValid(*It)) continue;', code)

if __name__ == '__main__':
    unittest.main()
