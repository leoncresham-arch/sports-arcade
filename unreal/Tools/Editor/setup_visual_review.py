"""
Sports Arcade native preview scene bootstrap (Unreal Editor Python).

UNTESTED AGAINST UNREAL ENGINE. This script must run inside a compatible
Unreal Editor with the project's C++ classes built and Python Editor Script
Plugin enabled. It cannot run on iPad or standard GitHub Actions runners.

Creates a new saved preview map, places field/goals/broadcast camera and
starter lighting; deliberately does not misrepresent placeholder geometry
as realistic player/stadium artwork.
"""
import unreal

LEVEL_PATH = "/Game/Maps/SA_VisualReview"
PITCH_HALF_LENGTH_CM = 5250.0
GOAL_X = (-PITCH_HALF_LENGTH_CM, PITCH_HALF_LENGTH_CM)


def log(message):
    unreal.log("[SportsArcadePreview] " + str(message))


def spawn(subsystem, actor_class, label, location, rotation=None):
    rotation = rotation or unreal.Rotator(0.0, 0.0, 0.0)
    actor = subsystem.spawn_actor_from_class(actor_class, location, rotation)
    if actor is None:
        raise RuntimeError("Could not spawn " + label)
    actor.set_actor_label(label)
    return actor


def resolve_project_class(class_name):
    # C++ reflected Unreal Engine classes have a /Script/Module.Class path.
    klass = unreal.load_class(None, "/Script/SportsArcade." + class_name)
    if klass is None:
        raise RuntimeError(
            class_name + " not available. Compile the SportsArcade C++ "
            "project with Unreal Editor before running this script."
        )
    return klass


def main():
    pitch_class = resolve_project_class("SportsArcadePitch")
    goal_class = resolve_project_class("SportsArcadeGoal")
    camera_class = resolve_project_class("SportsArcadeBroadcastCamera")

    editor = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    if editor is None:
        raise RuntimeError("Unreal EditorActorSubsystem not available")

    # Do not erase an artist's previously authored review level.
    if unreal.EditorAssetLibrary.does_asset_exist(LEVEL_PATH):
        raise RuntimeError(
            LEVEL_PATH + " already exists; refusing to overwrite it. "
            "Rename/move the existing asset before generating another."
        )

    unreal.EditorAssetLibrary.make_directory("/Game/Maps")
    if not unreal.EditorLevelLibrary.new_level(LEVEL_PATH):
        raise RuntimeError("Unable to create " + LEVEL_PATH)

    spawn(editor, pitch_class, "SA | Regulation 105x68 Pitch",
          unreal.Vector(0.0, 0.0, 0.0))
    for side, x in (("West", GOAL_X[0]), ("East", GOAL_X[1])):
        spawn(editor, goal_class, "SA | " + side + " Goal",
              unreal.Vector(x, 0.0, 0.0))

    camera = spawn(editor, camera_class, "SA | Broadcast Camera",
                   unreal.Vector(0.0, -8900.0, 3900.0))
    camera.set_actor_rotation(
        unreal.MathLibrary.find_look_at_rotation(
            camera.get_actor_location(), unreal.Vector(0.0, 0.0, 0.0)),
        False)

    sunlight = spawn(editor, unreal.DirectionalLight, "SA | Key Sunlight",
                     unreal.Vector(0.0, 0.0, 6000.0),
                     unreal.Rotator(-38.0, -35.0, 0.0))
    sunlight.get_component_by_class(
        unreal.DirectionalLightComponent).set_editor_property("intensity", 9.0)

    skylight = spawn(editor, unreal.SkyLight, "SA | Skylight",
                     unreal.Vector(0.0, 0.0, 2500.0))
    sky_component = skylight.get_component_by_class(unreal.SkyLightComponent)
    sky_component.set_editor_property("intensity", 1.0)

    spawn(editor, unreal.SkyAtmosphere, "SA | Atmosphere",
          unreal.Vector(0.0, 0.0, 0.0))
    spawn(editor, unreal.ExponentialHeightFog, "SA | Subtle Fog",
          unreal.Vector(0.0, 0.0, 0.0))

    if not unreal.EditorLevelLibrary.save_current_level():
        raise RuntimeError("Scene constructed, but could not save preview level")

    log("Saved native scene to " + LEVEL_PATH)
    log("This is a construction scene, NOT a photorealistic graphics preview.")
    log("Next: legally sourced player/animation/stadium materials and real UE render.")


if __name__ == "__main__":
    main()
