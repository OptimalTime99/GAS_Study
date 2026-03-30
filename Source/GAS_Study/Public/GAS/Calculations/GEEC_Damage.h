// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEEC_Damage.generated.h"

UCLASS()
class GAS_STUDY_API UGEEC_Damage : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()

public:
    UGEEC_Damage();

    virtual void Execute_Implementation(
        const FGameplayEffectCustomExecutionParameters& ExecutionParams,
        OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
