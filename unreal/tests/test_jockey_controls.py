from pathlib import Path
import unittest

BASE = Path(__file__).resolve().parents[1]
class TestJockeyControls(unittest.TestCase):
    def test_jockey_input_and_state(self):
        cpp = (BASE / 'Source/SportsArcade/Private/SportsArcadeCharacter.cpp').read_text()
        header = (BASE / 'Source/SportsArcade/Public/SportsArcadeCharacter.h').read_text()
        ini = (BASE / 'Config/DefaultInput.ini').read_text()
        self.assertIn('ActionName="Jockey",Key=LeftAlt', ini)
        self.assertIn('void ASportsArcadeCharacter::BeginJockey()', cpp)
        self.assertIn('void ASportsArcadeCharacter::EndJockey()', cpp)
        self.assertIn('!bJockeyHeld && bSprintHeld', cpp)
        self.assertIn('float JockeySpeed = 275.f;', header)

if __name__ == '__main__':
    unittest.main()
