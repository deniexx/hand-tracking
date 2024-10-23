// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

UCLASS()
class HANDTRACKING_API AHTHandsPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AHTHandsPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnRightHandSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnRightHandSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex);

	UFUNCTION()
	void OnLeftHandSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnLeftHandSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex);
	
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
	USphereComponent* LeftThumbTipSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	USphereComponent* LeftIndexTipSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	USphereComponent* LeftMiddleTipSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	USphereComponent* LeftRingTipSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	USphereComponent* LeftPinkyTipSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	USphereComponent* RightThumbTipSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	USphereComponent* RightIndexTipSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	USphereComponent* RightMiddleTipSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	USphereComponent* RightRingTipSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	USphereComponent* RightPinkyTipSphere;

private:

	UPROPERTY()
	TArray<USphereComponent*> LeftHandSpheres;

	UPROPERTY()
	TArray<USphereComponent*> RightHandSpheres;

	UPROPERTY()
	TMap<AActor*, FHandSphereList> ActorsToPointOfContacts;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
