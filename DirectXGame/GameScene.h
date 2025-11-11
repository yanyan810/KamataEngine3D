#pragma once
#include "CameraController.h"
#include "CollisionManager.h"
#include "Enemy.h"
#include "EnemyBullet.h"
#include "Fade.h"
#include "Goal.h"
#include "KamataEngine.h"
#include "Player.h"
#include "Skydome.h"

// ゲームシーン
class GameScene {

public:
	// 初期化
	void Initialize();

	// デストラクタ
	~GameScene();

	// 更新
	void Update();

	// 描画
	void Draw();

	/// <summary>
	/// 衝突判定と応答
	/// </summary>
	void CheckAllCollisions();

	// void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

	bool IsFinished() const { return finished_; } // ← 追加：メイン側遷移用

	void SpawnWaveInLockArea(int count);
	bool AnyEnemyAlive() const;

private:
	enum class GamePhase { kPlay, kFadeOutOnGoal };

	GamePhase phase_ = GamePhase::kPlay;

	bool finished_ = false;

	Fade* fade_ = nullptr;

	Goal goal_;

	Player* player_ = nullptr;                  // プレイヤーのインスタンス
	KamataEngine::Model* playerModel = nullptr; // プレイヤーのモデル
	uint32_t playerTextureHandle_ = 0;          // プレイヤーのテクスチャハンドル

	std::vector<std::unique_ptr<Enemy>> enemies_;
	KamataEngine::Model* enemyModel_ = nullptr;
	uint32_t enemyTextureHandle_ = 0;

	KamataEngine::Camera viewProjection_;
	KamataEngine::WorldTransform worldTransform_;

	bool isDebugCameraActive_ = false;

	// デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	KamataEngine::Input* input_ = nullptr;

	CollisionManager* collisionManager_ = nullptr;

	KamataEngine::Model* modelSkydome_ = nullptr; // 天球のモデル

	Skydome* skydome_ = nullptr; // 天球

	CameraController* cameraController = nullptr;

	// ロック領域（例：X ∈ [lockMinX_, lockMaxX_] に閉じ込める）
	bool lockActive_ = false;
	bool lockEngaged_ = false; // 一度入ったら解除まで維持
	float lockMinX_ = 100.0f;  // ★好みで
	float lockMaxX_ = 300.0f;  // ★好みで

	// 解除後に戻すカメラ範囲の保存
	struct {
		float left, right, bottom, top;
	} prevCamArea_{-10.0f, 10000.0f, -50.0f, 50.0f};

};