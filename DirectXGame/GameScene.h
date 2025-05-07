#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include <vector>
#include "Matrix4x4.h"


//ゲームシーン
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

	// スプライト
	KamataEngine::Sprite* sprite_ = nullptr;
	KamataEngine::Sprite* playerSprite_ = nullptr;

	// 3Dモデル
	KamataEngine::Model* model_ = nullptr;

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

	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	KamataEngine::Matrix4x4 matrix_;

	KamataEngine::Sprite* maskSprite1_ = nullptr;

	KamataEngine::Sprite* maskSprite2_ = nullptr;

	KamataEngine::Sprite* blackSprite_ = nullptr;

private:
	uint32_t textureHandle_ = 0; // テクスチャハンドル

	uint32_t soundDataHandle_ = 0; // サウンドデータハンドル

	uint32_t voiceHandle_ = 0; // 音声ハンドル

	float inputFloat3[3] = {0.0f, 0.0f, 0.0f}; // ImGui用のfloat配列

	int imageIndex = 0;

	// Matrix4x4 viewMatrix;

	bool isDebugCameraActive_ = false;

	uint32_t bowserMaskTextureHandle_[10]; // マスク画像
	float maskFadeRate_ = 1.5f;
	bool isGameOverFade_ = false;
};