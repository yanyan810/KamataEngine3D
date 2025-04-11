#pragma once
#include "KamataEngine.h"

//ゲームシーン
class GameScene
{

	public:
		//初期化
	    void Initialize();

		//デストラクタ
	    ~GameScene();

		// 更新
	    void Update();

		// 描画
		void Draw();

		//スプライト
	    KamataEngine::Sprite* sprite_ = nullptr;

		// 3Dモデル
	    KamataEngine::Model* model_ = nullptr;

		// ワールドトランスフォーム
	    KamataEngine::WorldTransform worldTransform_;

		//カメラ
	    KamataEngine::Camera camera_;

	private:

		uint32_t textureHandle_ = 0; // テクスチャハンドル

		uint32_t soundDataHandle_ = 0; // サウンドデータハンドル

		uint32_t voiceHandle_ = 0; // 音声ハンドル

		float inputFloat3[3] = {0.0f, 0.0f, 0.0f}; // ImGui用のfloat配列

};