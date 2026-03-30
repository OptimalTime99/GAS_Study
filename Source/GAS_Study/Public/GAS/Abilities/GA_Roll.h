// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Roll.generated.h"

UCLASS()
class GAS_STUDY_API UGA_Roll : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UGA_Roll();

    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;
    
    UFUNCTION()
    void OnMontageCompleted();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dodge|Montage")
    TObjectPtr<UAnimMontage> DodgeMontage;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dodge|Montage")
    float DodgePlayRate;
    
    TWeakObjectPtr<class AGAS_StudyCharacter> Character;
    
private:
    void Roll();
};
