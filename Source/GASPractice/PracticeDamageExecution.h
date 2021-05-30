// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagsModule.h"
#include "GameplayEffectExecutionCalculation.h"
#include "PlayerAttributes.h"
#include "PracticeDamageExecution.generated.h"




/**
* 
*/
//We need a struct to capture the values from our attribute data table
struct AttributeStruct
{
	//Declare the values
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageMod);
	//make a constructor for the struct where we define the values
AttributeStruct()
{
	//the place where the values are stored,the value,the targets(health),Take a snapshot(true) or get the values of the execution After it actually executes(False)
	DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributes,Health,Target,false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributes,DamageMod,Source,false);
}
};
/**
 * 
 */
UCLASS()
class GASPRACTICE_API UPracticeDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_UCLASS_BODY()
public:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParameters,OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput)const override ;
};
