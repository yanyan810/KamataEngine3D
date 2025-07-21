#pragma once
#include "KamataEngine.h"
#include "WorldTransformClass.h"
#include "Operator.h"
class PlayerBullet {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="position">初期座標</param>
	void Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position,const KamataEngine::Vector3& velocity);

	/// <summary>
	/// 更新
	/// </summary>
	void Updata();


	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(const KamataEngine::Camera& camera);

	bool IsDead() const { return isDead_; }

private:

	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	uint32_t textureHandle_ = 0;
	KamataEngine::Vector3 velosity_;
	static const int32_t kLifeTime = 60 * 5;
	int32_t deathTimer_ = kLifeTime;
	//デスフラグ
	bool isDead_ = false;
};