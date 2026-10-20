from pathlib import Path
import unittest
ROOT = Path(__file__).resolve().parents[1]
header = (ROOT / "Source/SportsArcade/Public/SportsArcadeGameMode.h").read_text()
cpp = (ROOT / "Source/SportsArcade/Private/SportsArcadeGameMode.cpp").read_text()
class CupMatchIntegration(unittest.TestCase):
    def test_knockout_binding(self):
        self.assertIn("BindNextCupFixture(USportsArcadeCupSave* Cup)", header)
        self.assertIn("Cup->CurrentRoundFixtures[Cup->CurrentRoundWinners.Num()]", cpp)
        self.assertIn("BoundCup = Cup;", cpp)
    def test_missing_cup_mapping_clears_stale_player_ids(self):
        self.assertIn("Home->ClearFootballerRecordIds()", cpp)
        self.assertIn("Away->ClearFootballerRecordIds()", cpp)

    def test_cup_teams_use_validated_catalog(self):
        self.assertIn("HomeClub.Category != AwayClub.Category", cpp)
        self.assertIn("Home->CanAssignFootballerRecords(FootballCatalog)", cpp)
        self.assertIn("Away->CanAssignFootballerRecords(FootballCatalog)", cpp)

    def test_completed_tie_cannot_be_replayed(self):
        self.assertIn("BoundCup->CurrentRoundWinners.Num() > BoundCupFixtureIndex", cpp)
        self.assertIn("BoundCupFixtureIndex = Cup->CurrentRoundWinners.Num()", cpp)
        self.assertIn("BoundCupFixtureIndex != BoundCup->CurrentRoundWinners.Num()", cpp)

    def test_draw_does_not_pick_winner(self):
        self.assertIn("HomeGoals == AwayGoals", cpp)
        self.assertIn("BoundCup->RecordWinner(", cpp)
        self.assertIn("CommitCupResult();", cpp)
    def test_completed_round_progression(self):
        self.assertIn("AdvanceBoundCupRound()", header)
        self.assertIn("BoundCup->AdvanceRound()", cpp)
        self.assertIn("return BindNextCupFixture(BoundCup);", cpp)

    def test_cup_ui_can_access_progress_and_save(self):
        self.assertIn("GetBoundCup() const", header)
        self.assertIn("SaveBoundCupToSlot", header)
        self.assertIn("BoundCup->SaveCupToSlot(SlotName, UserIndex)", cpp)

    def test_extra_time_ui_progress(self):
        self.assertIn("IsCupExtraTimeActive() const", header)
        self.assertIn("GetExtraTimeProgress() const", header)
        self.assertIn("ExtraTimeElapsedSeconds /", cpp)

    def test_two_extra_time_halves_before_penalties(self):
        self.assertIn("StartCupExtraTime()", header)
        self.assertIn("ResumeCupExtraTimeSecondHalf()", header)
        self.assertIn("IsCupExtraTimeRequired() const", header)
        self.assertIn("ExtraTimeElapsedSeconds + DeltaSeconds", cpp)
        self.assertIn("bExtraTimeSecondHalf = true;", cpp)
        self.assertIn("!bCupExtraTimePlayed", cpp)
        self.assertIn("if (bExtraTimeActive)", cpp)

    def test_shootout_start_rejects_stale_fixture(self):
        method = cpp.split("bool ASportsArcadeGameMode::StartCupPenaltyShootout()", 1)[1].split("\n}", 1)[0]
        self.assertIn("CurrentRoundFixtures.IsValidIndex(BoundCupFixtureIndex)", method)
        self.assertIn("CurrentRoundWinners.Num() != BoundCupFixtureIndex", method)
        self.assertIn("Tie.HomeClubId != SelectedHomeClubId", method)
        self.assertIn("Tie.AwayClubId != SelectedAwayClubId", method)
        self.assertLess(method.index("Tie.HomeClubId != SelectedHomeClubId"), method.index("NewObject<USportsArcadePenaltyShootout>"))

    def test_penalty_attempt_rejects_stale_cup_fixture_before_kicking(self):
        method = cpp.split("bool ASportsArcadeGameMode::RecordCupPenaltyAttempt(", 1)[1].split("\n}", 1)[0]
        for guard in (
            "CurrentRoundFixtures.IsValidIndex(BoundCupFixtureIndex)",
            "CurrentRoundWinners.Num() != BoundCupFixtureIndex",
            "Tie.HomeClubId != SelectedHomeClubId",
            "Tie.AwayClubId != SelectedAwayClubId",
        ):
            self.assertIn(guard, method)
        self.assertLess(
            method.index("Tie.HomeClubId != SelectedHomeClubId"),
            method.index("ActivePenaltyShootout->RecordAttempt("),
        )

    def test_physical_penalty_staging_is_opt_in_and_outcome_remains_refereed(self):
        self.assertIn("bool StageCupPenaltyKick();", header)
        self.assertIn("bool CanStageCupPenaltyKick() const;", header)
        self.assertIn("if (!CanStageCupPenaltyKick()) return false;", cpp)
        self.assertIn("BoundCup->CurrentRoundWinners.Num() != BoundCupFixtureIndex", cpp)
        self.assertIn("Fixture.HomeClubId == SelectedHomeClubId", cpp)
        self.assertIn("Fixture.AwayClubId == SelectedAwayClubId", cpp)
        self.assertIn("bool KickStagedCupPenalty(const FVector& Direction);", header)
        self.assertIn("bool bRequirePhysicalPenaltyKicks = false;", header)
        stage = cpp.split("bool ASportsArcadeGameMode::StageCupPenaltyKick()", 1)[1].split("\n}", 1)[0]
        strike = cpp.split("bool ASportsArcadeGameMode::KickStagedCupPenalty(", 1)[1].split("\n}", 1)[0]
        self.assertIn("ActivePenaltyShootout->GetNextKickingTeam()", stage)
        self.assertIn("MatchBall->PlaceAtRest(Spot)", stage)
        self.assertIn("FindRestartTaker(Team, Spot)", stage)
        self.assertIn("MatchBall->KickByPlayer(StagedPenaltyTaker", strike)
        self.assertIn("Direction.ContainsNaN()", strike)
        self.assertIn("(bRequirePhysicalPenaltyKicks && !bPenaltyKickTaken)", cpp)
        self.assertIn("bPenaltyKickTaken = false;", cpp)

    def test_opt_in_physical_penalties_require_a_strike_before_result(self):
        self.assertIn("bool StageCupPenaltyKick();", header)
        self.assertIn("bool KickStagedCupPenalty(const FVector& Direction);", header)
        self.assertIn("bRequirePhysicalPenaltyKicks", header)
        self.assertIn("bool IsCupPenaltyAwaitingResult() const;", header)
        self.assertIn("bool WasCupPenaltyTouchedByKeeper() const;", header)
        self.assertIn("int32 GetSuggestedCupPenaltyOutcome() const;", header)
        self.assertIn("void ASportsArcadeGameMode::ObservePhysicalCupPenalty()", cpp)
        self.assertIn("ObservePhysicalCupPenalty();", cpp)
        self.assertIn("SuggestedCupPenaltyOutcome = !bTouchlineFirst && bInsidePosts && bUnderBar ? 1 : 0;", cpp)
        self.assertIn("TouchlineFraction >= 0.f && TouchlineFraction < CrossingFraction", cpp)
        self.assertIn("return IsCupPenaltyAwaitingResult() ? SuggestedCupPenaltyOutcome : INDEX_NONE;", cpp)
        self.assertEqual(cpp.count("SuggestedCupPenaltyOutcome = INDEX_NONE;"), 5)
        self.assertIn("LastTouch->LineupSlot == 0", cpp)
        self.assertIn("LastTouch->TeamIndex == GetCupPenaltyDefendingTeam()", cpp)
        self.assertIn("bool ResolvePhysicalCupPenalty(bool bScored);", header)
        self.assertIn("return RecordCupPenaltyAttempt(KickingTeam, bScored);", cpp)
        self.assertIn("GetStagedCupPenaltyTaker() const;", header)
        self.assertIn("bPenaltyKickStaged && !bPenaltyKickTaken", cpp)
        self.assertIn("StagedPenaltyTaker->GetWorld() == GetWorld()", cpp)
        self.assertIn("bool ASportsArcadeGameMode::IsCupPenaltyAwaitingResult() const", cpp)
        self.assertIn("bPenaltyKickStaged = true;", cpp)
        self.assertIn("StagedPenaltySpot = Spot;", cpp)
        self.assertIn("ActivePenaltyShootout->GetNextKickerLineupSlot()", cpp)
        self.assertIn("if (ShooterSlot == INDEX_NONE) return false;", cpp)
        self.assertIn("Candidate->LineupSlot == ShooterSlot", cpp)
        self.assertIn("Keeper->TeamIndex != 1 - Team || Keeper->LineupSlot != 0", cpp)
        self.assertIn("Keeper->SetActorLocation(GoalLinePosition", cpp)
        self.assertIn("Taker->SetActorRotation(FRotator(0.f, Team == 0 ? 0.f : 180.f, 0.f))", cpp)
        self.assertIn("GoalX, 0.f, Keeper->GetActorLocation().Z", cpp)
        rotation = [9, 10, 8, 7, 6, 5, 4, 3, 2, 1, 0]
        self.assertEqual(len(rotation), 11)
        self.assertEqual(len(set(rotation)), 11)
        self.assertEqual([rotation[i % len(rotation)] for i in range(11)], rotation)
        self.assertEqual(rotation[11 % len(rotation)], rotation[0])
        self.assertEqual(cpp.count("StagedPenaltySpot = FVector::ZeroVector;"), 4)
        self.assertIn("FVector::DistSquared(MatchBall->GetActorLocation(), StagedPenaltySpot)", cpp)
        self.assertIn("bPenaltyKickTaken = true;", cpp)
        self.assertIn("FSportsArcadeMatchPhaseEvent OnPhysicalPenaltyKicked;", header)
        self.assertIn("OnPhysicalPenaltyKicked.Broadcast();", cpp)
        self.assertIn("FSportsArcadePenaltyReviewedEvent OnCupPenaltyReviewed;", header)
        self.assertIn("FSportsArcadePenaltyReviewedEvent OnCupPenaltyOutcomeSuggested;", header)
        self.assertIn("OnCupPenaltyOutcomeSuggested.Broadcast(Team, SuggestedCupPenaltyOutcome == 1);", cpp)
        self.assertIn("OnCupPenaltyReviewed.Broadcast(KickingTeam, bScored);", cpp)
        self.assertLess(cpp.index("bPenaltyKickTaken = true;"), cpp.index("OnPhysicalPenaltyKicked.Broadcast();"))
        self.assertIn("StagedPenaltyTaker->TeamIndex == ActivePenaltyShootout->GetNextKickingTeam()", cpp)
        self.assertIn("BoundCup->CurrentRoundWinners.Num() == BoundCupFixtureIndex", cpp)
        self.assertIn("CurrentRoundFixtures[BoundCupFixtureIndex].HomeClubId == SelectedHomeClubId", cpp)
        self.assertIn("bool CanKickStagedCupPenalty() const;", header)
        self.assertIn("if (!CanKickStagedCupPenalty()", cpp)
        self.assertIn("(bRequirePhysicalPenaltyKicks && !bPenaltyKickTaken)", cpp)
        self.assertIn("StagedPenaltyTaker = nullptr;", cpp)
        self.assertEqual(cpp.count("bool ASportsArcadeGameMode::StageCupPenaltyKick()"), 1)
        self.assertEqual(cpp.count("bool ASportsArcadeGameMode::KickStagedCupPenalty("), 1)
        self.assertIn("Direction.X * (StagedPenaltyTaker->TeamIndex == 0 ? 1.f : -1.f) <= 0.f", cpp)
        self.assertIn("Shot.X * (StagedPenaltyTaker->TeamIndex == 0 ? 1.f : -1.f) < 0.35f", cpp)
        self.assertIn("Shot.Z < -0.05f || Shot.Z > 0.65f", cpp)

    def test_penalty_winner_resolves_draw(self):
        self.assertIn("StartCupPenaltyShootout", header)
        self.assertIn("RecordCupPenaltyAttempt", header)
        self.assertIn("GetCupPenaltyShootout", header)
        self.assertIn("ActivePenaltyShootout->RecordAttempt(KickingTeam, bScored)", cpp)
        self.assertIn("bCupResultRecorded = BoundCup->RecordWinner(", cpp)

    def test_unresolved_draw_is_exposed_for_tie_break_ui(self):
        self.assertIn("IsCupTieBreakRequired() const", header)
        self.assertIn("!bCupResultRecorded && HomeGoals == AwayGoals", cpp)

    def test_post_match_result(self):
        self.assertIn("IsCupResultRecorded() const", header)
        self.assertIn("bMatchFinished && bCupResultRecorded", cpp)
