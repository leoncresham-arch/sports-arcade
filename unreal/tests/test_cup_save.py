from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
HEADER = (ROOT / "Source/SportsArcade/Public/SportsArcadeCupSave.h").read_text()
CPP = (ROOT / "Source/SportsArcade/Private/SportsArcadeCupSave.cpp").read_text()

class CupProgressTests(unittest.TestCase):
    def test_native_save_and_progression(self):
        self.assertIn("public USaveGame", HEADER)
        for method in ("StartCup", "RecordWinner", "AdvanceRound", "IsCupComplete"):
            self.assertIn(method, HEADER)
    def test_winner_matches_scheduled_fixture(self):
        self.assertIn("CurrentRoundFixtures.IsValidIndex(CurrentRoundWinners.Num())", CPP)
        self.assertIn("WinningClubId != Fixture.HomeClubId", CPP)
        self.assertIn("WinningClubId != Fixture.AwayClubId", CPP)
    def test_slot_persistence_and_load_validation(self):
        self.assertIn("SaveCupToSlot", HEADER)
        self.assertIn("LoadCupFromSlot", HEADER)
        self.assertIn("UGameplayStatics::SaveGameToSlot", CPP)
        self.assertIn("UGameplayStatics::LoadGameFromSlot", CPP)
        self.assertIn("Winner != Fixture.HomeClubId", CPP)
        self.assertIn("ChampionId != CurrentRoundWinners[0]", CPP)
        self.assertIn("Cup->ValidateProgress()", CPP)
        self.assertIn("if (SlotName.IsEmpty() || UserIndex < 0 || !ValidateProgress())", CPP)

    def test_completed_ties_match_bracket_stage(self):
        self.assertIn("PreviousCompletedTies = OpeningClubCount - 2 * FixtureCount;", CPP)
        self.assertIn("CompletedTies != PreviousCompletedTies + CurrentRoundWinners.Num()", CPP)
        self.assertIn("Round != ExpectedRound", CPP)
        self.assertIn("ChampionId.IsNone()) return false;", CPP)
        self.assertIn("if (!ValidateProgress() || IsCupComplete() || WinningClubId.IsNone()", CPP)
        for opening_count in (2, 4, 8, 16, 32, 64):
            fixture_count = opening_count // 2
            round_number = 1
            while fixture_count:
                previous_ties = opening_count - 2 * fixture_count
                for completed_in_round in range(fixture_count + 1):
                    total = previous_ties + completed_in_round
                    self.assertLessEqual(total, opening_count - 1)
                if fixture_count == 1:
                    self.assertEqual(total, opening_count - 1)
                    break
                fixture_count //= 2
                round_number += 1

    def test_cup_progress_and_next_pending_tie(self):
        self.assertIn("GetCupProgress() const", HEADER)
        self.assertIn("GetNextCupFixture(FSportsArcadeCupFixture& OutFixture)", HEADER)
        self.assertIn("++CompletedTies;", CPP)
        self.assertIn("OpeningClubCount = ClubIds.Num();", CPP)
        self.assertIn("OutFixture = CurrentRoundFixtures[CurrentRoundWinners.Num()]", CPP)

    def test_next_round_requires_every_winner(self):
        self.assertIn("CurrentRoundWinners.Num() != CurrentRoundFixtures.Num()", CPP)
        self.assertIn("GenerateNextRound(", CPP)
        self.assertIn("ChampionId = WinningClubId;", CPP)
