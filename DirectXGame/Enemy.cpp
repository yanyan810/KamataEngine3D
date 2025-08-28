#include "Enemy.h"
using namespace KamataEngine;

void Aquatic::SetGraphics(Model* model, Camera* camera) {
	s_model_ = model;
	s_camera_ = camera;
}

void Aquatic::Initialize(const Vector3& pos, const Vector3& vel, float radius) {
	pos_ = pos;
	vel_ = vel;
	radius_ = radius;
	alive_ = true;

	world_.Initialize();
	world_.scale_ = {radius_ * 0.1f, radius_ * 0.1f, radius_ * 0.1f}; // モデル基準に応じて調整
	world_.rotation_ = {0, 0, 0};
	world_.translation_ = pos_;
	world_.matWorld_ = MakeAffineMatrix(world_.scale_, world_.rotation_, world_.translation_);
	world_.TransferMatrix();
}

void Aquatic::Update(float dt) {
	if (!alive_)
		return;
	pos_ = pos_ + vel_ * dt;

	world_.translation_ = pos_;
	world_.matWorld_ = MakeAffineMatrix(world_.scale_, world_.rotation_, world_.translation_);
	world_.TransferMatrix();
}

void Aquatic::Draw() {
	if (!alive_ || !s_model_ || !s_camera_)
		return;
	s_model_->Draw(world_, *s_camera_);
}
