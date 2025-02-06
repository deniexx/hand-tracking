// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/HTTaskActor.h"

void AHTTaskActor::Interaction1_Implementation()
{
	if (HasAnyGrabPoses())
	{
		return;
	}

	OnGrabbed.Broadcast(this);
}

void AHTTaskActor::Interaction2_Implementation()
{
	if (HasAnyGrabPoses())
	{
		OnGrabbed.Broadcast(this);
	}
}

void AHTTaskActor::Interaction3_Implementation()
{
	OnDropped.Broadcast(this);
}

bool AHTTaskActor::HasAnyGrabPoses() const
{
	switch (GrabHand)
	{
	case EGrabHand::None:
		return false;
	case EGrabHand::Left:
		return GrabPosesLeftHand.Num() > 0;
	case EGrabHand::Right:
		return GrabPosesRightHand.Num() > 0;
	}

	/** Should never be hit, but it stops Rider from complaining */
	return false;
}
