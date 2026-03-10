// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/Attributes/CharacterAttributeSet.h"
#include "GameplayEffectExtension.h" // Data 구조체 인식

UCharacterAttributeSet::UCharacterAttributeSet()
{
    InitHealth(100.0f);
    InitMaxHealth(100.0f);
}

void UCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        // 이 게임플레이 이펙트는 Health를 변경합니다. 적용하되 우선 값을 제한합니다.
        // 이 경우 Health 베이스 값은 음수가 아니어야 합니다.
        SetHealth(FMath::Max(GetHealth(), 0.0f));
    }
}
