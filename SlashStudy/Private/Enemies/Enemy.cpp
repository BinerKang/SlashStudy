// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Enemy.h"
#include "SlashStudy/DebugMacros.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/AttributeComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "kismet/GameplayStatics.h"
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
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
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
	HealthBarComponent->SetVisibility(false);

	this->EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);

	this->PawnSensingComp->OnSeePawn.AddDynamic(this, &AEnemy::OnSeePawn);

	if (WeaponClass && GetWorld()) 
	{
		EquippedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
		EquippedWeapon->Equip(GetMesh(), RIGHT_HAND_SOCKET, this, this);
	}
}

void AEnemy::MoveToTarget(AActor* Target, float AcceptanceRadius)
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
	if (EnemyState > EEnemyState::EES_Patrolling) 
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
			TObjectPtr<AActor> NewTarget = PatrolTargets[FMath::RandRange(0, PatrolTargets.Num() - 1)];
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
		if (!IsInTargetRange(CombatTarget, this->CombatRadius))
		{
			// outside combat radius lose interest
			CombatTarget = nullptr;
			this->EnemyState = EEnemyState::EES_Patrolling;
			GoToPatrolling();
			//UE_LOG(LogTemp, Warning, TEXT("Lose Interest, Go to Patrolling..."));
			HealthBarComponent->SetVisibility(false);
		}
		else if (!IsInTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Chasing)
		{
			// In combat radius, not in attack radius, and not chasing : Go To Chasing
			this->EnemyState = EEnemyState::EES_Chasing;
			GoToChasing();
			//UE_LOG(LogTemp, Warning, TEXT("Chasing"));
		}
		else if (IsInTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Attacking)
		{
			EnemyState = EEnemyState::EES_Attacking;
			// TODO Attack
			//UE_LOG(LogTemp, Warning, TEXT("Attacking"));
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
	if (Pawn->ActorHasTag(SLASH_CHARACTER_TAG))
	{
		this->CombatTarget = Pawn;
		EnemyState = EEnemyState::EES_Chasing;
		this->GoToChasing();
		//UE_LOG(LogTemp, Warning, TEXT("Seen Slash Character"));
	}
}

void AEnemy::GetHit(const FVector& ImpactPoint)
{
	// DRAW_SPHERE_Color(ImpactPoint, FColor::Orange);
	if (AttributeComponent->IsAlive())
	{
		HealthBarComponent->SetVisibility(true);
		DirectionalHitReact(ImpactPoint);
	} 
	else 
	{
		Die();
	}
	
	if (HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, ImpactPoint);
	}

	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}
}

float AEnemy::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	AttributeComponent->ReceiveDamage(Damage);
	HealthBarComponent->SetHealthPercent(AttributeComponent->GetHealthPercent());
	this->CombatTarget = EventInstigator->GetPawn();
	this->EnemyState = EEnemyState::EES_Chasing;
	GoToChasing();
	return Damage;
}

void AEnemy::Die()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->HealthBarComponent->SetVisibility(false);
	this->SetLifeSpan(5.f);
	this->CombatTarget = nullptr;
	this->PatrolTarget = nullptr;

	TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && this->DeathMontage)
	{
		const int32 Index = FMath::RandRange(1, 6);
		char* Section = "Death6";
		this->DeathPose = EDeathPose::EDP_Death6;
		switch (Index)
		{
		case 1:
			Section = "Death1"; 
			this->DeathPose = EDeathPose::EDP_Death1;
			break;
		case 2:
			Section = "Death2"; 
			this->DeathPose = EDeathPose::EDP_Death2;
			break;
		case 3:
			Section = "Death3"; 
			this->DeathPose = EDeathPose::EDP_Death3;
			break;
		case 4:
			Section = "Death4"; 
			this->DeathPose = EDeathPose::EDP_Death4;
			break;
		case 5:
			Section = "Death5"; 
			this->DeathPose = EDeathPose::EDP_Death5;
			break;
		default:
			break;
		}
		AnimInstance->Montage_Play(this->DeathMontage);
		AnimInstance->Montage_JumpToSection(FName(Section), this->DeathMontage);
	}
}

bool AEnemy::IsInTargetRange(AActor* Target, float Radius)
{
	// DRAW_SPHERE_SingleFrame(Target->GetActorLocation());
	//DRAW_SPHERE_SingleFrame(this->GetActorLocation());
	return (Target->GetActorLocation() - this->GetActorLocation()).Length() <= Radius;
}

void AEnemy::GoToChasing()
{
	GetWorldTimerManager().ClearTimer(PatrolWaitTimerHandle);
	this->GetCharacterMovement()->MaxWalkSpeed = this->ChasingSpeed;
	MoveToTarget(this->CombatTarget);
}

void AEnemy::GoToPatrolling()
{
	this->GetCharacterMovement()->MaxWalkSpeed = this->PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

void AEnemy::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}

