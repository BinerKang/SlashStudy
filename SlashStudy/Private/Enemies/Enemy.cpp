// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Enemy.h"
#include "SlashStudy/DebugMacros.h"
#include "Components/CapsuleComponent.h"
#include "SlashStudy/DebugMacros.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "kismet/GameplayStatics.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "AIController.h"

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

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributesComp"));

	this->GetCharacterMovement()->bOrientRotationToMovement = true;
	this->bUseControllerRotationYaw = false;
}
	

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	// Because Constructer Method had initilized. Not Check HealthBarComponent
	HealthBarComponent->SetHealthPercent(1.f);
	// hide Health Bar Widget
	HealthBarComponent->SetVisibility(false);

	this->EnemyController = Cast<AAIController>(GetController());
	if (EnemyController && PatrolTarget)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(PatrolTarget);
		MoveRequest.SetAcceptanceRadius(15.f);
		FNavPathSharedPtr NavPath;
		EnemyController->MoveTo(MoveRequest, &NavPath);
		/*for (auto& Point : NavPath->GetPathPoints())
		{
			DrawDebugSphere(GetWorld(), Point.Location, 12.f, 12.f, FColor::Green, false, 20.f);
		}*/
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	this->GroundSpeed = UKismetMathLibrary::VSizeXY(this->GetCharacterMovement()->Velocity);
	if (this->CombatTarget)
	{
		if (!IsInTargetRange(CombatTarget, this->CombatRadius))
		{
			CombatTarget = nullptr;
			HealthBarComponent->SetVisibility(false);
		}
	}
	if (PatrolTarget && EnemyController && PatrolTargets.Num())
	{
		if (IsInTargetRange(PatrolTarget, PatrolRadius))
		{
			int32 RemoveNum = PatrolTargets.Remove(PatrolTarget);// Remove Current Patrol Target
			// Random Pick A New Target
			TObjectPtr<AActor> NewTarget = PatrolTargets[FMath::RandRange(0, PatrolTargets.Num() - 1)];
			// if removed then readd Old Target
			if (RemoveNum)
			{
				PatrolTargets.Add(PatrolTarget);
			}
			// Assign New Target to PatrolTarget
			PatrolTarget = NewTarget;
			// AI move to new Target
			FAIMoveRequest MoveRequest;
			MoveRequest.SetGoalActor(PatrolTarget);
			MoveRequest.SetAcceptanceRadius(15.f);
			EnemyController->MoveTo(MoveRequest);
		}
	}
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
	TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
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
	return Damage;
}

void AEnemy::DirectionalHitReact(const FVector& ImpactPoint)
{
	// Calculate Theta between ImpactPoint And ActorForward
	const FVector Forward = GetActorForwardVector();
	// make sure Z axis value = 0 , ActorForward's Z axis is 0. GetSafeNormal: make vector to unit
	const FVector ToHit = FVector(ImpactPoint.X - GetActorLocation().X, ImpactPoint.Y - GetActorLocation().Y, 0).GetSafeNormal();
	// Forward * ToHit = |Forward| * |ToHit| * CosTheta
	// |Forward| = 1 |ToHit| = 1, so Forward * ToHit = CosTheta
	// Theta = ArcCosine(Forward * ToHit)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	double Theta = FMath::Acos(CosTheta);
	// convert radians to degrees
	Theta = FMath::RadiansToDegrees(Theta);
	// if crossProdect points down,Theta should be negative
	if (FVector::CrossProduct(Forward, ToHit).Z < 0)
	{
		Theta = -Theta;
	}
	char* Section = "FromBack";
	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = "FromFront";
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = "FromLeft";
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = "FromRight";
	}
	PlayHitReactMontage(FName(Section));

	/*
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + FVector::CrossProduct(Forward, ToHit) * 100.f, 5.f, FColor::Blue, 5.f);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Orange, FString::Printf(TEXT("Degrees: %f"), Theta));
	}
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 60.f, 5.f, FColor::Red, 5.f);
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 60.f, 5.f, FColor::Green, 5.f);
	*/
}

void AEnemy::Die()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->HealthBarComponent->SetVisibility(false);
	this->SetLifeSpan(5.f);
	this->CombatTarget = nullptr;

	TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && this->DeathMontage)
	{
		const int32 Index = FMath::RandRange(1, 6);
		char* Section = "Death6";
		this->DeathPose = EDeathPose::EAS_Death6;
		switch (Index)
		{
		case 1:
			Section = "Death1"; 
			this->DeathPose = EDeathPose::EAS_Death1;
			break;
		case 2:
			Section = "Death2"; 
			this->DeathPose = EDeathPose::EAS_Death2;
			break;
		case 3:
			Section = "Death3"; 
			this->DeathPose = EDeathPose::EAS_Death3;
			break;
		case 4:
			Section = "Death4"; 
			this->DeathPose = EDeathPose::EAS_Death4;
			break;
		case 5:
			Section = "Death5"; 
			this->DeathPose = EDeathPose::EAS_Death5;
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
	DRAW_SPHERE_SingleFrame(Target->GetActorLocation());
	DRAW_SPHERE_SingleFrame(this->GetActorLocation());
	return (Target->GetActorLocation() - this->GetActorLocation()).Length() <= Radius;
}

