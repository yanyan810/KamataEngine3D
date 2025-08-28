#pragma once
#include "KamataEngine.h"
#include "Operator.h"
#include "WorldTransformClass.h"
class Aquatic {
public:
	void Initialize(const KamataEngine::Vector3& pos, const KamataEngine::Vector3& vel, float radius);
	void Update(float dt);
	void Draw();

	void Kill() { alive_ = false; }
	bool IsAlive() const { return alive_; }

	const KamataEngine::Vector3& GetPosition() const { return pos_; }
	float GetRadius() const { return radius_; }

	// ★ 追加
	static void SetGraphics(KamataEngine::Model* model, KamataEngine::Camera* camera);

private:
	KamataEngine::Vector3 pos_{};
	KamataEngine::Vector3 vel_{};
	float radius_ = 14.0f;
	bool alive_ = true;

	KamataEngine::WorldTransform world_{};
	static inline KamataEngine::Model* s_model_ = nullptr;
	static inline KamataEngine::Camera* s_camera_ = nullptr;
};
