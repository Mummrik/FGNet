#include "FGPlayer.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/NetDriver.h"
#include "GameFramework/PlayerState.h"
#include "../Components//FGMovementComponent.h"
#include "../FGMovementStatics.h"

AFGPlayer::AFGPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(CollisionComponent);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->bInheritYaw = false;
	SpringArmComponent->SetupAttachment(CollisionComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	MovementComponent = CreateDefaultSubobject<UFGMovementComponent>(TEXT("MovementComponent"));

	SetReplicateMovement(false);
}

void AFGPlayer::BeginPlay()
{
	Super::BeginPlay();

	MovementComponent->SetUpdatedComponent(CollisionComponent);
}

void AFGPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled())
	{
		const float Friction = IsBraking() ? BrakingFriction : DefaultFriction;
		const float Alpha = FMath::Clamp(FMath::Abs(MovementVelocity / (MaxVelocity * 0.75f)), 0.0f, 1.0f);
		const float TurnSpeed = FMath::InterpEaseOut(0.0f, TurnSpeedDefault, Alpha, 5.0f);
		const float MovementDirection = MovementVelocity > 0.0f ? Turn : -Turn;

		Yaw += (MovementDirection * TurnSpeed) * DeltaTime;
		FQuat WantedFacingDirection = FQuat(FVector::UpVector, FMath::DegreesToRadians(Yaw));
		MovementComponent->SetFacingRotation(WantedFacingDirection);

		FFGFrameMovement FrameMovement = MovementComponent->CreateFrameMovement();

		MovementVelocity += Forward * Acceleration * DeltaTime;
		MovementVelocity = FMath::Clamp(MovementVelocity, -MaxVelocity, MaxVelocity);
		MovementVelocity *= FMath::Pow(Friction, DeltaTime);

		MovementComponent->ApplyGravity();
		FrameMovement.AddDelta(GetActorForwardVector() * MovementVelocity * DeltaTime);
		MovementComponent->Move(FrameMovement);

		Server_SendLocation(GetActorLocation());
		Server_SendRotation(GetActorRotation());
	}
	else
	{
		SetActorLocation(FMath::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, InterpolationSpeed));
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, InterpolationSpeed));
	}
}

void AFGPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Accelerate"), this, &AFGPlayer::Handle_Accelerate);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AFGPlayer::Handle_Turn);

	PlayerInputComponent->BindAction(TEXT("Brake"), IE_Pressed, this, &AFGPlayer::Handle_BrakePressed);
	PlayerInputComponent->BindAction(TEXT("Brake"), IE_Released, this, &AFGPlayer::Handle_BrakeReleased);
}

int32 AFGPlayer::GetPing() const
{
	if (GetPlayerState())
	{
		return static_cast<int32>(GetPlayerState()->GetPing());
	}

	return 0;
}

void AFGPlayer::Server_SendLocation_Implementation(const FVector& Location)
{
	Multicast_SendLocation(Location);
}

void AFGPlayer::Multicast_SendLocation_Implementation(const FVector& Location)
{
	if (!IsLocallyControlled())
	{
		TargetLocation = Location;
	}
}

void AFGPlayer::Server_SendRotation_Implementation(const FRotator& Rotation)
{
	Multicast_SendRotation(Rotation);
}

void AFGPlayer::Multicast_SendRotation_Implementation(const FRotator& Rotation)
{
	if (!IsLocallyControlled())
	{
		TargetRotation = Rotation;
	}
}

void AFGPlayer::Handle_Accelerate(float Value)
{
	Forward = Value;
}

void AFGPlayer::Handle_Turn(float Value)
{
	Turn = Value;
}

void AFGPlayer::Handle_BrakePressed()
{
	bBrake = true;
}

void AFGPlayer::Handle_BrakeReleased()
{
	bBrake = false;
}