
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpartaCharacter.generated.h"

class USpringArmComponent; // ������ �� ���� Ŭ���� ���� ����
class UCameraComponent; // ī�޶� ���� Ŭ���� ���� ����
class UWidgetComponent;
struct FInputActionValue; // Enhanced Input���� �׼� ���� ���� �� ����ϴ� ����ü

UCLASS()
class SPARTAPROJECT_API ASpartaCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ASpartaCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp; // ������ �� ������Ʈ

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp; // ī�޶� ������Ʈ

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverheadWidget;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;
	UFUNCTION(BlueprintCallable, Category = "Health")

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(
		float DamageAmount, 
		struct FDamageEvent const& DamageEvent, 
		AController* EventInstigator, 
		AActor* DamageCauser) override;

public:	
	float NormalSpeed; // ���� �ӵ�
	float SprintSpeedMultiplier; // ���� �ӵ�
	float SprintSpeed; // ������ ������ �ӵ�

	UFUNCTION() // ��������Ϳ��� ������ �ʿ䰡 ��� ���縸 �˷���
	void Move(const FInputActionValue& value); // ������ ���� Ÿ���� ������ (bool, vater2D, vater3D��)
	UFUNCTION()
	void StartJump(const FInputActionValue& value); // Jump�� ���������� �������� ���� �ΰ��� ��찡 �ٸ��� �־
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& value);

	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual void OnDeath();
	void UpdateOverheadHP();

};
