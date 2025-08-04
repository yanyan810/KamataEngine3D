#include "EnemyBullet.h"

using namespace KamataEngine;

void EnemyBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {
	model_ = model;
	textureHandle_ = TextureManager::Load("black.png");
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	velocity_ = velocity;
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