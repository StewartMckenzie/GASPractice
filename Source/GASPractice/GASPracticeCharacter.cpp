// Copyright Epic Games, Inc. All Rights Reserved.

#include "GASPracticeCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "PlayerAttributes.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// AGASPracticeCharacter

AGASPracticeCharacter::AGASPracticeCharacter(const class FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//create a default the ability system component as a default subobject 
	AbilitySystem= CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGASPracticeCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGASPracticeCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGASPracticeCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AGASPracticeCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AGASPracticeCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AGASPracticeCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AGASPracticeCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AGASPracticeCharacter::OnResetVR);
}

UAbilitySystemComponent* AGASPracticeCharacter::GetAbilitySystemComponent() const
{
	return  AbilitySystem;
}

void AGASPracticeCharacter::BeginPlay()
{
	Super::BeginPlay();
	//is we have an ability system component initialize the stats from our data table
if(AbilitySystem)
{
	const UAttributeSet* Attributes = AbilitySystem->InitStats(UPlayerAttributes::StaticClass(),AttributeDataTable);
}
//if our debugging flag is triggered 
	if(bAttributesDebugging)
	{
		//loop over our array of attributes/abilities 
		for(size_t i =0;i<DebuggingPassiveAbilities.Num();i++)
		{
			//Give the ability system the ability at this index and save it as a refrence
			FGameplayAbilitySpecHandle specHandle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(DebuggingPassiveAbilities[i].GetDefaultObject(),1,0));
			//try to activate that ability on the server (This doesn't remove the ability after it fires, "GiveAbilityAndActivateOnce" does this in one line ,but removes the ability after it is called )
			AbilitySystem->CallServerTryActivateAbility(specHandle,false,FPredictionKey());

		}
		for(size_t i =0;i<StartingPassiveAbilities.Num();i++)
		{
		
			//Give the ability system the ability at this index and save it as a refrence
			FGameplayAbilitySpecHandle specHandle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(StartingPassiveAbilities[i].GetDefaultObject(),1,0));
			//try to activate that ability on the server (This doesn't remove the ability after it fires, "GiveAbilityAndActivateOnce" does this in one line ,but removes the ability after it is called )
			AbilitySystem->CallServerTryActivateAbility(specHandle,false,FPredictionKey());

		}
	}
}


void AGASPracticeCharacter::OnResetVR()
{
	// If GASPractice is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in GASPractice.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AGASPracticeCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AGASPracticeCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AGASPracticeCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AGASPracticeCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AGASPracticeCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AGASPracticeCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
