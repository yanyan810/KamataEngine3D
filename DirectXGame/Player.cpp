#define NOMINMAX
#include "Player.h"

using namespace KamataEngine;


void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle) {

	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	input_ = Input::GetInstance();

}

void Player::Updata() {
	
	Vector3 move = {0, 0, 0};
	//移動速度
	const float kCharacterSpeed = 0.2f;

	//押した方向でベクトル変更
	if (input_->PushKey(DIK_LEFT)) {
		move.x += kCharacterSpeed;
	
	} else if(input_->PushKey(DIK_RIGHT)) {
		move.x -= kCharacterSpeed;
	}

		if (input_->PushKey(DIK_UP)) {
		move.y -= kCharacterSpeed;

	} else if (input_->PushKey(DIK_DOWN)) {
		move.y += kCharacterSpeed;
	}

	//座標移動
	worldTransform_.translation_.x += move.x;
	worldTransform_.translation_.y += move.y;
	worldTransform_.translation_.z += move.z;

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_,worldTransform_.scale_,worldTransform_.translation_);

	worldTransform_.TransferMatrix();

}

void Player::Draw(Camera& viewProjection) {
	model_->Draw(worldTransform_,viewProjection ,textureHandle_);
}