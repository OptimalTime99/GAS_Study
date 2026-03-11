// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Notifies/AN_SendGameplayEvent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UAN_SendGameplayEvent::Notify(
    USkeletalMeshComponent* MeshComp,
    UAnimSequenceBase* Animation,
    const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if (!MeshComp) return;
    AActor* Owner = MeshComp->GetOwner();
    if (!Owner) return;
    
    if (!EventTag.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("EventTag가 비어 있습니다."));
        return;
    }

    FGameplayEventData Payload;
    Payload.Instigator = Owner;
    
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, EventTag, Payload);
}
