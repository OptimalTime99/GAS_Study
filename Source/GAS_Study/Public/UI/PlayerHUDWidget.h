// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h" // FOnAttributeChangeData 구조체
#include "PlayerHUDWidget.generated.h"

class UProgressBar;
class UAbilitySystemComponent;

UCLASS()
class GAS_STUDY_API UPlayerHUDWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    // 블루프린트의 HP_Bar 위젯과 C++ 코드를 연결하는 핵심 매크로입니다.
    // ⚠️ 주의: 변수명이 WBP_Player에 있는 프로그레스 바의 이름(HP_Bar)과 정확히 같아야 합니다.
    UPROPERTY(meta = (BindWidget))
    UProgressBar* HP_Bar;
    
    UPROPERTY(meta = (BindWidget))
    UProgressBar* Stamina_Bar;

    // 언제든 스탯을 다시 조회할 수 있도록 ASC를 캐싱해 둡니다.
    UPROPERTY()
    UAbilitySystemComponent* ASC;

public:
    // 캐릭터가 스폰되거나 UI가 생성될 때 한 번 호출하여 초기화해 주는 함수입니다.
    UFUNCTION(BlueprintCallable, Category = "UI")
    void InitWidget(UAbilitySystemComponent* InASC);

protected:
    float CurrentHealth = 0.f;
    float MaxHealth = 1.0f;
    
    float CurrentStamina = 0.f;
    float MaxStamina = 1.0f;

    // 체력 어트리뷰트가 변경될 때마다 자동으로 호출될 콜백 함수입니다.
    void OnHealthChanged(const FOnAttributeChangeData& Data);

    void OnMaxHealthChanged(const FOnAttributeChangeData& Data);
    
    void OnStaminaChanged(const FOnAttributeChangeData& Data);

    void OnMaxStaminaChanged(const FOnAttributeChangeData& Data);

    // 델리게이트 연결을 해제할 때 필요한 영수증(Handle)입니다.
    FDelegateHandle HealthChangedDelegateHandle;
    FDelegateHandle MaxHealthChangedDelegateHandle;
    
    FDelegateHandle StaminaChangedDelegateHandle;
    FDelegateHandle MaxStaminaChangedDelegateHandle;
};
