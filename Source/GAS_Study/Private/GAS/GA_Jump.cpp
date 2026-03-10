// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA_Jump.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"

UGA_Jump::UGA_Jump()
{
    // 1. 인스턴싱 정책 설정: 단순 점프이므로 CDO를 재사용
    InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;

    // 2. 어빌리티 태그 설정 (언리얼 에디터의 프로젝트 세팅 > Gameplay Tags에 미리 'Ability.Action.Jump'가 추가되어 있어야 합니다)
    FGameplayTag JumpTag = FGameplayTag::RequestGameplayTag(FName("Ability.Action.Jump"));
    AbilityTags.AddTag(JumpTag);
}

void UGA_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                               const FGameplayAbilityActivationInfo ActivationInfo,
                               const FGameplayEventData* TriggerEventData)
{
    // 부모의 ActivateAbility 호출 (필수)
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    // 액터 정보에서 캐릭터를 가져옵니다.
    ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
    if (Character)
    {
        // 실제 언리얼 캐릭터의 점프 함수 실행
        Character->Jump();
    }

    // Non-Instanced 정책이므로 비동기 태스크 없이 즉시 종료 처리합니다.
    bool bReplicateEndAbility = true;
    bool bWasCancelled = false;
    EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UGA_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
                                  FGameplayTagContainer* OptionalRelevantTags) const
{
    return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}
