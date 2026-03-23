#include "GAS/Abilities/GA_ComboAttackBase.h"
#include"AbilitySystemComponent.h"
#include"Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Components/ComboManagerComponent.h"
#include "GAS/GAS_StudyTags.h"
#include "GAS/Attributes/CharacterAttributeSet.h"
#include "Subsystem/CombatManagerSubsystem.h"


UGA_ComboAttackBase::UGA_ComboAttackBase()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    // 공격 중 태그
    ActivationOwnedTags.AddTag(
        FGameplayTag::RequestGameplayTag(FName("State.Attacking")));
}

void UGA_ComboAttackBase::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }
    
    const FComboData* MyComboData = FindMyComboData(ActorInfo);
    
    if (MyComboData && MyComboData->Stamina > 0.0f && StaminaCostEffectClass)
    {
        FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(StaminaCostEffectClass, 1.0f);
        if (SpecHandle.IsValid())
        {
            SpecHandle.Data->SetSetByCallerMagnitude(
                GAS_StudyTags::Data_Stamina_Cost,
                -MyComboData->Stamina);
            
            ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle);
        }
        
    }

    if (!MontageToPlay)
    {
        UE_LOG(LogTemp, Error, TEXT("[ComboAttack] No Montage!"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // Montage 재생 (Section 지정)
    UAbilityTask_PlayMontageAndWait* Task =
        UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
            this, NAME_None, MontageToPlay, 1.0f, SectionName);

    Task->OnCompleted.AddDynamic(this, &UGA_ComboAttackBase::OnMontageEnded);
    Task->OnBlendOut.AddDynamic(this, &UGA_ComboAttackBase::OnMontageEnded);
    Task->OnInterrupted.AddDynamic(this, &UGA_ComboAttackBase::OnMontageEnded);
    Task->OnCancelled.AddDynamic(this, &UGA_ComboAttackBase::OnMontageEnded);
    Task->ReadyForActivation();

    UE_LOG(LogTemp, Log, TEXT("[ComboAttack] Section:%s"), *SectionName.ToString());
    
    if (UCombatManagerSubsystem* CombatManagerSubsystem = GetWorld()->GetSubsystem<UCombatManagerSubsystem>())
{
	CombatManagerSubsystem->StartCombat();
}
}

bool UGA_ComboAttackBase::CanActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayTagContainer* SourceTags,
    const FGameplayTagContainer* TargetTags, 
    FGameplayTagContainer* OptionalRelevantTags) const
{
    if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) return false;
    
    
    const FComboData* MyComboData = FindMyComboData(ActorInfo);
    if (!MyComboData) return false;
    
    UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
    if (!ASC) return false;
    
    const UCharacterAttributeSet* AttributeSet = ASC->GetSet<UCharacterAttributeSet>();
    if (!AttributeSet) return false;
    
    float CurrentStamina = AttributeSet->GetStamina();
    float RequiredStamina = MyComboData->Stamina;
    
    if (CurrentStamina < RequiredStamina) return false;
    
    return true;
}

const FComboData* UGA_ComboAttackBase::FindMyComboData(const FGameplayAbilityActorInfo* ActorInfo) const
{
    AActor* AvatarActor = ActorInfo->AvatarActor.Get();
    if (!AvatarActor) return nullptr;
    
    UComboManagerComponent* ComboManager = AvatarActor->FindComponentByClass<UComboManagerComponent>();
    if (!ComboManager) return nullptr;
    
    for (const FComboData& AttackData : ComboManager->AttackList)
    {
        if (AbilityTags.HasTag(AttackData.AbilityTag))
        {
            return &AttackData;
        }
    }
    
    return nullptr;
}

void UGA_ComboAttackBase::OnMontageEnded()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
