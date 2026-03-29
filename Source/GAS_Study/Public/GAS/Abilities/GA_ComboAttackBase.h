#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/GA_AttackBase.h" // 부모 클래스 헤더 인클루드
#include "Combat/Data/ComboData.h"
#include "GA_ComboAttackBase.generated.h"

UCLASS()
class GAS_STUDY_API UGA_ComboAttackBase : public UGA_AttackBase // UGameplayAbility -> UGA_AttackBase 로 변경
{
    GENERATED_BODY()

public:
    UGA_ComboAttackBase();

    // 삭제: 부모 클래스의 AttackMontage를 대신 사용합니다.
    // UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
    // UAnimMontage* MontageToPlay;

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

    // 삭제: 부모 클래스의 OnMontageCompleted()를 사용합니다.
    // void OnMontageEnded();
};
