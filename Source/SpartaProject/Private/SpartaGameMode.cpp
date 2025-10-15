

#include "SpartaGameMode.h"
#include "SpartaCharacter.h" // SpartaCharacter��� �߰�
#include "SpartaPlayerController.h"
#include "SpartaGameState.h"

ASpartaGameMode::ASpartaGameMode()
{
    DefaultPawnClass = ASpartaCharacter::StaticClass();
    // DefaultPawnClass -> ������ ������ �� �÷��̾ �ڵ����� �����޴� Pawn�� Ŭ���� Ÿ���� ��� ����
    // ASpartaCharacter -> ĳ���� Ŭ����
    // ::StaticClass() -> Ŭ������ ��ȯ���ִ� �Լ�
    PlayerControllerClass = ASpartaPlayerController::StaticClass();
    GameStateClass = ASpartaGameState::StaticClass();
}
