#include "GAS/Abilities/GA_AttackBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS_StudyCharacter.h"
#include "GAS/GAS_StudyTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
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

    if (!Character.IsValid())
    {
        return;
    }
    AActor* Avatar = Character.Get();
    USkeletalMeshComponent* Mesh = Character->GetMesh(); // 캐시해두면 더 좋습니다.

    // 정면 기준이 아닌 Socket 위치 기준 트레이스
    FVector SocketLocation = Mesh->GetSocketLocation(TraceSocketName);
    FVector Start = SocketLocation;
    FVector End = SocketLocation + (Avatar->GetActorForwardVector() * 5.0f); // Sweep을 위한 미세한 이동 오프셋

    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    TArray<FHitResult> HitResults;
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(Avatar);

    EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;

    bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
        Character.Get(), Start, End, TraceRadius, ObjectTypes, false, ActorsToIgnore, DebugType, HitResults, true,
        FLinearColor::Red, FLinearColor::Green, 2.0f);

    // 2. 수동 디버그 드로우 추가: 강의 자료 방식 
    if (bShowDebug)
    {
        // Hit 여부에 따라 색상 결정 (맞으면 빨강, 안 맞으면 초록) 
        FColor DebugColor = bHit ? FColor::Red : FColor::Green;

        // 직접 구체 그리기 (2.0f는 2초 동안 화면에 남기겠다는 뜻입니다)
        DrawDebugSphere(GetWorld(), Start, TraceRadius, 12, DebugColor, false, 2.0f);
    }

    if (bHit && DamageEffectClass)
    {
        for (const FHitResult& Hit : HitResults)
        {
            AActor* HitActor = Hit.GetActor();
            
            // TSet에 없는 녀석일 때만 통과 (즉, 이번 공격에서 처음 맞은 녀석)
            if (HitActor && !HitActors.Contains(HitActor)) 
            {
                HitActors.Add(HitActor); // 명단에 추가!

                UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
                if (TargetASC)
                {
                    FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
                    if (SpecHandle.IsValid())
                    {
                        // 실제 데미지 적용
                        TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
                        
                        // 1. 출력 로그(Output Log)에 띄우기
                        UE_LOG(LogTemp, Warning, TEXT("[%s]에게 데미지 적용 완료!"), *HitActor->GetName());

                        // 2. 게임 화면 좌측 상단에 띄우기 (플레이 중 바로 확인 가능)
                        if (GEngine)
                        {
                            FString DebugMsg = FString::Printf(TEXT("[%s] Hit!"), *HitActor->GetName());
                            // 2초 동안 파란색으로 화면에 띄웁니다
                            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, DebugMsg); 
                        }
                    }
                }
            }
        }
    }
}
