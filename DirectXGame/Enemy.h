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

	private:

		KamataEngine::WorldTransform worldTransform_;
	    KamataEngine::Model* model_ = nullptr;
	    uint32_t textureHandle_ = 0;

};