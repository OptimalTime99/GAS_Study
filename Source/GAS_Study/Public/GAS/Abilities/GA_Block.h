// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Block.generated.h"

UCLASS()
class GAS_STUDY_API UGA_Block : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UGA_Block();

    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility,
        bool bWasCancelled) override;

protected:
    /** 방어 처리 */
    void Defense();

    // 🌟 추가: 몽타주 재생이 끝났을 때 호출될 함수
    UFUNCTION()
    void OnMontageCompleted();

    /** 방어 버프 Effect 클래스 */
    UPROPERTY(EditDefaultsOnly, Category = "Block")
    TSubclassOf<UGameplayEffect> DefenseBuffEffect;

    /** 적용한 버프 기억 */
    FActiveGameplayEffectHandle ActiveDefenseBuffHandle;

    /** 방어 애니메이션 Montage */
    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    TObjectPtr<UAnimMontage> DefenseMontage;

    TWeakObjectPtr<class AGAS_StudyCharacter> Character;

    // 🌟 추가: 애니메이션에서 '방어 준비 완료' 신호를 보냈을 때 실행될 함수
    UFUNCTION()
    void OnDefenseReady(FGameplayEventData Payload);

    // 🌟 추가: 몽타주에서 기다릴 방어 준비 완료 태그 (예: Event.Defense.Ready)
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat|Tags")
    FGameplayTag DefenseReadyEventTag;
    
    /** 가드 유지 중 지속적으로 스태미나를 깎을 Effect 클래스 (Infinite, Period 1.0 등) */
    UPROPERTY(EditDefaultsOnly, Category = "Block")
    TSubclassOf<UGameplayEffect> StaminaDrainEffect;

    /** 스태미나 감소 이펙트 영수증 */
    FActiveGameplayEffectHandle ActiveStaminaDrainHandle;
};
