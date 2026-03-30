// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_Roll.h"

#include "GAS_StudyCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_Roll::UGA_Roll()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    
    DodgePlayRate = 1.8f;
}

void UGA_Roll::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle, 
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, 
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    
    Character = Cast<AGAS_StudyCharacter>(ActorInfo->AvatarActor.Get());

    if (Character.IsValid())
    {
        // 초기 코스트 지불
        if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
        {
            EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
            return;
        }

        Roll();
    }
    else
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
    }
}

void UGA_Roll::OnMontageCompleted()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Roll::Roll()
{
    if (DodgeMontage && Character.IsValid())
    {
        FVector InputVector = Character->GetCharacterMovement()->GetLastInputVector();
        
        if (!InputVector.IsNearlyZero())
        {   
            FRotator TargetRotation = InputVector.Rotation();
            
            TargetRotation.Pitch = 0.f;
            TargetRotation.Roll = 0.f;
            
            Character->SetActorRotation(TargetRotation);
        }
        
        // 1. 몽타주 재생 태스크
        UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
            this,
            NAME_None,
            DodgeMontage,
            DodgePlayRate
        );

        // 몽타주가 정상 종료되거나, 끊기거나, 취소되었을 때 모두 OnMontageCompleted 함수를 실행하도록 바인딩합니다.
        MontageTask->OnBlendOut.AddDynamic(this, &UGA_Roll::OnMontageCompleted);
        MontageTask->OnCompleted.AddDynamic(this, &UGA_Roll::OnMontageCompleted);
        MontageTask->OnInterrupted.AddDynamic(this, &UGA_Roll::OnMontageCompleted);
        MontageTask->OnCancelled.AddDynamic(this, &UGA_Roll::OnMontageCompleted);

        // 태스크 실행!
        MontageTask->ReadyForActivation();
    }
    else
    {
        // 몽타주가 없다면 바로 종료합니다.
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
    }
}
