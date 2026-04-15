// EnemyAttributeSet.h
#pragma once

#include"CoreMinimal.h"
#include"AttributeSet.h"
#include"AbilitySystemComponent.h"
#include"EnemyAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName,PropertyName)\
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName,PropertyName)\
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)\
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)\
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * UEnemyAttributeSet
 *
 * 적 캐릭터 전용 AttributeSet입니다.
 * Player보다 단순한 구조 (Stamina 등 불필요).
 */
UCLASS()
class GAS_STUDY_API UEnemyAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    UEnemyAttributeSet();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

    // ============================================================
    // Attributes
    // ============================================================

    // 현재 체력
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, Health)

    // 최대 체력
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, MaxHealth)

    // 방어력
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Defense)
    FGameplayAttributeData Defense;
    ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, Defense)

    // 공격력
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_AttackDamage)
    FGameplayAttributeData AttackDamage;
    ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, AttackDamage)

protected:
    UFUNCTION()
    void OnRep_Health(const FGameplayAttributeData& OldHealth);

    UFUNCTION()
    void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

    UFUNCTION()
    void OnRep_Defense(const FGameplayAttributeData& OldDefense);

    UFUNCTION()
    void OnRep_AttackDamage(const FGameplayAttributeData& OldAttackDamage);
};