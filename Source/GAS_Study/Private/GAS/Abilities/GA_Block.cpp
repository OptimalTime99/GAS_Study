// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_Block.h"

#include "AbilitySystemComponent.h"
#include "GAS_StudyCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GAS/Attributes/CharacterAttributeSet.h"


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

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    Character = Cast<AGAS_StudyCharacter>(ActorInfo->AvatarActor.Get());

    if (Character.IsValid())
    {
        // 🌟 1. 입구 컷 (발동 조건 검사): 스태미나가 0 이하라면 아예 실행을 취소합니다!
        if (const UCharacterAttributeSet* AttrSet = Character->GetAbilitySystemComponent()->GetSet<
            UCharacterAttributeSet>())
        {
            if (AttrSet->GetStamina() <= 0.0f)
            {
                UE_LOG(LogTemp, Warning, TEXT("스태미나가 없어서 가드를 올릴 수 없습니다!"));
                EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
                return;
            }
        }

        // 🌟 2. 초기 코스트 지불 (가드 올릴 때 1회성 소모 GE가 있다면 여기서 처리됨)
        if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
        {
            EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
            return;
        }

        Defense();

        CommitAbility(Handle, ActorInfo, ActivationInfo);
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
    if (Character.IsValid())
    {
        UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();

        // 1. 방어력 버프 해제
        if (ActiveDefenseBuffHandle.IsValid())
        {
            ASC->RemoveActiveGameplayEffect(ActiveDefenseBuffHandle);
            ActiveDefenseBuffHandle.Invalidate();
        }

        // 🌟 2. 스태미나 지속 소모 디버프 강제 해제! (중첩 방지)
        if (ActiveStaminaDrainHandle.IsValid())
        {
            ASC->RemoveActiveGameplayEffect(ActiveStaminaDrainHandle);
            ActiveStaminaDrainHandle.Invalidate();
        }
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Block::Defense()
{
#if ENABLE_DRAW_DEBUG
    AActor* AvatarActor = GetAvatarActorFromActorInfo();
    if (AvatarActor)
    {
        FVector Location = AvatarActor->GetActorLocation();
        FVector Forward = AvatarActor->GetActorForwardVector();

        // 정면 방향 (파란색)
        DrawDebugLine(GetWorld(), Location,
                      Location + Forward * 200.f, FColor::Blue, false, 3.f, 0, 2.f);

        // 블록 가능 범위 (좌우 60도, 녹색)
        FVector Left = Forward.RotateAngleAxis(-60.f, FVector::UpVector);
        FVector Right = Forward.RotateAngleAxis(60.f, FVector::UpVector);
        DrawDebugLine(GetWorld(), Location,
                      Location + Left * 150.f, FColor::Green, false, 3.f, 0, 1.f);
        DrawDebugLine(GetWorld(), Location,
                      Location + Right * 150.f, FColor::Green, false, 3.f, 0, 1.f);
    }
#endif

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

    if (Character.IsValid())
    {
        UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
        FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
        EffectContext.AddSourceObject(this);

        // 1. 방어력 +10 버프 적용
        if (DefenseBuffEffect)
        {
            FGameplayEffectSpecHandle BuffSpec = ASC->MakeOutgoingSpec(DefenseBuffEffect, 1.0f, EffectContext);
            if (BuffSpec.IsValid())
            {
                ActiveDefenseBuffHandle = ASC->ApplyGameplayEffectSpecToSelf(*BuffSpec.Data.Get());
            }
        }

        // 🌟 2. 스태미나 지속 소모 디버프 적용 (Infinite)
        if (StaminaDrainEffect)
        {
            FGameplayEffectSpecHandle DrainSpec = ASC->MakeOutgoingSpec(StaminaDrainEffect, 1.0f, EffectContext);
            if (DrainSpec.IsValid())
            {
                // 여기서 영수증을 챙깁니다!
                ActiveStaminaDrainHandle = ASC->ApplyGameplayEffectSpecToSelf(*DrainSpec.Data.Get());
            }
        }
    }
}
