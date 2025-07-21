#pragma once
#include "AABB.h"
#include "KamataEngine.h"
#include "WorldTransformClass.h"
#include "Easing.h"
#include "Operator.h"

	enum class Phase {
	Approach, // 接近する
	Leave,    // 離脱する
	Unknown,
};

class Enemy {



public:
	void Initialize(KamataEngine::Model* model,const KamataEngine::Vector3& position, uint32_t textureHandle);

	void Update();

	void Draw(const KamataEngine::Camera& camera);

	void ApproachUpdate();

	void LeaveUpdate();

	private:

		KamataEngine::WorldTransform worldTransform_;
	    KamataEngine::Model* model_ = nullptr;
	    uint32_t textureHandle_ = 0;

		Phase phase_ = Phase::Unknown;

};