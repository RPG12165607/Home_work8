#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SpartaGameState.generated.h"


UCLASS()
class SPARTAPROJECT_API ASpartaGameState : public AGameState
{
    GENERATED_BODY()

public:
    ASpartaGameState();

    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
    int32 Score;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
    int32 SpawnedCoinCount;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
    int32 CollectedCoinCount;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
    float LevelDuration;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
    int32 CurrentLevelIndex;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
    int32 MaxLevels;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
    TArray<FName> LevelMapNames;

    FTimerHandle LevelTimerHandle;
    FTimerHandle WaveTimerHandle; //웨이브 타이머
    FTimerHandle HUDUpdateTimerHandle;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
    int32 CurrentWaveIndex;  // 현재 웨이브
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
    int32 MaxWaveCount;  // 웨이브 개수
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
    float WaveDuration;  // 웨이브 시간

    UFUNCTION(BlueprintPure, Category = "Score")
    int32 GetScore() const;
    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddScore(int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Level")
    void OnGameOver();

    void StartLevel();
    void OnLevelTimeUp();
    void OnCoinCollected();
    void EndLevel();
    void UpdateHUD();

    void StartWave();  // 웨이브 시작
    void OnWaveTimeUp();  // 웨이브 타이머
    void EndWave();  // 웨이브 종료
};
