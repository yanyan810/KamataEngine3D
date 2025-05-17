#include "Player.h"
#include "WorldTransformClass.h"

void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Camera* camera) {

//NULLポインタチェック
assert(model);
model_ = model;
textureHandle_ = textureHandle;
camera_ = camera;

worldTransform_.Initialize();

}

void Player::Updata() {
	MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	//worldTransform_.TransferMatrix();

}

void Player::Draw() { 
		
	model_->Draw(worldTransform_, *camera_, textureHandle_);

}