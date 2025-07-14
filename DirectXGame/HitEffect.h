#pragma once
#include <KamataEngine.h>
#include "WorldTransformClass.h"
class HitEffect {
public:
	void Initialize(KamataEngine::Vector3& spawnPosition);

	void Update();

	void Draw();

	static void SetModel(KamataEngine::Model* model) { model_ = model; }
	static void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; }

	static HitEffect* Create(KamataEngine::Vector3& spawnPosition);

private:
	// モデル
	static KamataEngine::Model* model_;
	// カメラ
	static KamataEngine::Camera* camera_;

	KamataEngine::WorldTransform circleWorldTransform;

};
