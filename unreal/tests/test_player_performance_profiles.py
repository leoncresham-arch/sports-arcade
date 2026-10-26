"""Per-footballer ratings and fitness-sensitive actions: bounded model checks."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1] / "Source/SportsArcade"
H = (ROOT / "Public/SportsArcadePlayerAttributes.h").read_text()
C = (ROOT / "Private/SportsArcadePlayerAttributes.cpp").read_text()
PLAYER = (ROOT / "Private/SportsArcadeCharacter.cpp").read_text()

def factor(rating, fitness, pressure):
    rating = min(99, max(1, rating))
    base = (0.70 + 0.30 * (rating - 1) / 64) if rating <= 65 else (1 + 0.30 * (rating - 65) / 34)
    resistance = (rating - 1) / 98
    return base * (0.78 + 0.22 * max(0, min(1, fitness))) * (1 - max(0, min(1, pressure)) * (0.24 - 0.16 * resistance))

class PlayerPerformanceTests(unittest.TestCase):
    def test_higher_rating_outperforms_lower_in_same_condition(self):
        for fitness in (0, 0.4, 1):
            for pressure in (0, 0.5, 1):
                self.assertGreater(factor(90, fitness, pressure), factor(40, fitness, pressure))

    def test_fatigue_and_pressure_reduce_output(self):
        for rating in (10, 65, 99):
            self.assertLess(factor(rating, 0.2, 0.8), factor(rating, 1, 0))
            self.assertGreater(factor(rating, 1, 0), 0)

    def test_existing_actions_use_individual_fitness(self):
        for skill in ("ShortPassing", "Finishing"):
            self.assertIn(f"ContextualRatingFactor(PlayerAttributes->{skill}, GetStaminaNormalized(), 0.f)", PLAYER)
        for skill in ("Vision", "LongPassing", "ShotPower", "DefensiveAwareness", "Composure", "Strength", "WeakFootAbility"):
            self.assertIn(f"int32 {skill} = 65;", H)
        self.assertIn("ContextualRatingFactor(", C)

if __name__ == "__main__":
    unittest.main()
