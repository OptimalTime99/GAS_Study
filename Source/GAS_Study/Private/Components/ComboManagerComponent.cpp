#include"Components/ComboManagerComponent.h"
#include"AbilitySystemComponent.h"
#include"AbilitySystemInterface.h"

UComboManagerComponent::UComboManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UComboManagerComponent::BeginPlay()
{
    Super::BeginPlay();

    if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner()))
    {
        CachedASC = ASI->GetAbilitySystemComponent();

        if (CachedASC)
        {
            // 콤보 윈도우 태그 이벤트 등록
            if (ComboWindowTag.IsValid())
            {
                ComboWindowTagHandle = CachedASC->RegisterGameplayTagEvent(
                    ComboWindowTag,
                    EGameplayTagEventType::NewOrRemoved
                ).AddUObject(this, &UComboManagerComponent::HandleComboWindowTagChanged);
            }

            // ★ 공격 중 태그 이벤트 등록 (State.Attacking)
            if (AttackingTag.IsValid())
            {
                AttackingTagHandle = CachedASC->RegisterGameplayTagEvent(
                    AttackingTag,
                    EGameplayTagEventType::NewOrRemoved
                ).AddUObject(this, &UComboManagerComponent::HandleAttackingTagChanged);
            }
        }
    }
}

void UComboManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (CachedASC)
    {
        if (ComboWindowTagHandle.IsValid())
        {
            CachedASC->UnregisterGameplayTagEvent(
                ComboWindowTagHandle, ComboWindowTag, EGameplayTagEventType::NewOrRemoved);
        }
        if (AttackingTagHandle.IsValid())
        {
            CachedASC->UnregisterGameplayTagEvent(
                AttackingTagHandle, AttackingTag, EGameplayTagEventType::NewOrRemoved);
        }
    }
    Super::EndPlay(EndPlayReason);
}

int32 UComboManagerComponent::GetMaxComboIndex() const
{
    int32 MaxIndex = -1;
    for (const FComboData& Attack : AttackList)
    {
        MaxIndex = FMath::Max(MaxIndex, Attack.ComboIndex);
    }
    return MaxIndex;
}

bool UComboManagerComponent::RequestAttack()
{
    const int32 MaxComboIndex = GetMaxComboIndex();
    // 마지막 콤보 공격 중이면 입력 무시
    if (CurrentComboIndex >= MaxComboIndex && bIsAttacking)
    {
        UE_LOG(LogTemp, Log, TEXT("[Combo] At max combo, ignoring input"));
        return false;
    }

    if (bIsAttacking)
    {
        // 1. 윈도우가 이미 열려있을 때 누르면 즉시 다음 콤보 실행
        if (bComboWindowOpen)
        {
            AdvanceCombo();
        }
        // 2. 윈도우가 아직 안 열렸을 때 누르면 입력 버퍼링 (예약)
        else if (!bPendingCombo)
        {
            bPendingCombo = true;
            UE_LOG(LogTemp, Log, TEXT("[Combo] Input buffered"));
        }
        return true;
    }

    // 공격 중이 아니면 1타 바로 실행
    return ExecuteCombo();
}

bool UComboManagerComponent::ExecuteCombo()
{
    FComboData* AttackData = GetAttackByIndex(CurrentComboIndex);
    if (!AttackData || !AttackData->AbilityTag.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[Combo] No attack data for index%d"), CurrentComboIndex);
        ResetCombo();
        return false;
    }

    bool bSuccess = ActivateAbilityByTag(AttackData->AbilityTag);
    if (bSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("[Combo] Executed index%d"), CurrentComboIndex);
        bPendingCombo = false; // 실행했으니 버퍼 클리어
        StartResetTimer();
    }

    return bSuccess;
}

void UComboManagerComponent::AdvanceCombo()
{
    const int32 MaxComboIndex = GetMaxComboIndex();

    if (CurrentComboIndex < MaxComboIndex)
    {
        CurrentComboIndex++;
        UE_LOG(LogTemp, Log, TEXT("[Combo] Advanced to index%d"), CurrentComboIndex);
        ExecuteCombo();
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("[Combo] Already at max combo"));
        bPendingCombo = false;
    }
}

void UComboManagerComponent::HandleComboWindowTagChanged(const FGameplayTag Tag, int32 NewCount)
{
    bComboWindowOpen = (NewCount > 0);
    UE_LOG(LogTemp, Log, TEXT("[Combo] Window%s"),
           bComboWindowOpen ? TEXT("OPEN") : TEXT("CLOSED"));

    // ★ 윈도우 열릴 때 버퍼링된 입력 있으면 다음 콤보로!
    if (bComboWindowOpen && bPendingCombo)
    {
        UE_LOG(LogTemp, Log, TEXT("[Combo] Processing buffered input"));
        bPendingCombo = false;
        AdvanceCombo();
    }
}

void UComboManagerComponent::HandleAttackingTagChanged(const FGameplayTag Tag, int32 NewCount)
{
    bool bWasAttacking = bIsAttacking;
    bIsAttacking = (NewCount > 0);

    UE_LOG(LogTemp, Log, TEXT("[Combo] Attacking:%s"),
           bIsAttacking ? TEXT("YES") : TEXT("NO"));

    // ★ 공격 종료 시 마지막 콤보였으면 리셋
    if (bWasAttacking && !bIsAttacking)
    {
        if (CurrentComboIndex >= GetMaxComboIndex())
        {
            UE_LOG(LogTemp, Log, TEXT("[Combo] Final combo ended, resetting"));
            ResetCombo();
        }
    }
}

FComboData* UComboManagerComponent::GetAttackByIndex(int32 Index)
{
    for (FComboData& Attack : AttackList)
    {
        if (Attack.ComboIndex == Index)
        {
            return &Attack;
        }
    }
    return nullptr;
}

bool UComboManagerComponent::ActivateAbilityByTag(const FGameplayTag& Tag)
{
    if (!CachedASC || !Tag.IsValid()) return false;

    FGameplayTagContainer Tags;
    Tags.AddTag(Tag);
    return CachedASC->TryActivateAbilitiesByTag(Tags);
}

void UComboManagerComponent::StartResetTimer()
{
    GetWorld()->GetTimerManager().ClearTimer(ComboResetTimerHandle);
    GetWorld()->GetTimerManager().SetTimer(
        ComboResetTimerHandle,
        this,
        &UComboManagerComponent::ResetCombo,
        ComboResetTime,
        false
    );
}

void UComboManagerComponent::ResetCombo()
{
    UE_LOG(LogTemp, Log, TEXT("[Combo] RESET"));
    CurrentComboIndex = 0;
    bComboWindowOpen = false;
    bPendingCombo = false;
    // bIsAttacking은 태그로 관리되므로 건드리지 않음
    GetWorld()->GetTimerManager().ClearTimer(ComboResetTimerHandle);
}
