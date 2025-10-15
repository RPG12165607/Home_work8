
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpartaCharacter.generated.h"

class USpringArmComponent; // 스프링 암 관련 클래스 전방 선언
class UCameraComponent; // 카메라 관련 클래스 전방 선언
class UWidgetComponent;
struct FInputActionValue; // Enhanced Input에서 액션 값을 받을 때 사용하는 구조체

UCLASS()
class SPARTAPROJECT_API ASpartaCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ASpartaCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp; // 스프링 암 컴포넌트

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp; // 카메라 컴포넌트

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
	float NormalSpeed; // 지금 속도
	float SprintSpeedMultiplier; // 가속 속도
	float SprintSpeed; // 가속이 더해진 속도

	UFUNCTION() // 블루프린터에서 수정할 필요가 없어서 존재만 알려줌
	void Move(const FInputActionValue& value); // 설정한 값의 타입을 가져옴 (bool, vater2D, vater3D등)
	UFUNCTION()
	void StartJump(const FInputActionValue& value); // Jump를 눌렸을때랑 떗을때를 나눔 두가지 경우가 다를수 있어서
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
