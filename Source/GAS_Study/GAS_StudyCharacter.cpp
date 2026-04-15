// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAS_StudyCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "AbilitySystemComponent.h"
#include "GAS_Study.h"
#include "Blueprint/UserWidget.h"
#include "Character/MyPlayerState.h"
#include "Components/ComboManagerComponent.h"
#include "Components/ProgressBar.h"
#include "GAS/GAS_StudyTags.h"
#include "GAS/Attributes/CharacterAttributeSet.h"
#include "UI/EnemyHPBar.h"
#include "UI/PlayerHUDWidget.h"

UAbilitySystemComponent* AGAS_StudyCharacter::GetAbilitySystemComponent() const
{
    // 캐시된 참조가 있으면 사용
    if (ASC)
    {
        return ASC;
    }

    // 없으면 PlayerState에서 가져옴
    if (const AMyPlayerState* PS = GetPlayerState<AMyPlayerState>())
    {
        return PS->GetAbilitySystemComponent();
    }

    return nullptr;
}

AGAS_StudyCharacter::AGAS_StudyCharacter()
{
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

    // Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
    // instead of recompiling to adjust them
    GetCharacterMovement()->JumpZVelocity = 500.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 500.f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f;
    CameraBoom->bUsePawnControlRotation = true;

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;
}

void AGAS_StudyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    // Set up action bindings
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Jumping
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AGAS_StudyCharacter::DoJumpStart);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AGAS_StudyCharacter::DoJumpEnd);

        // Moving
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGAS_StudyCharacter::Move);
        EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AGAS_StudyCharacter::Look);

        // Looking
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGAS_StudyCharacter::Look);

        // Attacking
        EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Started, this,
                                           &AGAS_StudyCharacter::DoLightAttack);
        EnhancedInputComponent->BindAction(MeleeAttackAction, ETriggerEvent::Started, this,
                                           &AGAS_StudyCharacter::DoMeleeAttack);
        EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this,
                                           &AGAS_StudyCharacter::DoHeavyAttack);
        EnhancedInputComponent->BindAction(ComboAttackAction, ETriggerEvent::Started, this,
                                           &AGAS_StudyCharacter::DoComboAttack);

        // Defensing
        EnhancedInputComponent->BindAction(DefenseAction, ETriggerEvent::Started, this,
                                           &AGAS_StudyCharacter::DoDefenseStart);
        EnhancedInputComponent->BindAction(DefenseAction, ETriggerEvent::Completed, this,
                                           &AGAS_StudyCharacter::DoDefenseEnd);

        // Roll
        EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Started, this,
                                           &AGAS_StudyCharacter::DoRollStart);
    }
    else
    {
        UE_LOG(LogGAS_Study, Error,
               TEXT(
                   "'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
               ), *GetNameSafe(this));
    }
}

void AGAS_StudyCharacter::Move(const FInputActionValue& Value)
{
    // input is a Vector2D
    FVector2D MovementVector = Value.Get<FVector2D>();

    // route the input
    DoMove(MovementVector.X, MovementVector.Y);
}

void AGAS_StudyCharacter::Look(const FInputActionValue& Value)
{
    // input is a Vector2D
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    // route the input
    DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AGAS_StudyCharacter::DoMove(float Right, float Forward)
{
    if (ASC->HasMatchingGameplayTag(GAS_StudyTags::State_Blocking)) return;

    if (GetController() != nullptr)
    {
        // find out which way is forward
        const FRotator Rotation = GetController()->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get forward vector
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

        // get right vector 
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        // add movement 
        AddMovementInput(ForwardDirection, Forward);
        AddMovementInput(RightDirection, Right);
    }
}

void AGAS_StudyCharacter::DoLook(float Yaw, float Pitch)
{
    if (GetController() != nullptr)
    {
        // add yaw and pitch input to controller
        AddControllerYawInput(Yaw);
        AddControllerPitchInput(Pitch);
    }
}

void AGAS_StudyCharacter::DoJumpStart()
{
    if (ASC)
    {
        // 찾고자 하는 태그를 컨테이너에 담습니다.
        FGameplayTagContainer TagContainer;
        TagContainer.AddTag(GAS_StudyTags::Ability_Action_Jump);

        // 해당 태그를 가진 어빌리티(GA_Jump)의 실행을 시도합니다.
        // bAllowRemoteActivation 매개변수는 기본값(true)을 사용하여 서버에도 실행을 요청합니다.
        ASC->TryActivateAbilitiesByTag(TagContainer);
    }
}

void AGAS_StudyCharacter::DoJumpEnd()
{
    // signal the character to stop jumping
    StopJumping();
}

void AGAS_StudyCharacter::DoLightAttack()
{
    if (ASC)
    {
        FGameplayTagContainer TagContainer;
        TagContainer.AddTag(GAS_StudyTags::Ability_Action_LightAttack);

        ASC->TryActivateAbilitiesByTag(TagContainer);
    }
}

void AGAS_StudyCharacter::DoMeleeAttack()
{
    if (ASC)
    {
        FGameplayTagContainer TagContainer;
        TagContainer.AddTag(GAS_StudyTags::Ability_Action_MeleeAttack);

        ASC->TryActivateAbilitiesByTag(TagContainer);
    }
}

void AGAS_StudyCharacter::DoHeavyAttack()
{
    if (ASC)
    {
        FGameplayTagContainer TagContainer;
        TagContainer.AddTag(GAS_StudyTags::Ability_Action_HeavyAttack);

        ASC->TryActivateAbilitiesByTag(TagContainer);
    }
}

void AGAS_StudyCharacter::DoComboAttack()
{
    if (UComboManagerComponent* ComboMgr = FindComponentByClass<UComboManagerComponent>())
    {
        ComboMgr->RequestAttack();
    }
}

void AGAS_StudyCharacter::DoDefenseStart()
{
    if (ASC)
    {
        FGameplayTagContainer TagContainer;
        TagContainer.AddTag(GAS_StudyTags::Ability_Action_Defense);

        ASC->TryActivateAbilitiesByTag(TagContainer);
    }
}

void AGAS_StudyCharacter::DoDefenseEnd()
{
    if (ASC)
    {
        FGameplayTagContainer TagContainer;
        TagContainer.AddTag(GAS_StudyTags::Ability_Action_Defense);

        ASC->CancelAbilities(&TagContainer);
    }
}

void AGAS_StudyCharacter::DoRollStart()
{
    if (ASC)
    {
        FGameplayTagContainer TagContainer;
        TagContainer.AddTag(GAS_StudyTags::Ability_Action_Roll);

        ASC->TryActivateAbilitiesByTag(TagContainer);
    }
}

void AGAS_StudyCharacter::GiveDefaultAbilities()
{
    // 서버(Authority)에서만, 그리고 ASC가 유효할 때만 실행합니다.
    if (!HasAuthority() || !ASC) return;

    // 🌟 핵심: 배열에 들어있는 모든 어빌리티를 꺼내서 자동으로 부여합니다.
    for (const TSubclassOf<UGameplayAbility>& StartupAbility : DefaultAbilities)
    {
        // 빈 슬롯(None)이 들어있을 수 있으므로 유효성 검사는 필수입니다.
        if (StartupAbility)
        {
            FGameplayAbilitySpec AbilitySpec(StartupAbility, 1, INDEX_NONE, this);
            ASC->GiveAbility(AbilitySpec);
        }
    }
}

void AGAS_StudyCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    // 서버에서 호출됨
    InitializeAbilitySystem();
    GiveDefaultAbilities();
}

void AGAS_StudyCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    // 클라이언트에서 호출됨
    InitializeAbilitySystem();
    // Ability 부여는 서버에서만 (클라이언트에서는 복제됨)
}

void AGAS_StudyCharacter::InitializeAbilitySystem()
{
    AMyPlayerState* PS = GetPlayerState<AMyPlayerState>();
    if (!PS)
    {
        return;
    }

    // PlayerState에서 ASC 가져와서 캐시
    ASC = PS->GetAbilitySystemComponent();
    AttributeSet = PS->GetAttributeSet();

    if (ASC)
    {
        // InitAbilityActorInfo 핵심!
        // Owner: PlayerState (ASC 소유자)
        // Avatar: this (물리적 캐릭터)
        ASC->InitAbilityActorInfo(PS, this);

        UE_LOG(LogTemp, Log, TEXT("[%s] ASC initialized from PlayerState"), *GetName());
    }
}

UCharacterAttributeSet* AGAS_StudyCharacter::GetAttributeSet() const
{
    // 캐시된 참조가 있으면 사용
    if (AttributeSet)
    {
        return AttributeSet;
    }

    // 없으면 PlayerState에서 가져옴
    if (const AMyPlayerState* PS = GetPlayerState<AMyPlayerState>())
    {
        return PS->GetAttributeSet();
    }

    return nullptr;
}

void AGAS_StudyCharacter::SetMainHUDWidget(UUserWidget* InWidget)
{
    MainHUDWidget = InWidget;
}

void AGAS_StudyCharacter::UpdatePlayerHUD_EnemyHP(float CurrentHP, float MaxHP)
{
    if (!MainHUDWidget) return;

    // 1. WBP_Player 내부에 있는 WBP_EnemyHPBar라는 이름의 위젯을 찾습니다.
    // (위젯 블루프린트에서 해당 위젯 변수 이름이 'WBP_EnemyHPBar'여야 합니다)
    UEnemyHPBar* HPBarWidget = Cast<UEnemyHPBar>(MainHUDWidget->GetWidgetFromName(TEXT("WBP_EnemyHPBar")));

    if (HPBarWidget)
    {
        // 2. 그 안에 있는 ProgressBar를 찾습니다. (이름이 'HPProgressBar'라고 가정)
        UProgressBar* ProgressBar = Cast<UProgressBar>(HPBarWidget->GetWidgetFromName(TEXT("HPProgressBar")));

        if (ProgressBar && MaxHP > 0.f)
        {
            ProgressBar->SetPercent(CurrentHP / MaxHP);

            // 3. 체력이 0보다 크면 보이게, 0이면 숨기기 (선택 사항)
            HPBarWidget->SetVisibility(CurrentHP > 0.f ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
        }
    }
}

void AGAS_StudyCharacter::UpdateEnemyHPOnHUD(float CurrentHP, float MaxHP)
{
    if (!MainHUDWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("MainHUDWidget이 없습니다!"));
        return;
    }

    // 1단계: WBP_Player 내부에서 'WBP_EnemyHPBar' 위젯을 찾습니다.
    UUserWidget* EnemyHPBarWidget = Cast<UUserWidget>(MainHUDWidget->GetWidgetFromName(TEXT("WBP_EnemyHPBar")));
    
    if (EnemyHPBarWidget)
    {
        // [중요] 2단계: 찾은 'WBP_EnemyHPBar' 내부에서 다시 'HPProgressBar'를 찾아야 합니다.
        // 그냥 MainHUDWidget에서 찾으면 못 찾을 확률이 높습니다.
        UProgressBar* HPBar = Cast<UProgressBar>(EnemyHPBarWidget->GetWidgetFromName(TEXT("HPProgressBar")));
        
        if (HPBar)
        {
            float Percent = (MaxHP > 0.f) ? (CurrentHP / MaxHP) : 0.f;
            HPBar->SetPercent(Percent);
            
            // 데미지를 입었을 때만 보이게 설정
            EnemyHPBarWidget->SetVisibility(ESlateVisibility::Visible);
            
            UE_LOG(LogTemp, Log, TEXT("UI 업데이트 성공: %f%%"), Percent * 100.f);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("WBP_EnemyHPBar 안에서 'HPProgressBar'를 찾지 못했습니다!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("MainHUDWidget 안에서 'WBP_EnemyHPBar'를 찾지 못했습니다!"));
    }
}

void AGAS_StudyCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    // 1. 내 캐릭터(로컬 플레이어)인 경우에만 UI를 생성합니다. (네트워크 멀티플레이 방지)
    if (IsLocallyControlled())
    {
        APlayerController* PC = Cast<APlayerController>(GetController());
        
        // 2. 에디터에서 PlayerHUDClass(WBP_Player)가 잘 등록되었는지 확인
        if (PC && PlayerHUDClass)
        {
            // 3. 위젯 생성
            UUserWidget* CreatedWidget = CreateWidget<UUserWidget>(PC, PlayerHUDClass);
            if (CreatedWidget)
            {
                // 4. 화면에 띄우기
                CreatedWidget->AddToViewport();

                // 5. [핵심] 완성된 위젯의 주소를 MainHUDWidget 변수에 저장!
                SetMainHUDWidget(CreatedWidget);

                UE_LOG(LogTemp, Log, TEXT("C++에서 UI 생성 및 MainHUDWidget 연결 완료!"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("PlayerHUDClass가 설정되지 않았습니다. 블루프린트를 확인하세요."));
        }
    }
}
