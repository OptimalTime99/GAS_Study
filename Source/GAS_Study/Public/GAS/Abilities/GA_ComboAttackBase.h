#pragma once

#include"CoreMinimal.h"
#include"Abilities/GameplayAbility.h"
#include"GA_ComboAttackBase.generated.h"

/**
 * 콤보 공격 베이스 클래스
 * Blueprint로 01, 02, 03 생성
 */
UCLASS()
class UGA_ComboAttackBase : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UGA_ComboAttackBase();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
    UAnimMontage* MontageToPlay;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
    FName SectionName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
    float Damage = 10.0f;

protected:
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

private:
    UFUNCTION()
    void OnMontageEnded();
};
