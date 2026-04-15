#include "GAS/Calculations/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "GAS/Attributes/CharacterAttributeSet.h"
#include "GAS/Attributes/EnemyAttributeSet.h"

// 캡처할 Attribute들을 모아두는 구조체
struct FDamageStatics
{
    // 플레이어용 방어력 캡처
    DECLARE_ATTRIBUTE_CAPTUREDEF(Defense);
    // 적용 방어력 캡처
    DECLARE_ATTRIBUTE_CAPTUREDEF(EnemyDefense);

    FDamageStatics()
    {
        // 캡처 정의: 어떤 클래스의 어떤 속성을, 누구(Target)에게서 가져올 것인지 정의
        DEFINE_ATTRIBUTE_CAPTUREDEF(UCharacterAttributeSet, Defense, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UEnemyAttributeSet, Defense, Target, false);
    }
};

static const FDamageStatics& DamageStatics()
{
    static FDamageStatics Statics;
    return Statics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
    // 실행 시점에 캡처할 Attribute들을 등록합니다.
    RelevantAttributesToCapture.Add(DamageStatics().DefenseDef);
    RelevantAttributesToCapture.Add(DamageStatics().EnemyDefenseDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                              FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
    if (!TargetASC) return;

    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    EvaluationParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    // 1. 기본 데미지 가져오기 (SetByCaller로 데미지를 넘긴다고 가정)
    // 데이터 태그 이름("Data.Damage")은 프로젝트 설정에 맞게 변경하세요.
    // 데미지 계산기 내부
    float BaseDamage = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage.Cost")),
                                                    false, 0.f);

    // 2. 방어력 가져오기 (대상에 따라 분기)
    float Defense = 0.0f;

    // 대상이 EnemyAttributeSet을 가지고 있다면 적의 방어력을 캡처
    if (TargetASC->HasAttributeSetForAttribute(DamageStatics().EnemyDefenseProperty))
    {
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().EnemyDefenseDef,
                                                                   EvaluationParameters, Defense);
    }
    // 대상이 CharacterAttributeSet을 가지고 있다면 플레이어의 방어력을 캡처
    else if (TargetASC->HasAttributeSetForAttribute(DamageStatics().DefenseProperty))
    {
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefenseDef, EvaluationParameters,
                                                                   Defense);
    }

    // 3. 최종 데미지 계산 (기본 데미지 - 방어력)
    float FinalDamage = FMath::Max<float>(BaseDamage - Defense, 0.0f);

    if (FinalDamage > 0.0f)
    {
        // 4. 알맞은 체력(Health) 속성에 데미지 적용
        if (TargetASC->HasAttributeSetForAttribute(UEnemyAttributeSet::GetHealthAttribute()))
        {
            OutExecutionOutput.AddOutputModifier(
                FGameplayModifierEvaluatedData(UEnemyAttributeSet::GetHealthAttribute(), EGameplayModOp::Additive,
                                               -FinalDamage));
        }
        else if (TargetASC->HasAttributeSetForAttribute(UCharacterAttributeSet::GetHealthAttribute()))
        {
            OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
                UCharacterAttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, -FinalDamage));
        }
    }
}
