// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_AttackBase.generated.h"

/**
 * 
 */
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

    virtual bool CanActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayTagContainer* SourceTags = nullptr,
        const FGameplayTagContainer* TargetTags = nullptr,
        OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
    
protected:
    /** 공격 처리 */
    void Attack();
    
    // 🌟 추가: 몽타주 재생이 끝났을 때 호출될 함수
    UFUNCTION()
    void OnMontageCompleted();

    /** 공격 애니메이션 Montage */
    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    TObjectPtr<UAnimMontage> AttackMontage;
    
    TWeakObjectPtr<class AGAS_StudyCharacter> Character;
};
