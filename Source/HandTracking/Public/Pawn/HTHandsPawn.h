// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTTypes.h"
#include "MotionControllerComponent.h"
#include "OculusXRHandComponent.h"
#include "GameFramework/Pawn.h"
#include "HTHandsPawn.generated.h"

class UCameraHandInput;
class UHandPoseRecognizer;
class UHandGestureRecognizer;
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

	virtual void GetControllerVelocitiesAveraged(FVector& OutLeftControllerVelocity, FVector& OutRightControllerVelocity);

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UHandPoseRecognizer* HandPoseRecognizerLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UHandPoseRecognizer* HandPoseRecognizerRight;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UHandGestureRecognizer* HandGestureRecognizerLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UHandGestureRecognizer* HandGestureRecognizerRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UCameraHandInput* CameraHandInputLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UCameraHandInput* CameraHandInputRight;	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = HandTrackingSettings)
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceChannels;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = HandTrackingSettings)
	float TraceSphereRadius = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = HandTrackingSettings)
	float VelocitySamples = 30;

	UPROPERTY(EditDefaultsOnly, Category = HandTrackingSettings)
	FKey LogLeftHandPoseKey;

	UPROPERTY(EditDefaultsOnly, Category = HandTrackingSettings)
	FKey LogRightHandPoseKey;
	
	UPROPERTY(BlueprintReadOnly, Category = HandTrackingSettings)
	FVector LeftControllerVelocityRunningAverage = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = HandTrackingSettings)
	FVector RightControllerVelocityRunningAverage = FVector::ZeroVector;

private:
	
	static bool AreGrabRequirementsMet(const FFingerCollisionData& FingerCollision);

	void TraceAndGrabFromHand(UOculusXRHandComponent* HandComponent, UMotionControllerComponent* ControllerComponent, FFingerCollisionData& FingerCollision, TArray<AActor*>& HeldActors);
	void DoFeedback(ETargetHand Hand, ETargetHandLocation Location, float Duration) const;
	bool TraceFinger(UOculusXRHandComponent* HandComponent, FName SocketName, const TArray<AActor*>& IgnoredActors, bool bDrawDebug, FHitResult& OutResult) const;

	void TryGrabItem(UOculusXRHandComponent* HandComponent, UMotionControllerComponent* ControllerComponent,
		FFingerCollisionData& FingerCollision, ETargetHandLocation Location, const FHitResult& HitResult, TArray<AActor*>& HeldActors);

	void TryGrabItemFromPose(UMotionControllerComponent* ControllerComponent, UOculusXRHandComponent* HandComponent, TArray<AActor*>& HeldActors);
	
	void TryReleaseItem(UOculusXRHandComponent* HandComponent, UMotionControllerComponent* ControllerComponent,
		FFingerCollisionData& FingerCollision, TArray<AActor*>& HeldActors);

	void ProcessHandLogInput();

private:

	UPROPERTY()
	TArray<AActor*> HeldActorsRightHand;

	UPROPERTY()
	TArray<AActor*> HeldActorsLeftHand;

	FString LastLeftPoseRecognised = FString("");
	FString LastRightPoseRecognised = FString("");

	FFingerCollisionData FingerCollisionLeft;
	FFingerCollisionData FingerCollisionRight;

};
