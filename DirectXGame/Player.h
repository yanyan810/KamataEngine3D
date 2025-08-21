#pragma once
#include "AABB.h"
#include "KamataEngine.h"
#include "Matrix4x4_.h"
#include "WorldTransformClass.h"
#include <cassert>
#include "Easing.h"
#include "Operator.h"
#include "PlayerBullet.h"
// 02_14の29ページから再開

/// <summary>
/// 自キャラ
/// </summary>
class Player {

public:

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();
	
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

	/// <summary>
	/// 回転
	/// </summary>
	void Rotate();

	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();

	KamataEngine::Vector3 GetPosition();

	// 衝突を検知したら曜日出されるコールバック関数
	void OnCollision();

		// 弾リストを取得
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }

	float GetRadius() const { return 1.0f; } // 自キャラの半径（仮値）

private:

		// 弾
	std::list<PlayerBullet*> bullets_;

	KamataEngine::Input* input_ = nullptr;
	
	KamataEngine::WorldTransform worldTransform_; // ワールド変形
	KamataEngine::Model* model_ = nullptr;        // モデル
	uint32_t textureHandle_ = 0;                  // テクスチャハンドル


};
