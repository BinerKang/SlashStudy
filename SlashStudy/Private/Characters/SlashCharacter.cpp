// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"
#include "Items/Weapons/Weapon.h"

#include "SlashStudy/Tags.h"
#include "SlashStudy/SocketNames.h"

ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
	
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	// Add SpringArm And Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;
	
	// Camera Control
	CameraBoom->bUsePawnControlRotation = true;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	//Groom Hair Eyebrows
	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");
	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();
	// Add Tag
	this->Tags.Add(ENGAGEABLE_TARGET_TAG);
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(SlashMappingContext, 0);
		}
	}
}

void ASlashCharacter::Move(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;

	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
	AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), MovementVector.Y);
	AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), MovementVector.X);
}

void ASlashCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();
	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(LookVector.Y);
}

void ASlashCharacter::DoJump(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		ACharacter::Jump();
	}
}

void ASlashCharacter::EKeyPressed(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		if (AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem))
		{
			OverlappingWeapon->Equip(this->GetMesh(), RIGHT_HAND_SOCKET, this, this);
			this->CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
			this->OverlappingItem = nullptr;
			this->EquippedWeapon = OverlappingWeapon;
		}
		else
		{
			if (CanDisarm())
			{
				this->PlayEquipMontage(FName("Unequip"));
				this->CharacterState = ECharacterState::ECS_Unequipped;
				this->ActionState = EActionState::EAS_Equipping;
			}
			else if (CanArm())
			{
				this->PlayEquipMontage(FName("Equip"));
				this->CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
				this->ActionState = EActionState::EAS_Equipping;
			}
		}
	}
}

void ASlashCharacter::Attack(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		Super::Attack(Value);
		if (CanAttack())
		{
			ActionState = EActionState::EAS_Attacking;
			PlayAttackMontage();
		}
	}
}

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

void ASlashCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		this->ActionState = EActionState::EAS_Equipping;
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

bool ASlashCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped &&
		EquippedWeapon;
}

void ASlashCharacter::Arm()
{
	if (this->EquippedWeapon)
	{
		this->EquippedWeapon->AttachMeshToSocket(this->GetMesh(), RIGHT_HAND_SOCKET);
	}
}

void ASlashCharacter::Disarm()
{
	if (this->EquippedWeapon)
	{
		this->EquippedWeapon->AttachMeshToSocket(this->GetMesh(), SPINE_SOCKET);
	}
}

void ASlashCharacter::FinishEquipping()
{
	this->ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::HitReactionEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComp = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);
		EnhancedInputComp->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Look);
		EnhancedInputComp->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ASlashCharacter::DoJump);
		EnhancedInputComp->BindAction(EKeyAction, ETriggerEvent::Triggered, this, &ASlashCharacter::EKeyPressed);
		EnhancedInputComp->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Attack);
	}
}

void ASlashCharacter::GetHit(const FVector& ImpactPoint, AActor* Hitter)
{
	ActionState = EActionState::EAS_HitReaction;
	// if attacking GetHit will broken attack, disable box collision function not call, so call function here.
	SetWeaponBoxCollisionEnabled(ECollisionEnabled::NoCollision);
	Super::GetHit(ImpactPoint, Hitter);
}

