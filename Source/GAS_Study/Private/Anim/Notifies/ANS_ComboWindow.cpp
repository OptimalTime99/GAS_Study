#include "Anim/Notifies/ANS_ComboWindow.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Components/SkeletalMeshComponent.h"

void UANS_ComboWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
    float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    if (UAbilitySystemComponent* ASC = GetASC(MeshComp))
    {
        if (ComboWindowTag.IsValid())
        {
            // ★ 핵심: LooseGameplayTag 추가 → RegisterGameplayTagEvent 트리거
            ASC->AddLooseGameplayTag(ComboWindowTag);
            UE_LOG(LogTemp, Log, TEXT("[AN_ComboWindow] Tag Added: %s"), *ComboWindowTag.ToString());
        }
    }
}

void UANS_ComboWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
    const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyEnd(MeshComp, Animation, EventReference);

    if (UAbilitySystemComponent* ASC = GetASC(MeshComp))
    {
        if (ComboWindowTag.IsValid())
        {
            // ★ 핵심: LooseGameplayTag 제거 → RegisterGameplayTagEvent 트리거
            ASC->RemoveLooseGameplayTag(ComboWindowTag);
            UE_LOG(LogTemp, Log, TEXT("[AN_ComboWindow] Tag Removed: %s"), *ComboWindowTag.ToString());
        }
    }
}

FString UANS_ComboWindow::GetNotifyName_Implementation() const
{
    if (ComboWindowTag.IsValid())
    {
        return FString::Printf(TEXT("ComboWindow: %s"), *ComboWindowTag.ToString());
    }
    return TEXT("ComboWindow");
}

UAbilitySystemComponent* UANS_ComboWindow::GetASC(USkeletalMeshComponent* MeshComp) const
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