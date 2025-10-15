
#include "SpartaCharacter.h"
#include "SpartaPlayerController.h"
#include "SpartaGameState.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h" // 카메라 실제 구현이 필요한 경우라서 include
#include "GameFramework/SpringArmComponent.h" // 스프링 암 실제 구현이 필요한 경우라서 include
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"

ASpartaCharacter::ASpartaCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

    // (1) 스프링 암 생성
    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    // 스프링 암을 루트 컴포넌트 (CapsuleComponent)에 부착                         
    SpringArmComp->SetupAttachment(RootComponent);
    // 캐릭터와 카메라 사이의 거리 기본값 300으로 설정                             
    SpringArmComp->TargetArmLength = 300.0f; ;
    // 컨트롤러 회전에 따라 스프링 암도 회전하도록 설정                            
    SpringArmComp->bUsePawnControlRotation = true;                                 
                                                                                   
    // (2) 카메라 컴포넌트 생성                                                    
    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));         
    // 스프링 암의 소켓 위치에 카메라를 부착                                       
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
    // 카메라는 스프링 암의 회전을 따르므로 PawnControlRotation은 꺼둠
    CameraComp->bUsePawnControlRotation = false;

    OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
    OverheadWidget->SetupAttachment(GetMesh());
    OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);

    NormalSpeed = 600.0f; // 지금 속도
    SprintSpeedMultiplier = 1.7f; // 가속 속도
    SprintSpeed = NormalSpeed * SprintSpeedMultiplier; // 가속이 더해진 속도

    GetCharacterMovement()->MaxWalkSpeed = NormalSpeed; // MaxWalkSpeed -> 캐릭터의 이동속도
    
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
        // PlayerInputComponent이 다양한 방식이 있어서 EnhancedInput 기능으로 Cast 한다를 한번 선언하는것
    {
        if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
            // 캐릭터를 조종하는 Controller을 가져오고 그걸 SpartaPlayerController로 Cast -> PlayerController에 변수로 진입
        {
            if (PlayerController->MoveAction)
            {
                EnhancedInput->BindAction( PlayerController->MoveAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::Move);
                // EnhancedInput->BindAction -> 이벤트랑 함수를 연결하는 핵심 코드
                // PlayerController->MoveAction -> >MoveAction을 가져온다
                // ETriggerEvent::Triggered -> 키가 눌려서 이벤트가 발생 했을때
                // this -> 호출된 함수의 객체 ( Character의 객체 )
                // &ASpartaCharacter::Move -> 실제로 호출된 함수
            }
            if (PlayerController->JumpAction)
            {
                EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Triggered, this, &ASpartaCharacter::StartJump);
                // Jump의 경우 눌렸을때랑 떗을때 두가지 경우이기에 나눠서 작성
            }
            if (PlayerController->JumpAction)
            {
                EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Completed, this, &ASpartaCharacter::StopJump);
                // ETriggerEvent::Completed -> 키를 떼서 이벤트가 발생했을때
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
    if (!Controller) return; // Controller의 유효한지 확인 (ai에 의해 조작될 수도 있음)

    const FVector2D MoveInput = value.Get<FVector2D>();
    // value에서 FVector2D를 가져와서 MoveInput에 FVector2D 형태로 저장

    if (!FMath::IsNearlyZero(MoveInput.X))
        // IsNearlyZero() -> 입력값이 0에 가까운값이 들어왔을때 0으로 취급
        // 앞에 ! 이 있으니 값이 0이 아닐때 작동 하는 안전코드
    {
        AddMovementInput(GetActorForwardVector(), MoveInput.X);
        // GetActorForwardVector() -> 캐릭터의 전면방향 벡터
        // AddMovementInput(GetActorForwardVector(), MoveInput.X) -> GetActorForwardVector()로 MoveInput.X 만큼 이동
    }
    if (!FMath::IsNearlyZero(MoveInput.Y))
    {
        AddMovementInput(GetActorRightVector(), MoveInput.Y);
        // GetActorRightVector() -> 캐릭터의  오른쪽방향 벡터
    }
}
void ASpartaCharacter::StartJump(const FInputActionValue& value)
{
    if (value.Get<bool>()) // Jump는 bool 타입이라서 따로 저장할 필요x
        // value.Get<bool>() -> 참일때 작동
    {
        Jump(); // Character클래스에서 제공하는 함수
    }
}
void ASpartaCharacter::StopJump(const FInputActionValue& value)
{
    if (!value.Get<bool>())
        // !value.Get<bool>() -> 거짓일때 작동
    {
        StopJumping(); // Character클래스에서 제공하는 함수
    }
}
// Jump에서 if (!Controller) return; 가 없는 이유 
// 1. Jump(), StopJumping() 기본 제공되는 함수에 포함되어있음
// 2. Move에서 GetActorForwardVector()등의 함수를 가져올때 Controller가 필요함 없으면 작동이 안되기 때문 그래서 밖에서 유효한지 확인
void ASpartaCharacter::Look(const FInputActionValue& value)
{
    FVector2D LookInput = value.Get<FVector2D>();

    AddControllerYawInput(LookInput.X); // X축이 양수면 오른쪽 음수면 왼쪽으로 회전
    // Yaw -> Y축 회전값을 갖는다
    // AddControllerYawInput() -> 값에 따라 좌우로 회전한다
    AddControllerPitchInput(LookInput.Y);
    // Pitch -> X축 회전값을 갖는다
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