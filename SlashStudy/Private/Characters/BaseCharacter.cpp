// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Items/Weapons/Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/AttributeComponent.h"
#include "kismet/GameplayStatics.h"
#include "SlashStudy/Tags.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributesComp"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Attack(const FInputActionValue& Value)
{
	// If combatTarget has dead, stop attack
	if (CombatTarget && CombatTarget->ActorHasTag(DEAD_TAG)) CombatTarget = nullptr;
}

void ABaseCharacter::Die()
{
	this->Tags.Add(DEAD_TAG);
	PlayDeathMontage();
}

void ABaseCharacter::GetHit(const FVector& ImpactPoint, AActor* Hitter)
{
	if (IsAlive() && Hitter)
	{
		DirectionalHitReact(Hitter->GetActorLocation());
	}
	else Die();

	PlayHitSound(ImpactPoint);
	SpawnHitParticle(ImpactPoint);
}

bool ABaseCharacter::IsAlive()
{
	return AttributeComponent && AttributeComponent->IsAlive();
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
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

void ABaseCharacter::SpawnHitParticle(const FVector& ImpactPoint)
{
	if (HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, ImpactPoint);
	}
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}
}

FVector ABaseCharacter::GetTranslationWarp()
{
	if (CombatTarget)
	{
		const FVector TargetToMeNormalVector = (GetActorLocation() - CombatTarget->GetActorLocation()).GetSafeNormal();
		return CombatTarget->GetActorLocation() + TargetToMeNormalVector * WarpTargetDistance;
	}
	return FVector();
}

FVector ABaseCharacter::GetRotationWarp()
{
	if (CombatTarget) return CombatTarget->GetActorLocation();
	return FVector();
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

int32 ABaseCharacter::PlayMontageRandomSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if (SectionNames.Num() < 0) return -1;
	int32 i = FMath::RandRange(0, SectionNames.Num() - 1);
	PlayMontageSection(Montage, SectionNames[i]);
	return i;
}

void ABaseCharacter::PlayAttackMontage()
{
	PlayMontageRandomSection(AttackMontage, AttackMontageSections);	
}

void ABaseCharacter::StopAttackMontage()
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Stop(.25f, AttackMontage);
	}
}

void ABaseCharacter::PlayDodgeMontage()
{
	PlayMontageSection(DodgeMontage, FName("Default"));
}

void ABaseCharacter::PlayDeathMontage()
{
	const int32 i = PlayMontageRandomSection(DeathMontage, DeathMontageSections);
	if (i == -1) return;
	TEnumAsByte<EDeathPose> Pose(i);
	if (Pose < EDeathPose::EDP_Max)
	{
		DeathPose = Pose;
	}
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (AttributeComponent) {
		AttributeComponent->ReceiveDamage(DamageAmount);
	}
}

void ABaseCharacter::AttackEnd()
{
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::SetWeaponBoxCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}
