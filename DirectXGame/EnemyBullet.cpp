#include "EnemyBullet.h"

using namespace KamataEngine;

#include <cmath> // std::atan2 用

void EnemyBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {
	model_ = model;
	textureHandle_ = TextureManager::Load("black.png");

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	velocity_ = velocity;

	worldTransform_.scale_ = Vector3(0.5f, 0.5f, 3.0f); // 見た目を細長くする

	// 進行方向のY回転（XZ平面に投影して考える）
	worldTransform_.rotation_.y = std::atan2(velocity_.x, velocity_.z);

	// 進行方向のX回転（Y方向の傾き）
	float horizontalLen = std::sqrt(velocity_.x * velocity_.x + velocity_.z * velocity_.z);
	worldTransform_.rotation_.x = std::atan2(-velocity_.y, horizontalLen); // ※右手系前提

	// ワールド行列を更新
	WorldTrnasformUpdate(worldTransform_);
}


void EnemyBullet::Update() {
	worldTransform_.translation_ += velocity_;

	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	WorldTrnasformUpdate(worldTransform_);
}

void EnemyBullet::Draw(const Camera& camera) {

	model_->Draw(worldTransform_, camera, textureHandle_);

}