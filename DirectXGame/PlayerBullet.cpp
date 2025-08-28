#include "playerBullet.h"
using namespace KamataEngine;

void Bullet::SetGraphics(Model* model, Camera* camera) {
	s_model_ = model;
	s_camera_ = camera;
}

void Bullet::Initialize(const Vector3& pos, const Vector3& vel, float lifeSec) {
	pos_ = pos;
	vel_ = vel;
	life_ = lifeSec;
	world_.Initialize();
	world_.scale_ = {0.8f, 0.8f, 0.8f}; // 弾サイズ（モデルが1m立方想定なら小さめに）
	world_.rotation_ = {0, 0, 0};
	world_.translation_ = pos_;
	world_.matWorld_ = MakeAffineMatrix(world_.scale_, world_.rotation_, world_.translation_);
	world_.TransferMatrix();
}

void Bullet::Update(float dt) {
	pos_ = pos_ + vel_ * dt;
	life_ -= dt;

	world_.translation_ = pos_;
	world_.matWorld_ = MakeAffineMatrix(world_.scale_, world_.rotation_, world_.translation_);
	world_.TransferMatrix();

	// ★ デバッグ表示用
	
}

void Bullet::Draw() {
	if (!s_model_ || !s_camera_)
		return;

	
	s_model_->Draw(world_, *s_camera_);
}