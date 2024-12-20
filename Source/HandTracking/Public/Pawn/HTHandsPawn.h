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

USTRUCT(BlueprintType)
struct FHandSphereList
{
	GENERATED_BODY();

	TArray<USphereComponent*> Spheres;
};

typedef TMap<ETargetHandLocation, AActor*> ActorGrabMap;

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

	void DoFeedback(ETargetHand Hand, ETargetHandLocation Location, float Duration) const;
	bool TraceFinger(UOculusXRHandComponent* HandComponent, FName SocketName, const TArray<AActor*>& IgnoredActors, bool bDrawDebug, FHitResult& OutResult) const;
	void TryGrabItem(ActorGrabMap& GrabMap, ETargetHandLocation Location, const FHitResult& HitResult, TArray<AActor*>& HeldActors) const;
	void TryReleaseItem(ActorGrabMap& GrabMap, ETargetHandLocation Location, TArray<AActor*>& HeldActors);

private:

	TArray<AActor*> HeldActorsRightHand;
	TArray<AActor*> HeldActorsLeftHand;
	ActorGrabMap GrabMapLeft;
	ActorGrabMap GrabMapRight;

};
