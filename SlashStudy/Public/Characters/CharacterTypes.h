#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8 {
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon")
};

UENUM(BlueprintType)
enum class EActionState : uint8 {
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_Equipping UMETA(DisplayName = "Equipping")
};

UENUM(BlueprintType)
enum class EDeathPose : uint8 {
	EDP_Alive UMETA(DisplayName = "Alive"),
	EAS_Death1 UMETA(DisplayName = "Death1"),
	EAS_Death2 UMETA(DisplayName = "Death2"),
	EAS_Death3 UMETA(DisplayName = "Death3"),
	EAS_Death4 UMETA(DisplayName = "Death4"),
	EAS_Death5 UMETA(DisplayName = "Death5"),
	EAS_Death6 UMETA(DisplayName = "Death6")
};