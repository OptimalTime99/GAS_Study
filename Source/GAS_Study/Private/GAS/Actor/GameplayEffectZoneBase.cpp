// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Actor/GameplayEffectZoneBase.h"
#include "AbilitySystemComponent.h"
#include "Components/BoxComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

AGameplayEffectZoneBase::AGameplayEffectZoneBase()
{
    PrimaryActorTick.bCanEverTick = false;

    // 박스 컴포넌트 생성 및 루트로 설정
    ZoneBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneBox"));
    RootComponent = ZoneBox;

    // 충돌 설정 (기본적으로 모두와 겹치도록 설정, 필요에 따라 Profile 변경)
    ZoneBox->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    ZoneBox->SetGenerateOverlapEvents(true);
}

void AGameplayEffectZoneBase::BeginPlay()
{
    Super::BeginPlay();

    // Overlap 이벤트 바인딩
    if (ZoneBox)
    {
        ZoneBox->OnComponentBeginOverlap.AddDynamic(this, &AGameplayEffectZoneBase::OnOverlapBegin);
        ZoneBox->OnComponentEndOverlap.AddDynamic(this, &AGameplayEffectZoneBase::OnOverlapEnd);
    }
}

void AGameplayEffectZoneBase::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    // 유효성 검사
    if (!OtherActor || OtherActor == this || !GameplayEffectClass) return;

    // 대상 액터에서 Ability System Component 추출
    UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);

    if (!TargetASC) return;

    // Effect Context와 Spec 생성
    FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
    ContextHandle.AddInstigator(this, this);

    FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1.0f, ContextHandle);
    if (!SpecHandle.IsValid()) return;

    // 이펙트 적용
    FActiveGameplayEffectHandle ActiveHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    if (!ActiveHandle.WasSuccessfullyApplied()) return;

    // 적용할 GameplayEffect의 기본 객체(CDO)를 가져옵니다.
    const UGameplayEffect* EffectCDO = GameplayEffectClass->GetDefaultObject<UGameplayEffect>();

    // 이펙트의 지속 정책이 'Infinite(무한)'일 때만 이탈 시 지우기 위해 맵에 저장합니다.
    // Instant나 HasDuration(Persistent)은 맵에 저장되지 않으므로 OnOverlapEnd에서 무시됩니다.
    if (EffectCDO && EffectCDO->DurationPolicy == EGameplayEffectDurationType::Infinite)
    {
        ActiveEffectHandles.Add(OtherActor, ActiveHandle);
    }
}

void AGameplayEffectZoneBase::OnOverlapEnd(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    if (!OtherActor || OtherActor == this) return;

    // 해당 액터가 핸들 맵에 있는지 확인
    if (FActiveGameplayEffectHandle* HandlePtr = ActiveEffectHandles.Find(OtherActor))
    {
        UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
        if (TargetASC)
        {
            // 타겟의 ASC에서 활성화된 이펙트 제거
            TargetASC->RemoveActiveGameplayEffect(*HandlePtr);
        }

        // 메모리 누수 방지를 위해 맵에서 대상 제거
        ActiveEffectHandles.Remove(OtherActor);
    }
}
