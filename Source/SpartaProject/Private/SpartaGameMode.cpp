

#include "SpartaGameMode.h"
#include "SpartaCharacter.h" // SpartaCharacter헤드 추가
#include "SpartaPlayerController.h"
#include "SpartaGameState.h"

ASpartaGameMode::ASpartaGameMode()
{
    DefaultPawnClass = ASpartaCharacter::StaticClass();
    // DefaultPawnClass -> 게임이 시작할 때 플레이어가 자동으로 스폰받는 Pawn의 클래스 타입을 담는 변수
    // ASpartaCharacter -> 캐릭터 클래스
    // ::StaticClass() -> 클래스를 반환해주는 함수
    PlayerControllerClass = ASpartaPlayerController::StaticClass();
    GameStateClass = ASpartaGameState::StaticClass();
}
