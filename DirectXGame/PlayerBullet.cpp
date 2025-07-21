#include "PlayerBullet.h"

using namespace KamataEngine;

void PlayerBullet::Initialize(Model* model, const Vector3& position,const Vector3& velosity) { 
	assert(model);

	model_ = model;
	//テクスチャ読み込み
	textureHandle_ = TextureManager::Load("black.png");
	//引数で受け取った速度をメンバ変数に代入
	velosity_ = velosity;


	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

}

void PlayerBullet::Updata() {

	if (--deathTimer_ <= 0) {
		isDead_=true;
	}

	//座標を移動させる
	worldTransform_.translation_ += velosity_;

	WorldTrnasformUpdate(worldTransform_);

}


void PlayerBullet::Draw(const Camera& camera) {

	model_->Draw(worldTransform_, camera, textureHandle_);

}