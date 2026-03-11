// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_AttackBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS_StudyCharacter.h"
#include "GAS/GAS_StudyTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GAS/Attributes/CharacterAttributeSet.h"
#include "Kismet/KismetSystemLibrary.h"

UGA_AttackBase::UGA_AttackBase()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    FGameplayTag AttackTag = GAS_StudyTags::Ability_Action_Attack;
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

void UGA_AttackBase::Attack()
{
    if (AttackMontage)
    {
        // 🌟 1. 몽타주 재생 태스크
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

        if (!HitEventTag.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("[%s] HitEventTag가 세팅되지 않았습니다!"), *GetName());
        }

        // 2. 🌟 노티파이가 쏘는 Hit 이벤트를 기다리는 태스크
        UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
            this,
            HitEventTag, // 기다릴 태그
            nullptr, false, false);

        WaitEventTask->EventReceived.AddDynamic(this, &UGA_AttackBase::OnHitEventReceived);
        WaitEventTask->ReadyForActivation();
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

void UGA_AttackBase::OnHitEventReceived(FGameplayEventData Payload)
{
    // 노티파이에서 호출됨!
    if (!Character.IsValid()) return;
    AActor* Avatar = Character.Get();

    FVector Forward = Avatar->GetActorForwardVector();
    FVector Start = Avatar->GetActorLocation() + Forward * TraceStartDistance;
    FVector End = Avatar->GetActorLocation() + Forward * TraceEndDistance;

    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    TArray<FHitResult> HitResults;
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(Avatar);

    EDrawDebugTrace::Type DebugType = bShowDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

    bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
        Avatar,
        Start,
        End,
        TraceRadius,
        ObjectTypes,
        false,
        ActorsToIgnore,
        DebugType,
        HitResults,
        true,
        FLinearColor::Red,
        FLinearColor::Green,
        2.0f);

    if (bHit && DamageEffectClass)
    {
        TSet<AActor*> ProcessedActors;
        for (const FHitResult& Hit : HitResults)
        {
            AActor* HitActor = Hit.GetActor();
            if (HitActor && !ProcessedActors.Contains(HitActor))
            {
                ProcessedActors.Add(HitActor);

                // 어빌리티 내부이므로 ASC를 쉽게 가져와서 데미지를 적용할 수 있습니다.
                UAbilitySystemComponent* TargetASC =
                    UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
                if (TargetASC)
                {
                    FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(
                        DamageEffectClass, GetAbilityLevel());
                    if (SpecHandle.IsValid())
                    {
                        // 타겟에게 데미지 이펙트 적용
                        TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

                        // 타겟의 어트리뷰트 셋을 가져와 남은 체력 확인
                        if (const UCharacterAttributeSet* TargetAttributeSet = TargetASC->GetSet<
                            UCharacterAttributeSet>())
                        {
                            float TargetHealth = TargetAttributeSet->GetHealth();
                            UE_LOG(LogTemp, Warning, TEXT("타겟: %s / 남은 체력: %f"), *HitActor->GetName(), TargetHealth);
                        }
                    }
                }
            }
        }
    }
}
