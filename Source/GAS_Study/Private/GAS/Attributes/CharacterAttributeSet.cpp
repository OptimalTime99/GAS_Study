// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/Attributes/CharacterAttributeSet.h"
#include "GameplayEffectExtension.h" // Data 구조체 인식

UCharacterAttributeSet::UCharacterAttributeSet()
{
    InitHealth(50.0f);
    InitMaxHealth(100.0f);
    InitDefense(0.0f);
    InitMaxDefense(100.0f);
    InitStamina(100.0f);
    InitMaxStamina(100.0f);
}

void UCharacterAttributeSet::PreAttributeChange(
    const FGameplayAttribute& Attribute, float& NewValue)
{
    // 잊지 말고 부모 구현을 먼저 호출합니다.
    Super::PreAttributeChange(Attribute, NewValue);

    // 1. Health(체력)의 현재 값이 변하려고 할 때
    if (Attribute == GetHealthAttribute())
    {
        // 체력이 0보다 작아지거나, 최대 체력(MaxHealth)을 초과하지 못하도록 제한합니다.
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
    }
    // 2. MaxHealth(최대 체력) 자체가 변하려고 할 때
    else if (Attribute == GetMaxHealthAttribute())
    {
        // 최대 체력 값 자체가 음수가 되지 않도록 최소 0으로 방어해 줍니다.
        NewValue = FMath::Max(NewValue, 0.0f);
    }

    if (Attribute == GetDefenseAttribute())
    {
        NewValue = FMath::Clamp(NewValue, -30.0f, GetMaxDefense());
    }
    
    if (Attribute == GetStaminaAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
    }
    
    
}

void UCharacterAttributeSet::PostGameplayEffectExecute(
    const FGameplayEffectModCallbackData& Data)
{
    // 잊지 말고 부모 구현을 호출합니다.
    Super::PostGameplayEffectExecute(Data);

    // 게임플레이 이펙트가 Health 베이스 값을 변경한 직후
    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        // 영구적인 변화를 일으키는 베이스 값이 0 미만으로 떨어져 회복 불가 버그가 생기거나, 
        // MaxHealth를 초과하지 않도록 여기서 확실하게 최종 값을 제한(Clamp)하여 덮어씌웁니다.
        SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));

        /* * 이곳에 추가적인 게임 로직을 작성하기 좋습니다.
         * 예: if (GetHealth() <= 0.0f) { 캐릭터 사망 처리 함수 호출 }
         */
    }

    if (Data.EvaluatedData.Attribute == GetDefenseAttribute())
    {
        SetDefense(FMath::Clamp(GetDefense(), -30.0f, GetMaxDefense()));
    }
    
    if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
    {
        SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetMaxStamina()));
    }
}
