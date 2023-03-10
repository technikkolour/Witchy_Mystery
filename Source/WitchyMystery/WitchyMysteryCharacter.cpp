// Copyright Epic Games, Inc. All Rights Reserved.

#include "WitchyMysteryCharacter.h"
#include "Item.h"
#include "InteractableInterface.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Grabber.h"


//////////////////////////////////////////////////////////////////////////
// AWitchyMysteryCharacter

AWitchyMysteryCharacter::AWitchyMysteryCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AWitchyMysteryCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Initialise health and stamina
	CurrentHealth = 100;
	CurrentStamina = 100;
}

void AWitchyMysteryCharacter::AddToInventory()
{
	FVector Start = FollowCamera->GetComponentLocation();
	FVector End = Start + FollowCamera->GetForwardVector() * 500.0f;
	FHitResult HitResult;
	FCollisionQueryParams Params;

	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, Params)) {
		if (IInteractableInterface* Interface = Cast<IInteractableInterface>(HitResult.GetActor())) {
			Interface->AddToInventory(this);
		}
	}
}

void AWitchyMysteryCharacter::CastSpell()
{
	FVector Start = FollowCamera->GetComponentLocation();
	FVector End = Start + FollowCamera->GetForwardVector() * 700.0f + FollowCamera->GetRightVector() * (60.0f);
	FHitResult HitResult;
	FCollisionQueryParams Params;

	Params.AddIgnoredActor(this);

	UPhysicsHandleComponent* PhysicsHandle = FindComponentByClass<UPhysicsHandleComponent>();

	if (PhysicsHandle == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("No Physics Handle."));
	}

	if (CurrentStamina > 0.5) {
		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, Params)) {
			PhysicsHandle->GrabComponentAtLocationWithRotation(
				HitResult.GetComponent(),
				NAME_None,
				HitResult.ImpactPoint,
				HitResult.GetComponent()->GetComponentRotation());
		}

		if (PhysicsHandle->GetGrabbedComponent() != nullptr) {
			CurrentStamina -= 0.5;
			UE_LOG(LogTemp, Warning, TEXT("Stamina: %f"), CurrentStamina);
		}
	}
	else {
		StopCast();
	}
}

void AWitchyMysteryCharacter::StopCast()
{
	UPhysicsHandleComponent* PhysicsHandle = FindComponentByClass<UPhysicsHandleComponent>();

	if (PhysicsHandle == nullptr) {
		return;
	}

	if (PhysicsHandle->GetGrabbedComponent() != nullptr) {
		PhysicsHandle->GetGrabbedComponent()->WakeAllRigidBodies();
		PhysicsHandle->ReleaseComponent();
	}
}

void AWitchyMysteryCharacter::Interact()
{
}

void AWitchyMysteryCharacter::TakeDamage(float Damage)
{
	if (CurrentHealth > Damage)
		CurrentHealth -= Damage;
	else
		CurrentHealth = 0;
}

void AWitchyMysteryCharacter::Tick(float DeltaSeconds)
{
	UPhysicsHandleComponent* PhysicsHandle = FindComponentByClass<UPhysicsHandleComponent>();

	if (CurrentStamina < 100 && PhysicsHandle->GetGrabbedComponent() == nullptr) {
		if (CurrentStamina < 95)
			CurrentStamina += 0.25;
		else
			CurrentStamina = 100;
	}

	UE_LOG(LogTemp, Warning, TEXT("Stamina: %f"), CurrentStamina);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AWitchyMysteryCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AWitchyMysteryCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AWitchyMysteryCharacter::Look);

		//Adding items to inventory
		EnhancedInputComponent->BindAction(AddToInventoryAction, ETriggerEvent::Triggered, this, &AWitchyMysteryCharacter::AddToInventory);

		//Casting spells
		EnhancedInputComponent->BindAction(CastSpellAction, ETriggerEvent::Triggered, this, &AWitchyMysteryCharacter::CastSpell);

		//Stop casting spell
		EnhancedInputComponent->BindAction(StopCastAction, ETriggerEvent::Triggered, this, &AWitchyMysteryCharacter::StopCast);

		//Interacting with objects
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AWitchyMysteryCharacter::Interact);
	}

}

void AWitchyMysteryCharacter::Use(TSubclassOf<AItem> Item)
{
	if (Item) {
		if (AItem* CDOItem = Item.GetDefaultObject()) {
			UE_LOG(LogTemp, Warning, TEXT("%s"), *CDOItem->IsUsed());
		}
	}
}

void AWitchyMysteryCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AWitchyMysteryCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}