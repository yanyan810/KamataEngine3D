#pragma once
#include "KamataEngine.h"
#include <cassert>

/// <summary>
/// 自キャラ
/// </summary>
class Player {

	public:

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="model">モデル</param>
		/// <param name="textureHandle">テクスチャハンドル</param>
	    void Initialize(KamataEngine::Model* model, uint32_t textureHandle,KamataEngine::Camera* camera);

		/// <summary>
		/// 更新
		/// </summary>
		void Updata();

		/// <summary>
		/// 描画
		/// </summary>
		void Draw();

		private:
	    KamataEngine::WorldTransform worldTransform_;

		KamataEngine::Model* model_ = nullptr;

		KamataEngine::Camera* camera_ = nullptr;

		uint32_t textureHandle_ = 0u;

};

