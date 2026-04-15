// EnemyCharacterBase.cpp
#include "Character/EnemyCharacterBase.h"

#include"AbilitySystemComponent.h"
#include"Abilities/GameplayAbility.h"
#include"GameplayEffect.h"
#include"GameFramework/CharacterMovementComponent.h"
#include"Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GAS/Attributes/EnemyAttributeSet.h"
#include "UI/EnemyHPBar.h"

AEnemyCharacterBase::AEnemyCharacterBase()
{
    // ASC 생성 - Character가 직접 소유
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(
        TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    // AttributeSet 생성
    AttributeSet = CreateDefaultSubobject<UEnemyAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AEnemyCharacterBase::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void AEnemyCharacterBase::BeginPlay()
{
    Super::BeginPlay();

    // Enemy는 BeginPlay에서 초기화
    InitializeAbilitySystem();

    // 서버에서만 Ability/Effect 부여
    if (HasAuthority())
    {
        GiveDefaultAbilities();
        ApplyDefaultEffects();
    }

    // Health 변경 콜백 등록
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
                                  UEnemyAttributeSet::GetHealthAttribute())
                              .AddUObject(this, &AEnemyCharacterBase::OnHealthChanged);
    }
}

void AEnemyCharacterBase::InitializeAbilitySystem()
{
    if (AbilitySystemComponent)
    {
        // ⚠️ Enemy: Owner와 Avatar 모두 자기 자신
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }
}

void AEnemyCharacterBase::GiveDefaultAbilities()
{
    if (!AbilitySystemComponent) { return; }

    for (const TSubclassOf<UGameplayAbility>& AbilityClass : DefaultAbilities)
    {
        if (AbilityClass)
        {
            FGameplayAbilitySpec Spec(AbilityClass, 1, INDEX_NONE, this);
            AbilitySystemComponent->GiveAbility(Spec);
        }
    }
}

void AEnemyCharacterBase::ApplyDefaultEffects()
{
    if (!AbilitySystemComponent) { return; }

    for (const TSubclassOf<UGameplayEffect>& EffectClass : DefaultEffects)
    {
        if (EffectClass)
        {
            FGameplayEffectContextHandle ContextHandle =
                AbilitySystemComponent->MakeEffectContext();
            ContextHandle.AddSourceObject(this);

            FGameplayEffectSpecHandle SpecHandle =
                AbilitySystemComponent->MakeOutgoingSpec(EffectClass, 1, ContextHandle);
            if (SpecHandle.IsValid())
            {
                AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(
                    *SpecHandle.Data.Get());
            }
        }
    }
}

// ============================================================
// Attribute 접근자
// ============================================================

float AEnemyCharacterBase::GetHealth() const
{
    return AttributeSet ? AttributeSet->GetHealth() : 0.0f;
}

float AEnemyCharacterBase::GetMaxHealth() const
{
    return AttributeSet ? AttributeSet->GetMaxHealth() : 0.0f;
}

// ============================================================
// 사망 처리
// ============================================================

void AEnemyCharacterBase::OnHealthChanged(const FOnAttributeChangeData& Data)
{

    if (Data.NewValue <= 0.0f && !bIsDead)
    {
        HandleDeath();
    }
}

void AEnemyCharacterBase::HandleDeath()
{
    if (bIsDead) { return; }
    bIsDead = true;

    UE_LOG(LogTemp, Log, TEXT("[%s] HandleDeath - Ragdoll 활성화"), *GetName());

    // 1. 모든 Ability 취소
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->CancelAllAbilities();
    }

    // 2. 이동 비활성화
    if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
    {
        MovementComp->StopMovementImmediately();
        MovementComp->DisableMovement();
    }

    // 3. Capsule 충돌 해제
    if (UCapsuleComponent* Capsule = GetCapsuleComponent())
    {
        Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    // 4. Ragdoll 활성화
    if (USkeletalMeshComponent* MeshComp = GetMesh())
    {
        MeshComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
        MeshComp->SetSimulatePhysics(true);
    }

    // 5. Timer 후 제거
    SetLifeSpan(DeathLifeSpan);
}