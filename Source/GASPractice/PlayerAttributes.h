// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "PlayerAttributes.generated.h"

/**
 * 
 */
UCLASS()
class GASPRACTICE_API UPlayerAttributes : public UAttributeSet
{
	GENERATED_BODY()

	public:
	UPROPERTY(Category= "Abilities | Stats", EditAnywhere,BlueprintReadWrite)
	FGameplayAttributeData Speed;

	UPROPERTY(Category= "Abilities | Stats", EditAnywhere,BlueprintReadWrite)
	FGameplayAttributeData Health;

	UPROPERTY(Category= "Abilities | Stats", EditAnywhere,BlueprintReadWrite)
	FGameplayAttributeData DamageMod;

	UFUNCTION(Category= "Attribute functions")
	FGameplayAttribute SpeedAttribute();
	
	UFUNCTION(Category= "Attribute functions")
FGameplayAttribute HealthAttribute();

	UFUNCTION(Category= "Attribute functions")
FGameplayAttribute DamageModAttribute();
	
};
