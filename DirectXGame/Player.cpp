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

Player::~Player() {
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
}

void Player::Rotate() {
	//回転速さ
	const float kRotSpeed = 0.02f;

	//押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	} else if (input_->PushKey(DIK_D)){

		worldTransform_.rotation_.y += kRotSpeed;

	}


}

void Player::Attack() { 
	if (input_->TriggerKey(DIK_SPACE)) {

		

		//球を生成し初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, worldTransform_.translation_);

		//球を登録
		bullets_.push_back(newBullet);

	}

}

void Player::Updata() {
	
WorldTrnasformUpdate(worldTransform_);

	Vector3 move = {0, 0, 0};
	//移動速度
	const float kCharacterSpeed = 0.2f;

	Rotate();

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

	Attack();

	//弾更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Updata();
	
	}


	//座標移動
	worldTransform_.translation_ += move;
	
		// 範囲を超えない処理
	worldTransform_.translation_.x = std::clamp(worldTransform_.translation_.x, -kMoveLimitX, +kMoveLimitX);
	worldTransform_.translation_.y = std::clamp(worldTransform_.translation_.y, -kMoveLimitY, +kMoveLimitY);



	ImGui::SliderFloat3("Player", &worldTransform_.translation_.x,20.0f, -20.0f);
	


}

void Player::Draw(Camera& viewProjection) {
	model_->Draw(worldTransform_,viewProjection ,textureHandle_);

	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	
	}

}