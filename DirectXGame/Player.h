#pragma once
#include "AABB.h"
#include "KamataEngine.h"
#include "Matrix4x4_.h"
#include "WorldTransformClass.h"
#include <cassert>
#include "Easing.h"
#include "Operator.h"

// 02_14の29ページから再開

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
	void Initialize(KamataEngine::Model* model, uint32_t textureHandle);

	/// <summary>
	/// 更新
	/// </summary>
	void Updata();

	
	/// <summary>
	/// 描画
	/// </summary>
	void Draw(KamataEngine::Camera& viewProjection);

	

private:

	KamataEngine::Input* input_ = nullptr;
	
	KamataEngine::WorldTransform worldTransform_; // ワールド変形
	KamataEngine::Model* model_ = nullptr;        // モデル
	uint32_t textureHandle_ = 0;                  // テクスチャハンドル

};
