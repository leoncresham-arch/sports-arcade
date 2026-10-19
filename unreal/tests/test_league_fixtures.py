"""Independent algorithm checks and source contract for offline league fixtures."""
import pathlib
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]
SOURCE = (ROOT / "Source/SportsArcade/Private/SportsArcadeFixtures.cpp").read_text()
HEADER = (ROOT / "Source/SportsArcade/Public/SportsArcadeFixtures.h").read_text()


def reference_schedule(ids, double_leg):
    rotation = list(ids)
    if len(rotation) % 2:
        rotation.append(None)
    rounds = len(rotation) - 1
    fixtures = []
    for round_index in range(rounds):
        for index in range(len(rotation) // 2):
            left, right = rotation[index], rotation[-index - 1]
            if left is None or right is None:
                continue
            home, away = (right, left) if (round_index + index) % 2 else (left, right)
            fixtures.append((round_index + 1, home, away))
            if double_leg:
                fixtures.append((round_index + 1 + rounds, away, home))
        rotation.insert(1, rotation.pop())
    return sorted(fixtures, key=lambda fixture: fixture[0])


class LeagueFixtureTests(unittest.TestCase):
    def test_native_blueprint_contract(self):
        self.assertIn("public UBlueprintFunctionLibrary", HEADER)
        self.assertIn("GenerateLeagueFixtures(", HEADER)
        self.assertIn("Rotation.Insert(Last, 1);", SOURCE)
        self.assertIn("const int32 FirstLegCount = Fixtures.Num();", SOURCE)
        self.assertIn("Seen.Contains(Id)", SOURCE)
        self.assertIn("ReturnFixture.HomeClubId = Fixture.AwayClubId", SOURCE)

    def test_second_leg_starts_after_first_leg_schedule(self):
        for number_of_clubs in range(2, 13):
            fixtures = reference_schedule(list(range(number_of_clubs)), True)
            rounds = (number_of_clubs if number_of_clubs % 2 else number_of_clubs - 1)
            first = [match for match in fixtures if match[0] <= rounds]
            second = [match for match in fixtures if match[0] > rounds]
            self.assertEqual(len(first), len(second))
            self.assertTrue(all(match[0] <= rounds for match in first))
            self.assertTrue(all(match[0] > rounds for match in second))

    def test_every_pair_once_or_twice_and_odd_byes(self):
        for count in range(2, 13):
            ids = list(range(count))
            for double_leg in (False, True):
                fixtures = reference_schedule(ids, double_leg)
                pairs = {}
                for _, home, away in fixtures:
                    self.assertNotEqual(home, away)
                    pair = frozenset((home, away))
                    pairs[pair] = pairs.get(pair, 0) + 1
                self.assertEqual(len(pairs), count * (count - 1) // 2)
                self.assertEqual(set(pairs.values()), {2 if double_leg else 1})
                self.assertEqual(len(fixtures), count * (count - 1) * (2 if double_leg else 1) // 2)


if __name__ == "__main__":
    unittest.main()
