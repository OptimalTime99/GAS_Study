// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Calculations/GEEC_Damage.h"

#include "GAS/Attributes/CharacterAttributeSet.h"

UGEEC_Damage::UGEEC_Damage()
{
}

void UGEEC_Damage::Execute_Implementation(
    const FGameplayEffectCustomExecutionParameters& ExecutionParams,
    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);
    
    // 1. 공격자와 피격자의 ASC 가져오기
    UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
    UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();

    if (!TargetASC || !SourceASC) return;

    // 2. 피격자의 태그 가져오기
    FGameplayTagContainer TargetTags;
    TargetASC->GetOwnedGameplayTags(TargetTags);

    // 3. [핵심] 무적(I-Frame) 상태인지 검사
    FGameplayTag InvincibleTag = FGameplayTag::RequestGameplayTag(FName("State.Dodging")); // ANS_DodgeWindow에서 부여한 태그 이름 사용
    
    if (TargetTags.HasTagExact(InvincibleTag))
    {
        // 무적 태그가 있다면 연산을 즉시 종료하여 데미지를 0으로 만듭니다.
        UE_LOG(LogTemp, Log, TEXT("Target is Invincible! Damage Blocked."));
        return;
    }

    // 4. 가해지는 기본 데미지 값 가져오기 (SetByCaller로 전달받았다고 가정)
    // GameplayTag "Data.Damage"를 통해 데미지 수치를 받아옵니다.
    float BaseDamage = ExecutionParams.GetOwningSpec().GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), false, 0.0f);
    
    // (이 부분에서 나중에 공격력/방어력 곱연산, 크리티컬 계산 등을 추가하시면 됩니다)
    float FinalDamage = BaseDamage;

    // 5. 최종 데미지 적용
    // 계산된 FinalDamage만큼 타겟의 Health를 감소(-기호)시킵니다.
    if (FinalDamage > 0.f)
    {
        OutExecutionOutput.AddOutputModifier(
            FGameplayModifierEvaluatedData(UCharacterAttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, -FinalDamage)
        );
    }
}
