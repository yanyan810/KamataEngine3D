#pragma once
#include "KamataEngine.h"
#include "Operator.h"
#include "WorldTransformClass.h"
class Bullet {
public:
	void Initialize(const KamataEngine::Vector3& pos, const KamataEngine::Vector3& vel, float lifeSec = 2.0f);
	void Update(float dt);
	void Draw();

	bool IsDead() const { return life_ <= 0.0f; }
	const KamataEngine::Vector3& GetPosition() const { return pos_; }
	float GetRadius() const { return radius_; }

	// ★ 追加
	static void SetGraphics(KamataEngine::Model* model, KamataEngine::Camera* camera);

	void Kill() { life_ = 0.0f; }

private:
	KamataEngine::Vector3 pos_{};
	KamataEngine::Vector3 vel_{};
	float life_ = 0.0f;
	float radius_ = 4.0f;

	

	// ★ 追加
	KamataEngine::WorldTransform world_{};
	static inline KamataEngine::Model* s_model_ = nullptr;
	static inline KamataEngine::Camera* s_camera_ = nullptr;
};