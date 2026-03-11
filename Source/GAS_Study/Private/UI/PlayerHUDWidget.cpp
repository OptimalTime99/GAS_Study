// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerHUDWidget.h"

#include "Components/ProgressBar.h"
#include "GAS/Attributes/CharacterAttributeSet.h"

void UPlayerHUDWidget::InitWidget(UAbilitySystemComponent* InASC)
{
    if (!InASC) return;
    // 핵심: 이미 연결된 예전 ASC가 있다면, 예전 캐릭터와의 델리게이트 연결을 먼저 끊어줍니다.
    if (ASC)
    {
        ASC->GetGameplayAttributeValueChangeDelegate(
               UCharacterAttributeSet::GetHealthAttribute())
           .Remove(HealthChangedDelegateHandle);
        ASC->GetGameplayAttributeValueChangeDelegate(
               UCharacterAttributeSet::GetMaxHealthAttribute())
           .Remove(MaxHealthChangedDelegateHandle);
        
        ASC->GetGameplayAttributeValueChangeDelegate(
               UCharacterAttributeSet::GetStaminaAttribute())
           .Remove(StaminaChangedDelegateHandle);
        
        ASC->GetGameplayAttributeValueChangeDelegate(
               UCharacterAttributeSet::GetMaxStaminaAttribute())
           .Remove(MaxStaminaChangedDelegateHandle);
    }

    ASC = InASC;

    // 1. 초기값 세팅
    bool bFound;
    CurrentHealth = ASC->GetGameplayAttributeValue(
        UCharacterAttributeSet::GetHealthAttribute(), bFound);
    MaxHealth = ASC->GetGameplayAttributeValue(
        UCharacterAttributeSet::GetMaxHealthAttribute(), bFound);
    
    CurrentStamina = ASC->GetGameplayAttributeValue(
        UCharacterAttributeSet::GetStaminaAttribute(), bFound);
    MaxStamina = ASC->GetGameplayAttributeValue(
      UCharacterAttributeSet::GetMaxStaminaAttribute(), bFound);

    if (HP_Bar && MaxHealth > 0.f)
    {
        HP_Bar->SetPercent(CurrentHealth / MaxHealth);
    }
    
    if (Stamina_Bar && MaxStamina > 0.f)
    {
        Stamina_Bar->SetPercent(CurrentStamina / MaxStamina);
    }

    // 2. 델리게이트 바인딩
    HealthChangedDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(
                                         UCharacterAttributeSet::GetHealthAttribute())
                                     .AddUObject(this, &UPlayerHUDWidget::OnHealthChanged);
    MaxHealthChangedDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(
                                            UCharacterAttributeSet::GetMaxHealthAttribute())
                                        .AddUObject(this, &UPlayerHUDWidget::OnMaxHealthChanged);
    
    StaminaChangedDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(
                                         UCharacterAttributeSet::GetStaminaAttribute())
                                     .AddUObject(this, &UPlayerHUDWidget::OnStaminaChanged);
    MaxStaminaChangedDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(
                                            UCharacterAttributeSet::GetMaxStaminaAttribute())
                                        .AddUObject(this, &UPlayerHUDWidget::OnMaxStaminaChanged);
}

// 체력이 변했을 때
void UPlayerHUDWidget::OnHealthChanged(const FOnAttributeChangeData& Data)
{
    CurrentHealth = Data.NewValue; // 현재 체력 갱신

    if (HP_Bar && MaxHealth > 0.f)
    {
        float Percent = FMath::Clamp(CurrentHealth / MaxHealth, 0.f, 1.0f);
        HP_Bar->SetPercent(Percent);
    }
}

// 최대 체력이 변했을 때
void UPlayerHUDWidget::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
    MaxHealth = Data.NewValue; // 최대 체력 갱신

    if (HP_Bar && MaxHealth > 0.f)
    {
        // 최대 체력이 변해도 비율이 달라지므로 프로그레스 바를 다시 그려줍니다.
        float Percent = FMath::Clamp(CurrentHealth / MaxHealth, 0.f, 1.0f);
        HP_Bar->SetPercent(Percent);
    }
}

// 스태미나가 변했을 때
void UPlayerHUDWidget::OnStaminaChanged(const FOnAttributeChangeData& Data)
{
    CurrentStamina = Data.NewValue;
    
    if (Stamina_Bar && MaxStamina > 0.f)
    {
        float Percent = FMath::Clamp(CurrentStamina / MaxStamina, 0.f, 1.0f);
        Stamina_Bar->SetPercent(Percent);
    }
}

// 최대 스태미나가 변했을 때
void UPlayerHUDWidget::OnMaxStaminaChanged(const FOnAttributeChangeData& Data)
{
    MaxStamina = Data.NewValue;
    
    if (Stamina_Bar && MaxStamina > 0.f)
    {
        float Percent = FMath::Clamp(CurrentStamina / MaxStamina, 0.f, 1.0f);
        Stamina_Bar->SetPercent(Percent);
    }   
}
