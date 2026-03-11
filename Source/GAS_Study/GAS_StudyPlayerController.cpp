// Copyright Epic Games, Inc. All Rights Reserved.


#include "GAS_StudyPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "GAS_Study.h"
#include "UI/PlayerHUDWidget.h"
#include "Widgets/Input/SVirtualJoystick.h"

void AGAS_StudyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (SVirtualJoystick::ShouldDisplayTouchInterface() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogGAS_Study, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
	
	// 로컬 플레이어일 때만 UI를 띄웁니다.
	if (IsLocalPlayerController() && HUDWidgetClass)
	{
		ActiveHUD = CreateWidget<UPlayerHUDWidget>(this, HUDWidgetClass);
		if (ActiveHUD)
		{
			ActiveHUD->AddToViewport();
			
			UpdateHUD(GetPawn());
		}
	}
}

void AGAS_StudyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!SVirtualJoystick::ShouldDisplayTouchInterface())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

void AGAS_StudyPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);
	
	// 클라이언트 측에서도 UI를 갱신합니다.
	UpdateHUD(P);
}

void AGAS_StudyPlayerController::UpdateHUD(APawn* InPawn)
{
	// 화면에 UI가 잘 띄워져 있고, 빙의할 폰(캐릭터)이 존재한다면
	if (ActiveHUD && InPawn)
	{
		// 빙의한 폰에서 ASC를 찾아냅니다.
		UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InPawn);
		
		if (ASC)
		{
			// UI에게 "새로운 육체의 ASC야! PlayerHUD를 이쪽에 연결해 줘!" 라고 명령합니다.
			ActiveHUD->InitWidget(ASC);
		}
	}
}
