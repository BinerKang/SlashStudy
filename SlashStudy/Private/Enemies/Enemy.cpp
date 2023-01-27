// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Enemy.h"
#include "SlashStudy/DebugMacros.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/AttributeComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Soul.h"
#include "Kismet/KismetMathLibrary.h"
#include "HUD/HealthBarComponent.h"
#include "AIController.h"
#include "Perception/PawnSensingComponent.h"
#include "SlashStudy/Tags.h"
#include "SlashStudy/SocketNames.h"


AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	// Those settings make sure WeaponBoxCollision can overlapp.
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	HealthBarComponent = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBarComp"));
	HealthBarComponent->SetupAttachment(GetRootComponent());

	this->GetCharacterMovement()->bOrientRotationToMovement = true;
	this->bUseControllerRotationYaw = false;

	this->PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	this->PawnSensingComp->SetPeripheralVisionAngle(45.f);
	this->PawnSensingComp->SightRadius = 4000.f;
}
	

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	// Because Constructer Method had initilized. Not Check HealthBarComponent
	HealthBarComponent->SetHealthPercent(1.f);
	// hide Health Bar Widget
	HideHealthBar();

	// Add Tag
	this->Tags.Add(ENEMY_TAG);

	this->EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);

	this->PawnSensingComp->OnSeePawn.AddDynamic(this, &AEnemy::OnSeePawn);

	// Spawn Default Weapon
	if (WeaponClass && GetWorld()) 
	{
		EquippedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
		EquippedWeapon->Equip(GetMesh(), WEAPON_SOCKET, this, this);
	}
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (!Target || !EnemyController) return;
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	EnemyController->MoveTo(MoveRequest);
	//FNavPathSharedPtr NavPath;
	//EnemyController->MoveTo(MoveRequest, &NavPath);
	/*for (auto& Point : NavPath->GetPathPoints())
	{
	DrawDebugSphere(GetWorld(), Point.Location, 12.f, 12.f, FColor::Green, false, 20.f);
	}*/
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	this->GroundSpeed = UKismetMathLibrary::VSizeXY(this->GetCharacterMovement()->Velocity);
	if (IsEngaged()) {
		// Engaged(playing attack montage) do nothing
	} else if (EnemyState > EEnemyState::EES_Patrolling) 
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}

}

void AEnemy::CheckPatrolTarget()
{
	if (PatrolTarget && EnemyController && PatrolTargets.Num())
	{
		if (IsInTargetRange(PatrolTarget, PatrolRadius))
		{
			int32 RemovedNum = PatrolTargets.Remove(PatrolTarget);// Remove Current Patrol Target
			// Random Pick A New Target
			AActor* NewTarget = PatrolTargets[FMath::RandRange(0, PatrolTargets.Num() - 1)];
			// if removed then add Old Target back.
			if (RemovedNum)
			{
				PatrolTargets.Add(PatrolTarget);
			}
			// Assign New Target to PatrolTarget
			PatrolTarget = NewTarget;
			// Timer to handle AI Move
			GetWorldTimerManager().SetTimer(
				PatrolWaitTimerHandle, 
				this,
				&AEnemy::PatrolTimerFinished, 
				FMath::RandRange(this->PatrolWaitMinSecond, this->PatrolWaitMaxSecond)
			);
		}
	}
}

void AEnemy::CheckCombatTarget()
{
	if (this->CombatTarget)
	{
		if (IsOutsideCombatRadius())
		{
			ClearAttackTimer();
			LoseInterest();
			GoToPatrolling();
			//UE_LOG(LogTemp, Warning, TEXT("Lose Interest, Go to Patrolling..."));	
		}
		else if (IsOutsideAttackRadius() && !IsChasing())
		{
			// In combat radius, not in attack radius, and not chasing : Go To Chasing
			ClearAttackTimer();
			GoToChasing();
			//UE_LOG(LogTemp, Warning, TEXT("Chasing"));
		}
		else if (CanAttack())
		{
			//UE_LOG(LogTemp, Warning, TEXT("Attacking"));
			StartAttackTimer();
		}
	}
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(this->PatrolTarget);
}

void AEnemy::OnSeePawn(APawn* Pawn)
{
	// If Enemy Chasing or Attacking , Do noting.
	if (EnemyState != EEnemyState::EES_Patrolling) return;
	if (Pawn->ActorHasTag(ENGAGEABLE_TARGET_TAG) && !Pawn->ActorHasTag(DEAD_TAG))
	{
		this->CombatTarget = Pawn;
		CheckCombatTarget();
		//UE_LOG(LogTemp, Warning, TEXT("Seen Slash Character"));
	}
}

void AEnemy::HideHealthBar()
{
	HealthBarComponent->SetVisibility(false);
}

void AEnemy::ShowHealthBar()
{
	HealthBarComponent->SetVisibility(true);
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	HideHealthBar();
}

void AEnemy::GetHit(const FVector& ImpactPoint, AActor* Hitter)
{
	// DRAW_SPHERE_Color(ImpactPoint, FColor::Orange);
	ClearPatrolWaitTimer();
	ClearAttackTimer();
	Super::GetHit(ImpactPoint, Hitter);
	SetWeaponBoxCollisionEnabled(ECollisionEnabled::NoCollision);
	StopAttackMontage();
	if (IsAlive())
	{
		ShowHealthBar();
		if (IsInsideAttackRadius()) StartAttackTimer();
	}
}

float AEnemy::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(Damage);
	this->CombatTarget = EventInstigator->GetPawn();
	// Attack Montage was broke, can not call attack end function,so check IsEngaged, switch enemy state
	if(IsEngaged()) EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
	return Damage;
}

void AEnemy::Die()
{
	ClearAttackTimer();
	this->EnemyState = EEnemyState::EES_Dead;
	this->CombatTarget = nullptr;
	this->PatrolTarget = nullptr;
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetWeaponBoxCollisionEnabled(ECollisionEnabled::NoCollision);
	HideHealthBar();
	this->SetLifeSpan(DeadLifeSpan);
	
	if (bNeedPhysicalWhenDie)
	{
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}
	Super::Die();
	
	SpawnSoulActor();
}

void AEnemy::SpawnSoulActor()
{
	if (SoulClass)
	{
		if (UWorld* World = GetWorld())
		{
			ASoul* Soul = World->SpawnActor<ASoul>(SoulClass, GetActorLocation() + FVector(0.f, 0.f, 125.f), GetActorRotation());
			if (Soul && AttributeComponent)
			{
				Soul->SetSouls(AttributeComponent->GetSouls());
				Soul->SetOwner(this);
			}
		}
	}
}

void AEnemy::Attack(const FInputActionValue& Value)
{
	Super::Attack(Value);
	if (!CombatTarget)
	{
		GoToPatrolling();
	}
	else
	{
		EnemyState = EEnemyState::EES_Engaged;
		PlayAttackMontage();
	}	
}

void AEnemy::Attack()
{
	Attack(NULL);
}

bool AEnemy::IsInTargetRange(AActor* Target, float Radius)
{
	// DRAW_SPHERE_SingleFrame(Target->GetActorLocation());
	//DRAW_SPHERE_SingleFrame(this->GetActorLocation());
	return (Target->GetActorLocation() - this->GetActorLocation()).Length() <= Radius;
}

void AEnemy::GoToChasing()
{
	this->EnemyState = EEnemyState::EES_Chasing;
	ClearPatrolWaitTimer();
	this->GetCharacterMovement()->MaxWalkSpeed = this->ChasingSpeed;
	MoveToTarget(this->CombatTarget);
}

void AEnemy::GoToPatrolling()
{
	this->EnemyState = EEnemyState::EES_Patrolling;
	this->GetCharacterMovement()->MaxWalkSpeed = this->PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

bool AEnemy::IsOutsideCombatRadius()
{
	return !IsInTargetRange(CombatTarget, this->CombatRadius);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEnemy::ClearPatrolWaitTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolWaitTimerHandle);
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);
	if (HealthBarComponent) {
		HealthBarComponent->SetHealthPercent(AttributeComponent->GetHealthPercent());
	}	
}

bool AEnemy::CanAttack()
{
	return IsInsideAttackRadius() && !IsAttacking() && !IsEngaged();
}

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !IsInsideAttackRadius();
}

bool AEnemy::IsInsideAttackRadius()
{
	return IsInTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, 1.f);
}

void AEnemy::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}

