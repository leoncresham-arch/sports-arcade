"""License gate regression tests; no third-party files or UE editor required."""
import copy
import json
import pathlib
import sys
import tempfile
import unittest

UNREAL_ROOT = pathlib.Path(__file__).resolve().parents[1]
sys.path.insert(0, str(UNREAL_ROOT / "asset_pipeline"))
from validate_manifest import load_manifest, validate_manifest


class ManifestTests(unittest.TestCase):
    def setUp(self):
        self.base = pathlib.Path("/tmp/sports-arcade-test-assets")
        self.good = {
            "schema_version": 1,
            "assets": [{
                "id": "test_player",
                "role": "player_skeletal_mesh",
                "source_url": "https://example.org/model",
                "creator": "Test creator",
                "license_name": "Test redistribution-approved license",
                "license_url": "https://example.org/license",
                "allows_game_use": True,
                "allows_public_source_redistribution": False,
                "local_file": "private/model.fbx",
                "destination": "/Game/SportsArcade/Players",
                "reviewed": True,
            }]
        }

    def test_valid_reviewed_asset_is_accepted_without_local_file_check(self):
        self.assertEqual(validate_manifest(self.good, self.base), [])

    def test_example_manifest_fails_closed(self):
        template = UNREAL_ROOT / "asset_pipeline/manifest.example.json"
        data, errors = load_manifest(template)
        self.assertEqual(data["schema_version"], 1)
        self.assertTrue(errors)
        self.assertTrue(any("game-use permission" in error for error in errors))

    def test_game_use_permission_required(self):
        data = copy.deepcopy(self.good)
        data["assets"][0]["allows_game_use"] = False
        self.assertTrue(validate_manifest(data, self.base))

    def test_reviewed_flag_required(self):
        data = copy.deepcopy(self.good)
        data["assets"][0]["reviewed"] = False
        self.assertTrue(validate_manifest(data, self.base))

    def test_duplicate_ids_rejected(self):
        data = copy.deepcopy(self.good)
        data["assets"].append(copy.deepcopy(data["assets"][0]))
        self.assertTrue(any("duplicate" in p for p in validate_manifest(data, self.base)))

    def test_directory_escape_rejected(self):
        data = copy.deepcopy(self.good)
        data["assets"][0]["local_file"] = "../../private.fbx"
        self.assertTrue(any("unsafe" in p for p in validate_manifest(data, self.base)))

    def test_untrusted_destination_rejected(self):
        data = copy.deepcopy(self.good)
        data["assets"][0]["destination"] = "/Engine/Editor"
        self.assertTrue(any("destination" in p for p in validate_manifest(data, self.base)))

    def test_disallow_unreviewed_executable_file(self):
        data = copy.deepcopy(self.good)
        data["assets"][0]["local_file"] = "private/setup.py"
        errors = validate_manifest(data, self.base)
        self.assertTrue(any("extension" in error for error in errors))

    def test_symlink_escape_is_rejected(self):
        data = copy.deepcopy(self.good)
        with tempfile.TemporaryDirectory() as working, tempfile.TemporaryDirectory() as outside:
            root = pathlib.Path(working)
            external = pathlib.Path(outside) / "player.fbx"
            external.write_bytes(b"external asset")
            (root / "private").mkdir()
            (root / "private" / "model.fbx").symlink_to(external)
            errors = validate_manifest(data, root, require_files=True)
            self.assertTrue(any("outside staging" in error for error in errors))

    def test_reject_destination_traversal(self):
        data = copy.deepcopy(self.good)
        data["assets"][0]["destination"] = "/Game/SportsArcade/Players/../Hidden"
        self.assertTrue(any("destination" in error for error in validate_manifest(data, self.base)))

    def test_file_existence_check(self):
        with tempfile.TemporaryDirectory() as temp:
            p = pathlib.Path(temp)
            model = p / "private/model.fbx"
            model.parent.mkdir()
            model.write_bytes(b"unit test asset")
            self.assertEqual(validate_manifest(self.good, p, require_files=True), [])
            model.unlink()
            self.assertTrue(any("missing local file" in e for e in validate_manifest(self.good, p, require_files=True)))


if __name__ == "__main__":
    unittest.main()
