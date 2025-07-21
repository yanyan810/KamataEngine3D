#include "PlayerBullet.h"

using namespace KamataEngine;

void PlayerBullet::Initialize(Model* model, const Vector3& position) { 
	assert(model);

	model_ = model;
	//テクスチャ読み込み
	textureHandle_ = TextureManager::Load("black.png");

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

}

void PlayerBullet::Updata() {



	WorldTrnasformUpdate(worldTransform_);

}


void PlayerBullet::Draw(const Camera& camera) {

	model_->Draw(worldTransform_, camera, textureHandle_);

}