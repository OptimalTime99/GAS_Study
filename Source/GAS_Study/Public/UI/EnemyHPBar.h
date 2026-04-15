#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHPBar.generated.h"

class UProgressBar;

UCLASS()
class GAS_STUDY_API UEnemyHPBar : public UUserWidget
{
    GENERATED_BODY()

public:
    // UI에 배치된 ProgressBar와 C++ 변수를 이름으로 연결합니다.
    // 블루프린트에서 ProgressBar의 이름을 반드시 'HPProgressBar'로 지어야 합니다.
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> HPProgressBar;

    /** 비율에 따라 HP바 갱신 (0.0 ~ 1.0) */
    void UpdateHPPercent(float NewPercent);
};