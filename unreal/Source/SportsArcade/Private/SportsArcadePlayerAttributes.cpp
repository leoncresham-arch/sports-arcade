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

float USportsArcadePlayerAttributes::ContextualRatingFactor(
    int32 Rating, float FitnessNormalized, float PressureNormalized)
{
    // Authoring errors must not propagate invalid multipliers into gameplay.
    const float Fitness = FMath::IsFinite(FitnessNormalized)
        ? FMath::Clamp(FitnessNormalized, 0.f, 1.f) : 1.f;
    const float Pressure = FMath::IsFinite(PressureNormalized)
        ? FMath::Clamp(PressureNormalized, 0.f, 1.f) : 0.f;
    // A higher composure/technique rating retains relatively more ability
    // under pressure, while fatigue affects all players.
    const float Resistance = FMath::Clamp((FMath::Clamp(Rating, 1, 99) - 1.f) / 98.f, 0.f, 1.f);
    const float FatigueMultiplier = FMath::Lerp(0.78f, 1.f, Fitness);
    const float PressureMultiplier = 1.f - Pressure * FMath::Lerp(0.24f, 0.08f, Resistance);
    return RatingFactor(Rating) * FatigueMultiplier * PressureMultiplier;
}
