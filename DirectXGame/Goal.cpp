#include "Goal.h"
using namespace KamataEngine;

void Goal::SetGraphics(Model* model, Camera* camera) {
	s_model_ = model;
	s_camera_ = camera;


}

void Goal::Update(float dt) {
	if (!active_)
		return;

	// AABB を平行移動（vel * dt）
	const KamataEngine::Vector3 d = {velocity_.x * dt, velocity_.y * dt, velocity_.z * dt};
	aabb_.min = {aabb_.min.x + d.x, aabb_.min.y + d.y, aabb_.min.z + d.z};
	aabb_.max = {aabb_.max.x + d.x, aabb_.max.y + d.y, aabb_.max.z + d.z};
}

void Goal::Draw() {
	if (!s_model_ || !s_camera_)
		return;

	Vector3 size = {aabb_.max.x - aabb_.min.x, aabb_.max.y - aabb_.min.y, aabb_.max.z - aabb_.min.z};
	Vector3 center = {(aabb_.min.x + aabb_.max.x) * 0.5f, (aabb_.min.y + aabb_.max.y) * 0.5f, (aabb_.min.z + aabb_.max.z) * 0.5f};

	world_.Initialize();
	world_.scale_ = size;
	world_.rotation_ = {0, 1.55f, 0};
	world_.translation_ = center;
	world_.matWorld_ = MakeAffineMatrix(world_.scale_, world_.rotation_, world_.translation_);
	world_.TransferMatrix();

	s_model_->Draw(world_, *s_camera_);
}
