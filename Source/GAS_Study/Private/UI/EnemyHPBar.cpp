#include "UI/EnemyHPBar.h"

#include "Components/ProgressBar.h"

void UEnemyHPBar::UpdateHPPercent(float NewPercent)
{
    if (HPProgressBar)
    {
        HPProgressBar->SetPercent(NewPercent);
    }
}
