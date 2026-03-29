#include "Anim/Notifies/ANS_MeleeTrace.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/Abilities/GA_AttackBase.h"

void UANS_MeleeTrace::NotifyBegin(
    USkeletalMeshComponent* MeshComp,
    UAnimSequenceBase* Animation,
    float TotalDuration,
    const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    if (!MeshComp || !MeshComp->GetOwner()) return;

    UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner());
    if (!ASC) return;

    for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
    {
        if (Spec.IsActive())
        {
            if (UGA_AttackBase* AttackAbility = Cast<UGA_AttackBase>(Spec.GetPrimaryInstance()))
            {
                AttackAbility->BeginTraceWindow();
                break;
            }
        }
    }
}

void UANS_MeleeTrace::NotifyEnd(
    USkeletalMeshComponent* MeshComp,
    UAnimSequenceBase* Animation,
    const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyEnd(MeshComp, Animation, EventReference);

    if (!MeshComp || !MeshComp->GetOwner()) return;

    UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner());
    if (!ASC) return;

    for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
    {
        if (Spec.IsActive())
        {
            if (UGA_AttackBase* AttackAbility = Cast<UGA_AttackBase>(Spec.GetPrimaryInstance()))
            {
                AttackAbility->EndTraceWindow();
                break;
            }
        }
    }
}
