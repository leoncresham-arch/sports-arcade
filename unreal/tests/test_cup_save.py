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

    def test_next_round_requires_every_winner(self):
        self.assertIn("CurrentRoundWinners.Num() != CurrentRoundFixtures.Num()", CPP)
        self.assertIn("GenerateNextRound(", CPP)
        self.assertIn("ChampionId = WinningClubId;", CPP)
