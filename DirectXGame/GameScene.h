#pragma once
#include "KamataEngine.h"
#include "Player.h"
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

	

private:
	Player* player_ = nullptr;                  // プレイヤーのインスタンス
	KamataEngine::Model* playerModel = nullptr; // プレイヤーのモデル
	uint32_t playerTextureHandle_ = 0;          // プレイヤーのテクスチャハンドル

	Enemy* enemy_ = nullptr;
	KamataEngine::Model* enemyModel_ = nullptr;
	uint32_t enemyTextureHandle_ = 0;

	KamataEngine::Camera viewProjection_;
	KamataEngine::WorldTransform worldTransform_;

	bool isDebugCameraActive_ = false;

	// デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	KamataEngine::Input* input_ = nullptr;

};