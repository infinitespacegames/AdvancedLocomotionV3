// Copyright is Bullshit!  Do as you will with these files.

#include "HealthComponent.h"

#include <GameFramework/Actor.h>
#include <GameFramework/DamageType.h>
#include <GameFramework/Controller.h>
#include <Kismet/GameplayStatics.h>
#include <Net/UnrealNetwork.h>


// Base constructor
UHealthComponent::UHealthComponent() {

	PrimaryComponentTick.bCanEverTick = true;

	MaxHealth = 100.0f;
	Health = MaxHealth;
	CurrentHealth = Health;

	MaxShield = 100.0f;
	Shield = 0;
	CurrentShield = Shield;

	bSpawnWithShields = false;

	SetIsReplicated(true);
}


// Called when the game starts
void UHealthComponent::BeginPlay() {
	Super::BeginPlay();
	if (bSpawnWithShields) {
		Shield = MaxShield;
	}

	// Server only hook
	if (GetOwnerRole() == ROLE_Authority) {
		AActor* MyOwner = GetOwner();
		if (MyOwner) {
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleDamage);
		}
	}
}


// Play desired effects upon taking damage
void UHealthComponent::PlayDamageEffects() {
	AActor* Owner = GetOwner();

	// TODO: Add visual effects

	// Take damage sound
	if (Owner && DamageSFX) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DamageSFX, Owner->GetActorLocation());
	}
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Updates variable for health gui animation
	if (CurrentHealth > Health) {
		CurrentHealth -= 100.0f * DeltaTime;
	}
	else if (CurrentHealth < Health) {
		CurrentHealth = Health;
	}

	// Updates variable for shield gui animation
	if (CurrentShield > Shield) {
		CurrentShield -= 100.0f * DeltaTime;
	}
	else if (CurrentShield < Shield) {
		CurrentShield = Shield;
	}
}

// Process actor damage
void UHealthComponent::HandleDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) {
	if (Damage <= 0.0f) { return; }

	float Remaining = FMath::Max(0.0f, Damage - Shield);
	Shield = FMath::Max(0.0f, Shield - Damage);
	Health = FMath::Max(0.0f, Health - Remaining);

	UE_LOG(LogTemp, Warning, TEXT("[Health Update] %s  Damage: %f | Shield: %f | Health: %f"), *GetOwner()->GetName(), Damage, Shield, Health)

	OnHealthChanged.Broadcast(this, Health, Shield, Damage, DamageType, InstigatedBy, DamageCauser);
	PlayDamageEffects();
}


// Replicates properties on client
void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, Health);
	DOREPLIFETIME(UHealthComponent, Shield);
}

