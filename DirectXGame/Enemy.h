#pragma once
#include "AABB.h"
#include "KamataEngine.h"
#include "WorldTransformClass.h"
#include "Easing.h"
#include "Operator.h"


class Enemy {



public:
	void Initialize(KamataEngine::Model* model,const KamataEngine::Vector3& position, uint32_t textureHandle);

	void Update();

	void Draw(const KamataEngine::Camera& camera);

	void ApproachUpdate();

	void LeaveUpdate();

	private:

			using StateFunc = void (Enemy::*)(); // 状態関数型（メンバ関数ポインタ）
	    StateFunc stateFunc_ = nullptr;      // 現在の状態関数

		KamataEngine::WorldTransform worldTransform_;
	    KamataEngine::Model* model_ = nullptr;
	    uint32_t textureHandle_ = 0;

		//Phase phase_ = Phase::Unknown;

};