#include "SportsArcadeGameMode.h"
#include "SportsArcadeCharacter.h"

ASportsArcadeGameMode::ASportsArcadeGameMode()
{
    DefaultPawnClass = ASportsArcadeCharacter::StaticClass();
}
