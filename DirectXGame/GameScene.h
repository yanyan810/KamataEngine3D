#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include "Skydome.h"
#include <vector>
#include "MapChipFiled.h"
#include "CameraController.h"
#include "Enemy.h"

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

	void GenerateBlocks();

	// スプライト
	KamataEngine::Sprite* sprite_ = nullptr;
	KamataEngine::Sprite* playerSprite_ = nullptr;

	// 3Dモデル
	KamataEngine::Model* playerModel_ = nullptr;
	KamataEngine::Model* enemyModel_ = nullptr;
	// ブロックモデル
	KamataEngine::Model* modelBlock_ = nullptr;
	;

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;

	// カメラ
	KamataEngine::Camera camera_;

	// デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	Player* player_ = nullptr;

	Enemy* enemy_ = nullptr;

	Skydome* skydome_ = nullptr;

	CameraController* cameraController_ = nullptr;

	MapChipField* mapChipField_ ;

	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	KamataEngine::Matrix4x4 matrix_;

	KamataEngine::Sprite* maskSprite_ = nullptr;

	KamataEngine::Sprite* blackSprite_ = nullptr;

	KamataEngine::Model* modelSkydome_ = nullptr;

private:
	uint32_t textureHandle_ = 0; // テクスチャハンドル

	uint32_t playerTextureHandle_ = 0; // プレイヤーテクスチャハンドル

	uint32_t enemyTextureHandle_ = 0; // 敵のテクスチャハンドル

	uint32_t soundDataHandle_ = 0; // サウンドデータハンドル

	uint32_t voiceHandle_ = 0; // 音声ハンドル

	float inputFloat3[3] = {0.0f, 0.0f, 0.0f}; // ImGui用のfloat配列

	// Matrix4x4 viewMatrix;

	bool isDebugCameraActive_ = false;
};