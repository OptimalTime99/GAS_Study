// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_ComboWindow.generated.h"

/**
 * 
 */
UCLASS()
class GAS_STUDY_API UANS_ComboWindow : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    // 추가할 GameplayTag (예: Combat.Window.Combo)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ComboWindow")
    FGameplayTag ComboWindowTag;

    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                             float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                           const FAnimNotifyEventReference& EventReference) override;

    virtual FString GetNotifyName_Implementation() const override;

private:
    class UAbilitySystemComponent* GetASC(USkeletalMeshComponent* MeshComp) const;
};
