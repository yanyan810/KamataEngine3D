#pragma once
#include "KamataEngine.h"
#include "Player.h"

enum class Phase { 
	kFadeIn, 
	kPlay, 
	kDeath,
	kFadeOut
};



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
	
	Player* player_=nullptr;                              // プレイヤーのインスタンス
	KamataEngine::Model* playerModel = nullptr; // プレイヤーのモデル
	uint32_t playerTextureHandle_ = 0;                          // プレイヤーのテクスチャハンドル
	
	KamataEngine::Camera viewProjection_;
	KamataEngine::WorldTransform worldTransform_;

	bool isDebugCameraActive_ = false;

	//デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	KamataEngine::Input* input_ = nullptr;

};