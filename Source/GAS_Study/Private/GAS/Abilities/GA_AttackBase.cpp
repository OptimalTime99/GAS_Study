#include "GAS/Abilities/GA_AttackBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS_StudyCharacter.h"
#include "GAS/GAS_StudyTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Components/ComboManagerComponent.h"
#include "GAS/Attributes/EnemyAttributeSet.h"
#include "Kismet/KismetSystemLibrary.h"

UGA_AttackBase::UGA_AttackBase()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    ActivationOwnedTags.AddTag(GAS_StudyTags::Ability_Action_Attack);
}

void UGA_AttackBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    UE_LOG(LogTemp, Warning, TEXT("GA_Attack 시작 - 현재 추출된 데미지: %.2f"), CurrentDamage);

    Character = Cast<AGAS_StudyCharacter>(ActorInfo->AvatarActor.Get());
    if (UComboManagerComponent* ComboMgr = Character->FindComponentByClass<UComboManagerComponent>())
    {
        // 현재 실행 중인 콤보의 인덱스를 가져옴
        int32 Idx = ComboMgr->GetCurrentComboIndex();

        // AttackList에서 해당 인덱스의 데미지를 미리 변수에 저장
        for (const FComboData& Data : ComboMgr->AttackList)
        {
            if (Data.ComboIndex == Idx)
            {
                CurrentDamage = Data.Damage;
                break;
            }
        }
        UE_LOG(LogTemp, Warning, TEXT("[GA_Attack] Index: %d, Damage: %.2f 캐싱 완료"), Idx, CurrentDamage);
    }
    Attack();
}

void UGA_AttackBase::Attack()
{
    if (AttackMontage)
    {
        UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
            this,
            NAME_None,
            AttackMontage
        );

        MontageTask->OnBlendOut.AddDynamic(this, &UGA_AttackBase::OnMontageCompleted);
        MontageTask->OnCompleted.AddDynamic(this, &UGA_AttackBase::OnMontageCompleted);
        MontageTask->OnInterrupted.AddDynamic(this, &UGA_AttackBase::OnMontageCompleted);
        MontageTask->OnCancelled.AddDynamic(this, &UGA_AttackBase::OnMontageCompleted);

        MontageTask->ReadyForActivation();

        // WaitGameplayEvent 태스크 삭제 (NotifyState가 타이밍을 제어함)
    }
    else
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
    }
}

void UGA_AttackBase::OnMontageCompleted()
{
    EndTraceWindow(); // 만약 몽타주가 끊겼을 때를 대비해 타이머 종료
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_AttackBase::BeginTraceWindow()
{
    if (bIsTracing) return;
    bIsTracing = true;
    HitActors.Empty(); // 새로운 타격 윈도우 시작 시 초기화

    if (UWorld* World = GetWorld())
    {
        // TraceInterval 마다 PerformMeleeTrace 호출
        World->GetTimerManager().SetTimer(TraceTimerHandle, this, &UGA_AttackBase::PerformMeleeTrace, TraceInterval,
                                          true, 0.0f);
    }
}

void UGA_AttackBase::EndTraceWindow()
{
    if (!bIsTracing) return;
    bIsTracing = false;

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(TraceTimerHandle);
    }
}

void UGA_AttackBase::PerformMeleeTrace()
{
    Character = Cast<AGAS_StudyCharacter>(GetAvatarActorFromActorInfo());
    if (!Character.IsValid()) return;

    AActor* Avatar = Character.Get();
    USkeletalMeshComponent* Mesh = Character->GetMesh();

    FVector SocketLocation = Mesh->GetSocketLocation(TraceSocketName);
    FVector Start = SocketLocation;
    FVector End = SocketLocation + (Avatar->GetActorForwardVector() * 5.0f);

    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    TArray<FHitResult> HitResults;
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(Avatar);

    bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
        Character.Get(), Start, End, TraceRadius, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, HitResults,
        true,
        FLinearColor::Red, FLinearColor::Green, 2.0f);

    if (bHit && DamageEffectClass)
    {
        // [중요] 때리는 순간의 최신 데미지 수치를 다시 한번 가져옵니다.
        float FinalDamage = 0.f;
        if (UComboManagerComponent* ComboMgr = Character->FindComponentByClass<UComboManagerComponent>())
        {
            int32 CurrentIdx = ComboMgr->GetCurrentComboIndex();
            for (const FComboData& ComboData : ComboMgr->AttackList)
            {
                if (ComboData.ComboIndex == CurrentIdx)
                {
                    FinalDamage = ComboData.Damage;
                    break;
                }
            }
        }

        for (const FHitResult& Hit : HitResults)
        {
            AActor* HitActor = Hit.GetActor();
            if (HitActor && !HitActors.Contains(HitActor))
            {
                HitActors.Add(HitActor);
                UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
                
                if (TargetASC)
                {
                    FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
                    if (SpecHandle.IsValid())
                    {
                        // 1. 요청하신 Data.Damage.Cost 태그로 데미지 주입 (UE 5.6.1 명칭)
                        FGameplayTag DataTag = FGameplayTag::RequestGameplayTag(FName("Data.Damage.Cost"));
                        SpecHandle.Data.Get()->SetSetByCallerMagnitude(DataTag, FinalDamage);

                        // 2. 데미지 적용
                        TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

                        // 3. 체력 정보 가져오기 (UEnemyAttributeSet 사용)
                        float CurrentHP = TargetASC->GetNumericAttribute(UEnemyAttributeSet::GetHealthAttribute());
                        float MaxHP = TargetASC->GetNumericAttribute(UEnemyAttributeSet::GetMaxHealthAttribute());

                        // [로그] 데미지 전달 확인
                        UE_LOG(LogTemp, Log, TEXT("Index:%d | Damage:%.2f -> %s | HP:%.2f"), 
                               Character->FindComponentByClass<UComboManagerComponent>()->GetCurrentComboIndex(),
                               FinalDamage, *HitActor->GetName(), CurrentHP);

                        // 4. UI 업데이트 호출
                        Character->UpdateEnemyHPOnHUD(CurrentHP, MaxHP);
                    }
                }
            }
        }
    }
}
