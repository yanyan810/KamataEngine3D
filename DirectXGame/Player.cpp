#define NOMINMAX
#include "Player.h"
#include <algorithm>
using namespace KamataEngine;


void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle) {

	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	input_ = Input::GetInstance();

}

void Player::Updata() {
	
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.TransferMatrix();

	Vector3 move = {0, 0, 0};
	//移動速度
	const float kCharacterSpeed = 0.2f;

	//押した方向でベクトル変更
	if (input_->PushKey(DIK_LEFT)) {
		move.x -= kCharacterSpeed;
	
	} else if(input_->PushKey(DIK_RIGHT)) {
		move.x += kCharacterSpeed;
	}

		if (input_->PushKey(DIK_UP)) {
		move.y += kCharacterSpeed;

	} else if (input_->PushKey(DIK_DOWN)) {
		move.y -= kCharacterSpeed;
	}

	const float kMoveLimitX = 34.0f;
	const float kMoveLimitY = 18.0f;



	//座標移動
	worldTransform_.translation_ += move;
	
		// 範囲を超えない処理
	worldTransform_.translation_.x = std::clamp(worldTransform_.translation_.x, -kMoveLimitX, +kMoveLimitX);
	worldTransform_.translation_.y = std::clamp(worldTransform_.translation_.y, -kMoveLimitY, +kMoveLimitY);



	ImGui::SliderFloat3("Player", &worldTransform_.translation_.x,20.0f, -20.0f);
	


}

void Player::Draw(Camera& viewProjection) {
	model_->Draw(worldTransform_,viewProjection ,textureHandle_);
}