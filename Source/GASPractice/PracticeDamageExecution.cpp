// Fill out your copyright notice in the Description page of Project Settings.


#include "PracticeDamageExecution.h"

#include "AbilitySystemComponent.h"

UPracticeDamageExecution::UPracticeDamageExecution(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	AttributeStruct Attributes;
//Attributes.HealthProperty returns the uproperty def returns the actual value as defined
	RelevantAttributesToCapture.Add(Attributes.HealthDef);

//does not allow the value to be modified further (doesn't show up in the menus for gameplay effects)
	InvalidScopedModifierAttributes.Add(Attributes.HealthDef);

	RelevantAttributesToCapture.Add(Attributes.DamageModDef);
	
}

//implement the execution function
void UPracticeDamageExecution::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParameters,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	AttributeStruct Attributes;

	//specify which effect
	const FGameplayEffectSpec& Spec = ExecutionParameters.GetOwningSpec();

	//ability system components of the target and the source
	UAbilitySystemComponent* TargetASC=ExecutionParameters.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* SourceASC=ExecutionParameters.GetSourceAbilitySystemComponent();

//get the target and source
AActor* TargetActor=TargetASC?TargetASC->GetAvatarActor():nullptr;
AActor* SourceActor=SourceASC?SourceASC->GetAvatarActor():nullptr;

	//Get the tag container for the target and source
const FGameplayTagContainer* TargetTags=Spec.CapturedTargetTags.GetAggregatedTags();
const FGameplayTagContainer* SourceTags=Spec.CapturedSourceTags.GetAggregatedTags();


	//
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.TargetTags=TargetTags;
	EvaluationParameters.SourceTags=SourceTags;

	//calculate the values of each attribute and set the to floats
	float Health=0.f;
	ExecutionParameters.AttemptCalculateCapturedAttributeMagnitude(Attributes.HealthDef, EvaluationParameters,Health);

	float DamageMod=0.f;
	ExecutionParameters.AttemptCalculateCapturedAttributeMagnitude(Attributes.DamageModDef, EvaluationParameters,DamageMod);

//calculate damage
	const float DamageDone = 10*DamageMod;

//if the damage brings us below 0
if(Health-DamageDone<=0.f)
{
	//Make the target die
	FGameplayEventData eventData;
	eventData.EventTag=UGameplayTagsManager::Get().RequestGameplayTag("Event.Die");
	//pass the targets ASC info on what killed it
	eventData.Instigator=SourceActor;

	eventData.Target=TargetActor;
//have the targets ASC handle the death event
	TargetASC->HandleGameplayEvent(eventData.EventTag,&eventData);
}
	//asking for the damage that has been calculated to be outputted
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(Attributes.HealthProperty,EGameplayModOp::Additive,-DamageDone));
	
}
