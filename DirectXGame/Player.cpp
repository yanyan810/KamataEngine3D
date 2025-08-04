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

Vector3 Player::GetPosition() { 

	Vector3 worldPos;

	worldPos = worldTransform_.translation_;

	return worldPos;

}

void Player::Attack() { 
	if (input_->TriggerKey(DIK_SPACE)) {

		//球の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);

		//速度ベクトルを自機の向きに合わせて回転させる
		velocity = Matrix4x4_::TransformNormal(velocity, worldTransform_.matWorld_);
	
		//球を生成し初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, worldTransform_.translation_,velocity);

		//球を登録
		bullets_.push_back(newBullet);

	}

}

void Player::Updata() {
	
WorldTrnasformUpdate(worldTransform_);

	Vector3 move = {0, 0, 0};
	//移動速度
	const float kCharacterSpeed = 0.2f;

	//ですっフラグの立った球を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}

		return false;

	});

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