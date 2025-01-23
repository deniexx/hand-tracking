// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTTypes.h"
#include "GameFramework/Pawn.h"
#include "HTHandsPawn.generated.h"

class USphereComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UOculusXRHandComponent;

USTRUCT()
struct FFingerCollisionData
{
	GENERATED_BODY()

	uint8 FingersColliding = 0;

	UPROPERTY()
	AActor* HitActor = nullptr;;

	void Reset()
	{
		FingersColliding = 0;
		HitActor = nullptr;
	}
};

UCLASS()
class HANDTRACKING_API AHTHandsPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AHTHandsPawn();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UMotionControllerComponent* MotionControllerLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UOculusXRHandComponent* OculusXRHandLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UMotionControllerComponent* MotionControllerRight;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UOculusXRHandComponent* OculusXRHandRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = HandTrackingSettings)
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceChannels;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = HandTrackingSettings)
	float TraceSphereRadius = 1.f;

private:

	static bool AreGrabRequirementsMet(const FFingerCollisionData& FingerCollision);

	void DoFeedback(ETargetHand Hand, ETargetHandLocation Location, float Duration) const;
	bool TraceFinger(UOculusXRHandComponent* HandComponent, FName SocketName, const TArray<AActor*>& IgnoredActors, bool bDrawDebug, FHitResult& OutResult) const;
	void TryGrabItem(FFingerCollisionData& FingerCollision, ETargetHandLocation Location, const FHitResult& HitResult, TArray<AActor*>& HeldActors) const;
	void TryReleaseItem(FFingerCollisionData& FingerCollision, TArray<AActor*>& HeldActors);

private:

	UPROPERTY()
	TArray<AActor*> HeldActorsRightHand;

	UPROPERTY()
	TArray<AActor*> HeldActorsLeftHand;

	FFingerCollisionData FingerCollisionLeft;
	FFingerCollisionData FingerCollisionRight;

};
