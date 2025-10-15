
#include "SpartaCharacter.h"
#include "SpartaPlayerController.h"
#include "SpartaGameState.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h" // ī�޶� ���� ������ �ʿ��� ���� include
#include "GameFramework/SpringArmComponent.h" // ������ �� ���� ������ �ʿ��� ���� include
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"

ASpartaCharacter::ASpartaCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

    // (1) ������ �� ����
    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    // ������ ���� ��Ʈ ������Ʈ (CapsuleComponent)�� ����                         
    SpringArmComp->SetupAttachment(RootComponent);
    // ĳ���Ϳ� ī�޶� ������ �Ÿ� �⺻�� 300���� ����                             
    SpringArmComp->TargetArmLength = 300.0f; ;
    // ��Ʈ�ѷ� ȸ���� ���� ������ �ϵ� ȸ���ϵ��� ����                            
    SpringArmComp->bUsePawnControlRotation = true;                                 
                                                                                   
    // (2) ī�޶� ������Ʈ ����                                                    
    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));         
    // ������ ���� ���� ��ġ�� ī�޶� ����                                       
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
    // ī�޶�� ������ ���� ȸ���� �����Ƿ� PawnControlRotation�� ����
    CameraComp->bUsePawnControlRotation = false;

    OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
    OverheadWidget->SetupAttachment(GetMesh());
    OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);

    NormalSpeed = 600.0f; // ���� �ӵ�
    SprintSpeedMultiplier = 1.7f; // ���� �ӵ�
    SprintSpeed = NormalSpeed * SprintSpeedMultiplier; // ������ ������ �ӵ�

    GetCharacterMovement()->MaxWalkSpeed = NormalSpeed; // MaxWalkSpeed -> ĳ������ �̵��ӵ�
    
    MaxHealth = 100.0f;
    Health = MaxHealth;
}

void ASpartaCharacter::BeginPlay()
{
	Super::BeginPlay();
    UpdateOverheadHP();
}

void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
        // PlayerInputComponent�� �پ��� ����� �־ EnhancedInput ������� Cast �Ѵٸ� �ѹ� �����ϴ°�
    {
        if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
            // ĳ���͸� �����ϴ� Controller�� �������� �װ� SpartaPlayerController�� Cast -> PlayerController�� ������ ����
        {
            if (PlayerController->MoveAction)
            {
                EnhancedInput->BindAction( PlayerController->MoveAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::Move);
                // EnhancedInput->BindAction -> �̺�Ʈ�� �Լ��� �����ϴ� �ٽ� �ڵ�
                // PlayerController->MoveAction -> >MoveAction�� �����´�
                // ETriggerEvent::Triggered -> Ű�� ������ �̺�Ʈ�� �߻� ������
                // this -> ȣ��� �Լ��� ��ü ( Character�� ��ü )
                // &ASpartaCharacter::Move -> ������ ȣ��� �Լ�
            }
            if (PlayerController->JumpAction)
            {
                EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::StartJump);
                // Jump�� ��� ���������� ������ �ΰ��� ����̱⿡ ������ �ۼ�
            }
            if (PlayerController->JumpAction)
            {
                EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Completed, this, &ASpartaCharacter::StopJump);
                // ETriggerEvent::Completed -> Ű�� ���� �̺�Ʈ�� �߻�������
            }
            if (PlayerController->LookAction)
            {
                EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::Look);
            }
            if (PlayerController->SprintAction)
            {
                EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::StartSprint);
            }
            if (PlayerController->SprintAction)
            {
                EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Completed, this, &ASpartaCharacter::StopSprint);
            }
        }
    }
}

void ASpartaCharacter::Move(const FInputActionValue& value)
{
    if (!Controller) return; // Controller�� ��ȿ���� Ȯ�� (ai�� ���� ���۵� ���� ����)

    const FVector2D MoveInput = value.Get<FVector2D>();
    // value���� FVector2D�� �����ͼ� MoveInput�� FVector2D ���·� ����

    if (!FMath::IsNearlyZero(MoveInput.X))
        // IsNearlyZero() -> �Է°��� 0�� ������ �������� 0���� ���
        // �տ� ! �� ������ ���� 0�� �ƴҶ� �۵� �ϴ� �����ڵ�
    {
        AddMovementInput(GetActorForwardVector(), MoveInput.X);
        // GetActorForwardVector() -> ĳ������ ������� ����
        // AddMovementInput(GetActorForwardVector(), MoveInput.X) -> GetActorForwardVector()�� MoveInput.X ��ŭ �̵�
    }
    if (!FMath::IsNearlyZero(MoveInput.Y))
    {
        AddMovementInput(GetActorRightVector(), MoveInput.Y);
        // GetActorRightVector() -> ĳ������  �����ʹ��� ����
    }
}
void ASpartaCharacter::StartJump(const FInputActionValue& value)
{
    if (value.Get<bool>()) // Jump�� bool Ÿ���̶� ���� ������ �ʿ�x
        // value.Get<bool>() -> ���϶� �۵�
    {
        Jump(); // CharacterŬ�������� �����ϴ� �Լ�
    }
}
void ASpartaCharacter::StopJump(const FInputActionValue& value)
{
    if (!value.Get<bool>())
        // !value.Get<bool>() -> �����϶� �۵�
    {
        StopJumping(); // CharacterŬ�������� �����ϴ� �Լ�
    }
}
// Jump���� if (!Controller) return; �� ���� ���� 
// 1. Jump(), StopJumping() �⺻ �����Ǵ� �Լ��� ���ԵǾ�����
// 2. Move���� GetActorForwardVector()���� �Լ��� �����ö� Controller�� �ʿ��� ������ �۵��� �ȵǱ� ���� �׷��� �ۿ��� ��ȿ���� Ȯ��
void ASpartaCharacter::Look(const FInputActionValue& value)
{
    FVector2D LookInput = value.Get<FVector2D>();

    AddControllerYawInput(LookInput.X); // X���� ����� ������ ������ �������� ȸ��
    // Yaw -> Y�� ȸ������ ���´�
    // AddControllerYawInput() -> ���� ���� �¿�� ȸ���Ѵ�
    AddControllerPitchInput(LookInput.Y);
    // Pitch -> X�� ȸ������ ���´�
}
void ASpartaCharacter::StartSprint(const FInputActionValue& value)
{
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
    }
}
void ASpartaCharacter::StopSprint(const FInputActionValue& value)
{
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
    }
}

float ASpartaCharacter::GetHealth() const
{
    return Health;
}

void ASpartaCharacter::AddHealth(float Amount)
{
    Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
    UpdateOverheadHP();
}

float ASpartaCharacter::TakeDamage(
    float DamageAmount, 
    FDamageEvent const& DamageEvent, 
    AController* EventInstigator, 
    AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
    UpdateOverheadHP();

    if (Health <= 0.0f)
    {
        OnDeath();
    }

    return ActualDamage;
}

void ASpartaCharacter::OnDeath()
{
    ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
    if (SpartaGameState)
    {
        SpartaGameState->OnGameOver();
    }
}

void ASpartaCharacter::UpdateOverheadHP()
{
    if (!OverheadWidget) return;

    UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
    if (!OverheadWidgetInstance) return;

    if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP"))))
    {
        HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
    }
}