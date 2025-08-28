#pragma once
#include "AABB.h"
#include "KamataEngine.h"
#include "WorldTransformClass.h"
class Rock {
public:
	Rock() = default;

	void Initialize(const KamataEngine::Vector3& min, const KamataEngine::Vector3& max);
	
	void Update(float dt);
	void Draw();

	const AABB& GetAABB() const { return aabb_; }

	// ★ 追加
	static void SetGraphics(KamataEngine::Model* model, KamataEngine::Camera* camera);

	 void OnCollision();

	   bool IsDead() const { return isDead_; }

	   // アルファ指定で一時的に描画（描画後に元へ戻す）
	   void DrawWithAlpha(float a);


private:
	AABB aabb_{};
	KamataEngine::WorldTransform world_{};
	static inline KamataEngine::Model* s_model_ = nullptr;
	static inline KamataEngine::Camera* s_camera_ = nullptr;
	bool isDead_ = false;
	

};
