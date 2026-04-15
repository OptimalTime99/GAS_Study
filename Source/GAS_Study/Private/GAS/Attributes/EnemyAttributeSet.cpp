// EnemyAttributeSet.cpp
#include "GAS/Attributes/EnemyAttributeSet.h"

#include"Net/UnrealNetwork.h"

UEnemyAttributeSet::UEnemyAttributeSet()
{
    // 기본값 (GE_EnemyDefaultStats로 덮어쓸 수 있음)
    InitHealth(100.0f);
    InitMaxHealth(100.0f);
    InitDefense(5.0f);
    InitAttackDamage(10.0f);
}

void UEnemyAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UEnemyAttributeSet, Health);
    DOREPLIFETIME(UEnemyAttributeSet, MaxHealth);
    DOREPLIFETIME(UEnemyAttributeSet, Defense);
    DOREPLIFETIME(UEnemyAttributeSet, AttackDamage);
}

void UEnemyAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
    }
    else if (Attribute == GetMaxHealthAttribute())
    {
        NewValue = FMath::Max(NewValue, 1.0f);
    }
}

void UEnemyAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UEnemyAttributeSet, Health, OldHealth);
}

void UEnemyAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UEnemyAttributeSet, MaxHealth, OldMaxHealth);
}

void UEnemyAttributeSet::OnRep_Defense(const FGameplayAttributeData& OldDefense)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UEnemyAttributeSet, Defense, OldDefense);
}

void UEnemyAttributeSet::OnRep_AttackDamage(const FGameplayAttributeData& OldAttackDamage)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UEnemyAttributeSet, AttackDamage, OldAttackDamage);
}