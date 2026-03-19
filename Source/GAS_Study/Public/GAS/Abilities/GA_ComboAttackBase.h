#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Combat/Data/ComboData.h"
#include "GA_ComboAttackBase.generated.h"

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

protected:
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    virtual bool CanActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayTagContainer* SourceTags = nullptr,
        const FGameplayTagContainer* TargetTags = nullptr,
        OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
    
    const FComboData* FindMyComboData(const FGameplayAbilityActorInfo* ActorInfo) const;
    
    UPROPERTY(EditDefaultsOnly, Category = "Combo")
    TSubclassOf<UGameplayEffect> StaminaCostEffectClass;
private:
    UFUNCTION()
    void OnMontageEnded();
};
