"""Unreal Editor-only diagnostics for an opened Sports Arcade match map.
Read-only: does not spawn, move, delete or save any actors.
Unverified against a running Unreal Engine 5.5 editor.
"""
import unreal

def validate():
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    if actor_subsystem is None:
        raise RuntimeError("EditorActorSubsystem is unavailable")

    rosters = []
    balls = []
    players = []
    for actor in actor_subsystem.get_all_level_actors():
        name = actor.get_class().get_name()
        if name == "SportsArcadeTeamRoster":
            rosters.append(actor)
        elif name == "SportsArcadeBall":
            balls.append(actor)
        elif name == "SportsArcadeCharacter":
            players.append(actor)

    errors = []
    if len(balls) != 1:
        errors.append("Expected one match ball, found %d" % len(balls))
    if len(rosters) != 2:
        errors.append("Expected two team rosters, found %d" % len(rosters))
    if len(players) != 22:
        errors.append("Expected 22 footballers, found %d" % len(players))

    for team in (0, 1):
        team_rosters = [r for r in rosters if r.get_editor_property("team_index") == team]
        if len(team_rosters) != 1:
            errors.append("Expected one roster for team %d" % team)
            continue
        team_players = team_rosters[0].get_editor_property("players")
        if len(team_players) != 11:
            errors.append("Team %d has %d roster members" % (team, len(team_players)))
        slots = []
        for player in team_players:
            if player is None:
                errors.append("Team %d roster contains a null player" % team)
                continue
            if player.get_editor_property("team_index") != team:
                errors.append("Roster member has wrong team: %d" % team)
            slots.append(player.get_editor_property("lineup_slot"))
        if len(set(slots)) != len(slots) or sorted(slots) != list(range(11)):
            errors.append("Team %d lineup slots must be unique 0-10" % team)

    for problem in errors:
        unreal.log_error("[SportsArcade] " + problem)
    if not errors:
        unreal.log("[SportsArcade] Match actor and lineup checks passed (not gameplay verification)")
    return errors

if __name__ == "__main__":
    validate()
