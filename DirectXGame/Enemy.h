#pragma once
#include "KamataEngine.h"
#include "WorldTransformClass.h"
#include "AABB.h"

class Player;

class Enemy {

public:
	

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	void Update();

	void Draw();

	void WorldPosUpdate();

	AABB GetAABB();

	void OnCollision(const Player* player);

private:
	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Camera* camera_ = nullptr;

	uint32_t textureHandle_ = 0u;

	static inline const float kWalkSpeed = 0.1f;
	// 速度
	KamataEngine::Vector3 velostity_{};

	//  float pi = 0.0f; // float版π

		// キャラクターの当たり判定サイズ
	static inline const float kWidth = 2.0f;
	static inline const float kHeight = 2.0f;

	// 最初の角度[度]
	static inline const float kWalkMotionAngleStart = 0.0f;
	// 最後の角度[度]
	static inline const float kWalkMotionAngleEnd = 30.0f;
	// アニメーションの周期となる時間[秒]
	static inline const float kWalkMotionTime = 1.0f;

	float walkTimer_ = 0.0f;

	float turnNum = 0.0f;
};