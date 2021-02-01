#pragma once

#include "GameFramework/Pawn.h"
#include "FGPlayer.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UFGMovementComponent;
class UStaticMeshComponent;
class USphereComponent;
class UFGPlayerSettings;
class UFGNetDebugWidget;
class AFGRocket;
class AFGPickup;

//TimeStamp: 44:40
UCLASS()
class FGNET_API AFGPlayer : public APawn
{
	GENERATED_BODY()
public:
	AFGPlayer();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = Settings)
		UFGPlayerSettings* PlayerSettings = nullptr;

	UFUNCTION(BlueprintPure)
		bool IsBraking() const { return bBrake; }

	UFUNCTION(BlueprintPure)
		int32 GetPing() const;

	UPROPERTY(EditAnywhere, Category = Debug)
		TSubclassOf<UFGNetDebugWidget> DebugMenuClass;

	UFUNCTION(Server, Unreliable)
		void Server_SendLocation(const FVector& Location);

	void OnPickup(AFGPickup* Pickup);

	UFUNCTION(Server, Reliable)
		void Server_OnPickup(AFGPickup* Pickup);

	UFUNCTION(Client, Reliable)
		void Client_OnPickupRockets(int32 PickedUpRockets);

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_OnPickupRockets(int32 PickedUpRockets);

	UFUNCTION(NetMulticast, Unreliable)
		void Multicast_SendLocation(const FVector& Location);

	UFUNCTION(Server, Unreliable)
		void Server_SendRotation(const FRotator& Rotation);

	UFUNCTION(NetMulticast, Unreliable)
		void Multicast_SendRotation(const FRotator& Rotation);

	void ShowDebugMenu();
	void HideDebugMenu();

	UFUNCTION(BlueprintPure)
		int32 GetNumRockets() const { return NumRockets; }

	UFUNCTION(BlueprintImplementableEvent, Category = Player, meta = (DisplayName = "On Num Rockets Changed"))
		void BP_OnNumRocketsChanged(int32 NewNumRockets);

	UFUNCTION(BlueprintImplementableEvent, Category = Player, meta = (DisplayName = "On Health Changed"))
		void BP_OnHealthChanged(int32 NewHealthValue);

	int32 GetNumActiveRockets() const;

	void FireRocket();

	void SpawnRockets();

	void ApplyDamage(int32 DamageValue);

	UFUNCTION(Server, Reliable)
		void Server_OnTakeDamage(int32 DamageValue);

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_OnTakeDamage(int32 DamageValue);

private:

	int32 ServerHealth = 0;

	int32 Health = 0;

	int32 ServerNumRockets = 0;

	int32 NumRockets = 0;

	FVector GetRocketStartLocation() const;

	AFGRocket* GetFreeRocket() const;

	UFUNCTION(Server, Reliable)
		void Server_FireRocket(AFGRocket* NewRocket, const FVector& RocketStartLocation, const FRotator& RocketFacingRotation);

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_FireRocket(AFGRocket* NewRocket, const FVector& RocketStartLocation, const FRotator& RocketFacingRotation);

	UFUNCTION(Client, Reliable)
		void Client_RemoveRocket(AFGRocket* RocketToRemove);

	UFUNCTION(BlueprintCallable)
		void Cheat_IncreaseRockets(int32 InNumRockets);

	UPROPERTY(Replicated, Transient)
		TArray<AFGRocket*> RocketInstances;

	UPROPERTY(EditAnywhere, Category = Weapon)
		TSubclassOf<AFGRocket> RocketClass;

	int32 MaxActiveRockets = 3;

	float FireCooldownElapsed = 0.0f;

	UPROPERTY(EditAnywhere, Category = Weapon)
		bool bUnlimitedRockets = false;

	void Handle_Accelerate(float Value);
	void Handle_Turn(float Value);
	void Handle_BrakePressed();
	void Handle_BrakeReleased();

	void Handle_DebugMenuPressed();

	void Handle_FirePressed();

	void CreateDebugWidget();

	UPROPERTY(Transient)
		UFGNetDebugWidget* DebugMenuInstance = nullptr;

	bool bShowDebugMenu = false;

	float Forward = 0.0f;
	float Turn = 0.0f;

	float MovementVelocity = 0.0f;
	float Yaw = 0.0f;

	bool bBrake = false;

	float InterpolationSpeed = 10.0f;

	FVector TargetLocation;
	FRotator TargetRotation;

	UPROPERTY(VisibleDefaultsOnly, Category = Collision)
		USphereComponent* CollisionComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Camera)
		USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Camera)
		UCameraComponent* CameraComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Movement)
		UFGMovementComponent* MovementComponent;
};