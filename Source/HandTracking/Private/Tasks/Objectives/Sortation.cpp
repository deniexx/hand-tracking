// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/Objectives/Sortation.h"

#include "SphereComponent.h"
#include "Actor/HTTaskActor.h"

void USortation::Activate_Implementation(UObject* InWorldContextManual)
{
	Super::Activate_Implementation(InWorldContextManual);

	TArray<AHTTaskActor*> TaskActors = GatherTaskActors();

	for (auto TaskActor : TaskActors)
	{
		USphereComponent* Sphere = TaskActor->GetComponentByClass<USphereComponent>();
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSortableSorted);
	}
}

void USortation::OnSortableSorted(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AHTTaskActor* MainActor = Cast<AHTTaskActor>(OverlappedComponent->GetOwner());
	AHTTaskActor* OtherTaskActor = Cast<AHTTaskActor>(OtherActor);
	if (MainActor && OtherTaskActor)
	{
		if (OtherTaskActor->TaskActorTag.MatchesTagExact(MainActor->OptionalTag))
		{
			// Object has been sorted correctly
		}
		else
		{
			// Object has not been sorted correctly
		}

		MainActor->Destroy();
	}
}
