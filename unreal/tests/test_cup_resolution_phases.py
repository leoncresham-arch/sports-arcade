"""Source-contract checks for knockout presentation phases (not UE runtime tests)."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
HEADER = (ROOT / "Source/SportsArcade/Public/SportsArcadeGameMode.h").read_text()
SOURCE = (ROOT / "Source/SportsArcade/Private/SportsArcadeGameMode.cpp").read_text()


class KnockoutPresentationPhaseTests(unittest.TestCase):
    def test_distinct_cup_resolution_phases_are_blueprint_visible(self):
        self.assertIn("UENUM(BlueprintType)\nenum class ESportsArcadeMatchPhase", HEADER)
        for phase in (
            "AwaitingExtraTime", "ExtraTimeFirstHalf", "ExtraTimeInterval",
            "ExtraTimeSecondHalf", "AwaitingPenalties", "PenaltyShootout"
        ):
            self.assertIn(phase + ",", HEADER.replace("PenaltyShootout\n};", "PenaltyShootout,\n};"))
            self.assertIn("ESportsArcadeMatchPhase::" + phase, SOURCE)

    def test_unresolved_cup_draw_precedes_generic_full_time(self):
        method = SOURCE.split(
            "ESportsArcadeMatchPhase ASportsArcadeGameMode::GetCurrentMatchPhase() const", 1
        )[1].split("\n}", 1)[0]
        self.assertLess(
            method.index("if (IsValid(BoundCup) && bMatchFinished"),
            method.index("if (bMatchFinished) return ESportsArcadeMatchPhase::FullTime;")
        )
        self.assertIn("!bCupResultRecorded && HomeGoals == AwayGoals", method)
        self.assertIn("if (IsValid(ActivePenaltyShootout))", method)

    def test_normalized_regulation_and_extra_time_clock(self):
        self.assertIn("float GetMatchClockMinutes() const;", HEADER)
        clock = SOURCE.split("float ASportsArcadeGameMode::GetMatchClockMinutes() const", 1)[1].split("\n}", 1)[0]
        self.assertIn("90.f * FMath::Clamp(", clock)
        self.assertIn("ElapsedMatchSeconds / FMath::Max(2.f * HalfDurationSeconds", clock)
        self.assertIn("if (!bCupExtraTimePlayed) return RegulationMinutes;", clock)
        self.assertIn("30.f * FMath::Clamp(", clock)
        self.assertIn("ExtraTimeElapsedSeconds / FMath::Max(2.f * ExtraTimeHalfDurationSeconds", clock)

    def test_period_number_tracks_regulation_and_extra_time(self):
        self.assertIn("int32 GetMatchPeriodNumber() const;", HEADER)
        method = SOURCE.split("int32 ASportsArcadeGameMode::GetMatchPeriodNumber() const", 1)[1].split("\n}", 1)[0]
        for field in ("bCupExtraTimePlayed", "bExtraTimeSecondHalf", "ExtraTimeElapsedSeconds", "bSecondHalfStarted", "ElapsedMatchSeconds"):
            self.assertIn(field, method)
        self.assertIn("? 4 : 3", method)
        self.assertIn("? 2 : 1", method)

    def test_extra_time_interval_precedes_generic_halftime(self):
        method = SOURCE.split(
            "ESportsArcadeMatchPhase ASportsArcadeGameMode::GetCurrentMatchPhase() const", 1
        )[1].split("\n}", 1)[0]
        self.assertLess(
            method.index("if (bExtraTimeActive && bHalfTime)"),
            method.index("if (bHalfTime) return")
        )
        self.assertIn("if (bExtraTimeActive)", method)
        self.assertIn("bExtraTimeSecondHalf", method)


if __name__ == "__main__":
    unittest.main()
