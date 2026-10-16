#include "SportsArcadePlayerAttributes.h"

USportsArcadePlayerAttributes::USportsArcadePlayerAttributes()
{
    PrimaryComponentTick.bCanEverTick = false;
}

float USportsArcadePlayerAttributes::RatingFactor(int32 Rating)
{
    // A neutral 65 rating produces 1x; endpoint ratings remain bounded.
    const int32 ClampedRating = FMath::Clamp(Rating, 1, 99);
    return ClampedRating <= 65
        ? FMath::Lerp(0.70f, 1.f, (ClampedRating - 1.f) / 64.f)
        : FMath::Lerp(1.f, 1.30f, (ClampedRating - 65.f) / 34.f);
}
