// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h" // ATTRIBUTE_ACCESSORS
#include "AttributeSet.h"
#include "CharacterAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName,PropertyName)\
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName,PropertyName)\
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)\
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)\
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class GAS_STUDY_API UCharacterAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    UCharacterAttributeSet();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes")
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, Health)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes")
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, MaxHealth)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes")
    FGameplayAttributeData Defense;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, Defense)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes")
    FGameplayAttributeData MaxDefense;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, MaxDefense)
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes")
    FGameplayAttributeData Stamina;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, Stamina)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attributes")
    FGameplayAttributeData MaxStamina;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, MaxStamina)

public:
    // 어트리뷰트의 '현재 값'이 변경되기 직전에 호출되는 함수 (필터링 역할)
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

    // 게임플레이 이펙트가 '베이스 값'을 변경한 직후에 호출되는 함수 (로직 확정 역할)
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
