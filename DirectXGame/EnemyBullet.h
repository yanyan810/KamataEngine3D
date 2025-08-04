#pragma once
#include "KamataEngine.h"
#include "WorldTransformClass.h"
#include "Operator.h"


class EnemyBullet {
public: 

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="position">初期座標</param>
	void Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position, const KamataEngine::Vector3& velocity);


	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const KamataEngine::Camera& camera);

	bool IsDead() const { return isDead_; }

	private:

	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	uint32_t textureHandle_ = 0;

	static const int32_t kLifeTime = 60 * 3;
	int32_t deathTimer_ = kLifeTime;
	bool isDead_ = false;

	KamataEngine::Vector3 velocity_{}; // 追加：方向と速さを持つベクトル



};


