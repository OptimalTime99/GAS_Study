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
    
protected:
    // 🌟 이벤트가 들어왔을 때 실행될 함수
    UFUNCTION()
    void OnHitEventReceived(FGameplayEventData Payload);

    // 🌟 노티파이에 있던 설정들을 어빌리티로 이사시킵니다.
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat|Damage")
    TSubclassOf<class UGameplayEffect> DamageEffectClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat|Trace")
    float TraceRadius = 50.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat|Trace")
    float TraceStartDistance = 50.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat|Trace")
    float TraceEndDistance = 150.0f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat|Debug")
    bool bShowDebug = false;
};
