#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_AttackBase.generated.h"

UCLASS()
class GAS_STUDY_API UGA_AttackBase : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UGA_AttackBase();

    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    // AnimNotifyState에서 호출할 Trace Window 함수
    void BeginTraceWindow();
    void EndTraceWindow();

    UFUNCTION()
    void OnMontageCompleted();

protected:
    void Attack();
    
    // 타이머가 반복 호출할 실제 Trace 함수
    UFUNCTION()
    void PerformMeleeTrace();

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
    TObjectPtr<UAnimMontage> AttackMontage;

    TWeakObjectPtr<class AGAS_StudyCharacter> Character;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat|Damage")
    TSubclassOf<class UGameplayEffect> DamageEffectClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat|Trace")
    float TraceRadius = 50.0f;

    // 트레이스할 소켓 이름
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat|Trace")
    FName TraceSocketName = FName("hand_r");

    // 트레이스 간격 (약 60FPS)
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat|Trace")
    float TraceInterval = 0.016f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat|Debug")
    bool bShowDebug = false;

private:
    // Trace 상태 및 타이머 관리, 중복 타격 방지용 TSet
    bool bIsTracing = false;
    FTimerHandle TraceTimerHandle;

    // 공격 1회(Trace Window)당 Hit한 액터들을 기록하여 중복 데미지 방지
    UPROPERTY()
    TSet<AActor*> HitActors;
};
