#pragma once
#include "AABB.h"
#include "KamataEngine.h"
#include "WorldTransformClass.h"

class Goal {
public:
	void Initialize(const KamataEngine::Vector3& min, const KamataEngine::Vector3& max) {
		aabb_.min = min;
		aabb_.max = max;
	}
	
	void Draw();

	const AABB& GetAABB() const { return aabb_; }

	static void SetGraphics(KamataEngine::Model* model, KamataEngine::Camera* camera);

   void SetVelocity(const KamataEngine::Vector3& v) {
		velocity_ = v;
		active_ = true;
	}
	bool IsActive() const { return active_; }

	// 便利関数（必要なら）
	KamataEngine::Vector3 GetCenter() const { return {(aabb_.min.x + aabb_.max.x) * 0.5f, (aabb_.min.y + aabb_.max.y) * 0.5f, (aabb_.min.z + aabb_.max.z) * 0.5f}; }

	void Spawn(const KamataEngine::Vector3& center) {
		KamataEngine::Vector3 size = {60.0f, 60.0f, 60.0f};
		aabb_.min = {center.x - size.x * 0.5f, center.y - size.y * 0.5f, center.z - size.z * 0.5f};
		aabb_.max = {center.x + size.x * 0.5f, center.y + size.y * 0.5f, center.z + size.z * 0.5f};
		active_ = true; // ★出現と同時に有効化
	}

	void Update(float dt); // ★実装を差し替え

private:
	AABB aabb_{};
	KamataEngine::WorldTransform world_{};
	static inline KamataEngine::Model* s_model_ = nullptr;
	static inline KamataEngine::Camera* s_camera_ = nullptr;

	bool active_ = false;                     // ★動作中フラグ
	KamataEngine::Vector3 velocity_{0, 0, 0}; // ★移動速度（単位/秒）
};
