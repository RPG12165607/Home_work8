#include "SpartaGameState.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

ASpartaGameState::ASpartaGameState()
{
    Score = 0;
    SpawnedCoinCount = 0;
    CollectedCoinCount = 0;
    LevelDuration = 30.0f;
    CurrentLevelIndex = 0;
    MaxLevels = 3;

    CurrentWaveIndex = 0;  // 0��° ���̺�
    MaxWaveCount = 3;  // ���̺� 3��
    WaveDuration = 10.0f;  // ���̺�ð� 10��
}

void ASpartaGameState::BeginPlay()
{
    Super::BeginPlay();

    StartLevel();

    GetWorldTimerManager().SetTimer(
        HUDUpdateTimerHandle,
        this,
        &ASpartaGameState::UpdateHUD,
        0.1f,
        true
    );
}

int32 ASpartaGameState::GetScore() const
{
    return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
        if (SpartaGameInstance)
        {
            SpartaGameInstance->AddToScore(Amount);
        }
    }
}

void ASpartaGameState::StartLevel()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
        {
            SpartaPlayerController->ShowGameHUD();
        }
    }

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
        if (SpartaGameInstance)
        {
            CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
        }
    }

    StartWave();  // ���̺� ����
}

void ASpartaGameState::StartWave()
{
    CurrentWaveIndex++;  // ���� ���̺� ī��Ʈ
    SpawnedCoinCount = 0;
    CollectedCoinCount = 0;

    UE_LOG(LogTemp, Warning, TEXT("Wave %d ����!"), CurrentWaveIndex);  // ���̺� ���� �˸�

    TArray<AActor*> FoundVolumes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

    const int32 ItemToSpawn = CurrentWaveIndex * 40;  // ���̺� �� ������ ���� ��ȭ

    for (int32 i = 0; i < ItemToSpawn; i++)
    {
        if (FoundVolumes.Num() > 0)
        {
            ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
            if (SpawnVolume)
            {
                AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
                if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
                {
                    SpawnedCoinCount++;
                }
            }
        }
    }
    // ���̺� Ÿ�̸�
    GetWorldTimerManager().SetTimer(
        WaveTimerHandle,
        this,
        &ASpartaGameState::OnWaveTimeUp,
        WaveDuration,
        false
    );
}

void ASpartaGameState::OnWaveTimeUp()
{
    EndWave();
}

void ASpartaGameState::EndWave()
{
    GetWorldTimerManager().ClearTimer(WaveTimerHandle);

    if (CurrentWaveIndex < MaxWaveCount)
    {
        StartWave();
    }
    else
    {
        EndLevel();
    }
}

void ASpartaGameState::OnLevelTimeUp()
{
    EndLevel();
}

void ASpartaGameState::OnCoinCollected()
{
    CollectedCoinCount++;

    UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"), 
        CollectedCoinCount,
        SpawnedCoinCount)

        if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
        {
            EndLevel();
        }
}

void ASpartaGameState::EndLevel()
{
    GetWorldTimerManager().ClearTimer(LevelTimerHandle);
    GetWorldTimerManager().ClearTimer(WaveTimerHandle);

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
        if (SpartaGameInstance)
        {
            AddScore(Score);
            CurrentLevelIndex++;
            SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;

            if (CurrentLevelIndex >= MaxLevels)
            {
                OnGameOver();
                return;
            }

            if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
            {
                UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
            }
            else
            {
                OnGameOver();
            }
        }
    }

}

void ASpartaGameState::OnGameOver()
{
    if (APlayerController* PlayerController = GetWorld() -> GetFirstPlayerController())
    {
        if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
        {
            SpartaPlayerController->SetPause(true);
            SpartaPlayerController -> ShowMainMenu(true);
        }
    }
}

void ASpartaGameState::UpdateHUD()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
        {
            if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
            {
                if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
                {
                    float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
                    TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
                }
                if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
                {
                    if (UGameInstance* GameInstance = GetGameInstance())
                    {
                        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
                        if (SpartaGameInstance)
                        {
                            ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
                        }
                    }
                }

                if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
                {
                    LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1)));
                }

                if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))  // ���̺� ���
                {
                    WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d / %d"), CurrentWaveIndex, MaxWaveCount)));
                }
            }
        }
    }
}