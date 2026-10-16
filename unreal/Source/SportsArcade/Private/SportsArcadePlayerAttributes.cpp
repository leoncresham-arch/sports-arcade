#include "SportsArcadePlayerAttributes.h"

USportsArcadePlayerAttributes::USportsArcadePlayerAttributes()
{
    PrimaryComponentTick.bCanEverTick = false;
}

float USportsArcadePlayerAttributes::RatingFactor(int32 Rating)
{
    // A neutral 65 rating produces 1x; endpoint ratings remain bounded.
    return FMath::Lerp(0.70f, 1.30f, (FMath::Clamp(Rating, 1, 99) - 1.f) / 98.f);
}
