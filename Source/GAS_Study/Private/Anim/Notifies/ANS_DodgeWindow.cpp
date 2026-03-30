// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/Notifies/ANS_DodgeWindow.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

void UANS_DodgeWindow::NotifyBegin(
    USkeletalMeshComponent* MeshComp, 
    UAnimSequenceBase* Animation, 
    float TotalDuration,
    const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
    
        if (UAbilitySystemComponent* ASC = GetASC(MeshComp))
    {
        if (DodgeWindowTag.IsValid())
        {
            ASC->AddLooseGameplayTag(DodgeWindowTag);
            UE_LOG(LogTemp, Log, TEXT("[ANS_DodgeWindow] Tag Added: %s"), *DodgeWindowTag.ToString());
        }
    }
}

void UANS_DodgeWindow::NotifyEnd(
    USkeletalMeshComponent* MeshComp, 
    UAnimSequenceBase* Animation,
    const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyEnd(MeshComp, Animation, EventReference);
    
        if (UAbilitySystemComponent* ASC = GetASC(MeshComp))
    {
        if (DodgeWindowTag.IsValid())
        {
            ASC->RemoveLooseGameplayTag(DodgeWindowTag);
            UE_LOG(LogTemp, Log, TEXT("[ANS_DodgeWindow] Tag Removed: %s"), *DodgeWindowTag.ToString());
        }
    }
}

UAbilitySystemComponent* UANS_DodgeWindow::GetASC(USkeletalMeshComponent* MeshComp) const
{
    if (!MeshComp) return nullptr;

    AActor* Owner = MeshComp->GetOwner();
    if (!Owner) return nullptr;

    if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Owner))
    {
        return ASI->GetAbilitySystemComponent();
    }

    return nullptr;
}
