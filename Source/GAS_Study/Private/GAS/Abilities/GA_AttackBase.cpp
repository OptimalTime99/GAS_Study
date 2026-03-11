// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_AttackBase.h"

#include "GAS_StudyCharacter.h"
#include "GAS/GAS_StudyTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UGA_AttackBase::UGA_AttackBase()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    FGameplayTag AttackTag = GAS_StudyTags::Ability_Action_Attack;
    AbilityTags.AddTag(AttackTag);
    ActivationOwnedTags.AddTag(AttackTag);
}

void UGA_AttackBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    Character = Cast<AGAS_StudyCharacter>(ActorInfo->AvatarActor.Get());

    if (Character.IsValid())
    {
        Attack();
    }
    else
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
    }
}

bool UGA_AttackBase::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo,
                                        const FGameplayTagContainer* SourceTags,
                                        const FGameplayTagContainer* TargetTags,
                                        FGameplayTagContainer* OptionalRelevantTags) const
{
    return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_AttackBase::Attack()
{
    if (AttackMontage)
    {
        // 🌟 수정: 캐릭터의 PlayAnimMontage 대신 GAS 전용 태스크를 생성합니다.
        UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
            this,
            NAME_None,
            AttackMontage
        );

        // 몽타주가 정상 종료되거나, 끊기거나, 취소되었을 때 모두 OnMontageCompleted 함수를 실행하도록 바인딩합니다.
        MontageTask->OnBlendOut.AddDynamic(this, &UGA_AttackBase::OnMontageCompleted);
        MontageTask->OnCompleted.AddDynamic(this, &UGA_AttackBase::OnMontageCompleted);
        MontageTask->OnInterrupted.AddDynamic(this, &UGA_AttackBase::OnMontageCompleted);
        MontageTask->OnCancelled.AddDynamic(this, &UGA_AttackBase::OnMontageCompleted);

        // 태스크 실행!
        MontageTask->ReadyForActivation();
    }
    else
    {
        // 몽타주가 없다면 바로 종료합니다.
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
    }
}

void UGA_AttackBase::OnMontageCompleted()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
