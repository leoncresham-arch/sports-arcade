#include "SportsArcadeVisualAssetSet.h"

bool USportsArcadeVisualAssetSet::HasCoreVisualReferences() const
{
    // Checks assignment only, not mesh quality or third-party license terms.
    return !PlayerMesh.IsNull()
        && !IdleAnimation.IsNull()
        && !JogAnimation.IsNull()
        && !SprintAnimation.IsNull()
        && !TurfMaterial.IsNull()
        && !GoalFrameMesh.IsNull()
        && !GoalNetMaterial.IsNull()
        && !SourceAndLicenseNotes.IsEmpty();
}
