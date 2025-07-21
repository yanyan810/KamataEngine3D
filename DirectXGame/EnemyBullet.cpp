#include "EnemyBullet.h"

using namespace KamataEngine;

void EnemyBullet::Initialize(Model* model,const Vector3& position) {

	assert(model);
	model_ = model;
	textureHandle_ = TextureManager::Load("debugfont.png");

	worldTransform_.Initialize();
	//引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;


}

void EnemyBullet::Update() {

	const float kBulletSpeed = 1.0f;

	Vector3 velocity = {0.0f, 0.0f, -kBulletSpeed};

	worldTransform_.translation_ += velocity;

	if (--deathTimer_ <=0) {
	
		isDead_ = true;

	}

	WorldTrnasformUpdate(worldTransform_);

}

void EnemyBullet::Draw(const Camera& camera) {

	model_->Draw(worldTransform_, camera, textureHandle_);

}