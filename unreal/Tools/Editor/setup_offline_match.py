"""Unverified Unreal Editor 11v11 test-map generator. Never overwrite maps."""
import unreal

MAP = "/Game/Maps/SA_Offline11v11"

def main():
    names = ["SportsArcadePitch", "SportsArcadeGoal", "SportsArcadeBall",
             "SportsArcadeTeamRoster", "SportsArcadeCharacter",
             "SportsArcadeLightingRig"]
    classes = {name: unreal.load_class(None, "/Script/SportsArcade." + name)
               for name in names}
    if any(klass is None for klass in classes.values()):
        raise RuntimeError("Compile all native SportsArcade classes first")
    actor_editor = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    level_editor = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    if actor_editor is None or level_editor is None:
        raise RuntimeError("Unreal editor subsystems unavailable")
    if unreal.EditorAssetLibrary.does_asset_exist(MAP):
        raise RuntimeError("Refusing to overwrite " + MAP)
    unreal.EditorAssetLibrary.make_directory("/Game/Maps")
    if not level_editor.new_level(MAP):
        raise RuntimeError("Could not create exhibition map")

    def spawn(name, label, position):
        actor = actor_editor.spawn_actor_from_class(
            classes[name], position, unreal.Rotator(0, 0, 0))
        if actor is None:
            raise RuntimeError("Unable to spawn " + label)
        actor.set_actor_label(label)
        return actor

    spawn("SportsArcadePitch", "Match Pitch", unreal.Vector(0, 0, 0))
    spawn("SportsArcadeBall", "Match Football", unreal.Vector(0, 0, 11))
    spawn("SportsArcadeLightingRig", "Match Daylight", unreal.Vector(0, 0, 0))
    for x in (-5250, 5250):
        spawn("SportsArcadeGoal", "Goal " + str(x), unreal.Vector(x, 0, 0))
    for team in (0, 1):
        roster = spawn("SportsArcadeTeamRoster", "Roster " + str(team),
                       unreal.Vector(0, 0, 0))
        roster.set_editor_property("team_index", team)
        players = []
        for slot in range(11):
            target = roster.get_formation_position(slot)
            player = spawn("SportsArcadeCharacter",
                           "Team %s slot %s" % (team, slot),
                           unreal.Vector(target.x, target.y, 95))
            player.set_editor_property("team_index", team)
            player.set_editor_property("lineup_slot", slot)
            player.set_editor_property("shirt_number", slot + 1)
            players.append(player)
        roster.set_editor_property("players", players)
    if not level_editor.save_current_level():
        raise RuntimeError("Could not save exhibition map")

if __name__ == "__main__":
    main()
