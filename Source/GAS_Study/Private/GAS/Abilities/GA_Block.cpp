// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_Block.h"

#include "AbilitySystemComponent.h"
#include "GAS_StudyCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"


UGA_Block::UGA_Block()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Block::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    Character = Cast<AGAS_StudyCharacter>(ActorInfo->AvatarActor.Get());

    if (Character.IsValid())
    {
        Defense();
    }
    else
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
    }
}

void UGA_Block::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                           const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                           bool bWasCancelled)
{
    // 저장해 두었던 버프 영수증이 유효하다면, 방어력 버프를 내 몸에서 지워버립니다.
    if (ActiveDefenseBuffHandle.IsValid() && Character.IsValid())
    {
        Character->GetAbilitySystemComponent()->RemoveActiveGameplayEffect(ActiveDefenseBuffHandle);
        
        ActiveDefenseBuffHandle.Invalidate();
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Block::Defense()
{
    if (DefenseMontage && Character.IsValid())
    {
        // 1. 몽타주 재생 태스크
        UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
            this,
            NAME_None,
            DefenseMontage
        );

        // 몽타주가 정상 종료되거나, 끊기거나, 취소되었을 때 모두 OnMontageCompleted 함수를 실행하도록 바인딩합니다.
        MontageTask->OnBlendOut.AddDynamic(this, &UGA_Block::OnMontageCompleted);
        MontageTask->OnCompleted.AddDynamic(this, &UGA_Block::OnMontageCompleted);
        MontageTask->OnInterrupted.AddDynamic(this, &UGA_Block::OnMontageCompleted);
        MontageTask->OnCancelled.AddDynamic(this, &UGA_Block::OnMontageCompleted);

        // 태스크 실행!
        MontageTask->ReadyForActivation();

        // 2. 추가: 팔이 다 올라갔다는 이벤트(노티파이)를 기다립니다.
        if (DefenseReadyEventTag.IsValid())
        {
            UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
                this, DefenseReadyEventTag, nullptr, false, false);

            WaitEventTask->EventReceived.AddDynamic(this, &UGA_Block::OnDefenseReady);
            WaitEventTask->ReadyForActivation();
        }
    }
    else
    {
        // 몽타주가 없다면 바로 종료합니다.
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
    }
}

void UGA_Block::OnMontageCompleted()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

// 애니메이션이 특정 프레임(방어 완료)에 도달해서 이벤트를 쏘면 여기서 비로소 버프를 줍니다!
void UGA_Block::OnDefenseReady(FGameplayEventData Payload)
{
    // 이미 버프가 있다면 종료
    if (ActiveDefenseBuffHandle.IsValid()) return;
    
    if (DefenseBuffEffect && Character.IsValid())
    {
        FGameplayEffectContextHandle EffectContext = Character->GetAbilitySystemComponent()->MakeEffectContext();
        EffectContext.AddSourceObject(this);

        FGameplayEffectSpecHandle SpecHandle = Character->GetAbilitySystemComponent()->MakeOutgoingSpec(
            DefenseBuffEffect, 1.0f, EffectContext);
        if (SpecHandle.IsValid())
        {
            // 여기서 버프(State.Blocking)가 캐릭터에게 적용됩니다.
            ActiveDefenseBuffHandle = Character->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(
                *SpecHandle.Data.Get());
            UE_LOG(LogTemp, Warning, TEXT("가드 자세 완료! 이제부터 데미지를 막습니다."));
        }
    }
}
