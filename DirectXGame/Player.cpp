#define NOMINMAX
#include "Player.h"
#include <algorithm>
using namespace KamataEngine;

void Player::LoadMell() { 
	mellModel_ = Model::CreateFromOBJ("cube"); 
	//mellTextureHandle_=
	mellWorldTransform_.Initialize();
}

void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle) {

	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	input_ = Input::GetInstance();
	worldTransform_.rotation_.y = 1.5f;
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionAttributeEnemy | kCollisionAttributeEnemyBullet);
}

Player::~Player() {
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
}

void Player::Rotate() {
	// 回転速さ
	const float kRotSpeed = 0.02f;

	// 押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	} else if (input_->PushKey(DIK_D)) {

		worldTransform_.rotation_.y += kRotSpeed;
	}
}

Vector3 Player::GetPosition() {

	Vector3 worldPos;

	worldPos = worldTransform_.translation_;

	return worldPos;
}

//// 何もしない
// void Player::OnCollision() {
//
//
//
// }

void Player::Attack() {
	////前に攻撃判定を出す
	//if (input_->TriggerKey(DIK_SPACE)) {

	//	// 球の速度
	//	const float kBulletSpeed = 1.0f;
	//	Vector3 velocity(0, 0, kBulletSpeed);

	//	// 速度ベクトルを自機の向きに合わせて回転させる
	//	velocity = Matrix4x4_::TransformNormal(velocity, worldTransform_.matWorld_);

	//	// 球を生成し初期化
	//	PlayerBullet* newBullet = new PlayerBullet();
	//	newBullet->Initialize(worldTransform_.translation_, velocity);

	//	// 球を登録
	//	bullets_.push_back(newBullet);
	//}

	if (!isAttacking_ && input_->TriggerKey(DIK_SPACE)) {
		isAttacking_ = true;
		attackTimer_ = 15.0f;

		// === 攻撃方向を決定 ===
		KamataEngine::Vector3 forward{};
		if (direction_ == Right) {
			forward = {1.0f, 0.0f, 0.0f}; // 右方向
		} else {
			forward = {-1.0f, 0.0f, 0.0f}; // 左方向
		}

		// プレイヤーの少し前に攻撃位置を設定
		attackPos_ = worldTransform_.translation_ + forward * 2.0f;

		// 近接攻撃モデルの位置更新
		mellWorldTransform_.translation_ = attackPos_;
		mellWorldTransform_.scale_ = attackRange_;
		mellWorldTransform_.rotation_ = worldTransform_.rotation_;
		WorldTrnasformUpdate(mellWorldTransform_);
	}

	// 攻撃中ならタイマー減少
	if (isAttacking_) {
		attackTimer_--;
		if (attackTimer_ <= 0) {
			isAttacking_ = false;
		}
	}

}

void Player::Updata() {

	WorldTrnasformUpdate(worldTransform_);

	Vector3 move = {0, 0, 0};
	// 移動速度
	const float kCharacterSpeed = 0.2f;

	// ですっフラグの立った球を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}

		return false;
	});

	// Rotate();

	if (direction_ == Left) {
		worldTransform_.rotation_.y = -1.5f;
	
	} else {
		worldTransform_.rotation_.y = 1.5f;
	}

	if(!isAttacking_) {
		// 押した方向でベクトル変更
		if (input_->PushKey(DIK_LEFT) || input_->PushKey(DIK_A)) {
			move.x -= kCharacterSpeed;
			direction_ = Left; // ← 方向更新

		} else if (input_->PushKey(DIK_RIGHT) || input_->PushKey(DIK_D)) {
			move.x += kCharacterSpeed;
			direction_ = Right; // ← 方向更新
		}

		if (worldTransform_.translation_.y < 1.0f) {

			if (input_->PushKey(DIK_UP) || input_->PushKey(DIK_W)) {
				move.y += kCharacterSpeed;

				move.z += kCharacterSpeed;
			}
		}
		if (worldTransform_.translation_.y > -4.0f) {

			if (input_->PushKey(DIK_DOWN || input_->PushKey(DIK_S))) {
				move.y -= kCharacterSpeed;

				move.z -= kCharacterSpeed;
			}
		}
	}
	/*const float kMoveLimitX = 34.0f;
	const float kMoveLimitY = 18.0f;*/

	Attack();

	// 弾更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Updata();
	}

	// 座標移動
	worldTransform_.translation_ += move;

	if (lockXEnabled_) {
		worldTransform_.translation_.x = std::clamp(worldTransform_.translation_.x, lockMinX_, lockMaxX_);
	}


	//// 範囲を超えない処理
	//worldTransform_.translation_.x = std::clamp(worldTransform_.translation_.x, -kMoveLimitX, +kMoveLimitX);
	//worldTransform_.translation_.y = std::clamp(worldTransform_.translation_.y, -kMoveLimitY, +kMoveLimitY);

	#ifdef DEBUG
	ImGui::Text("Player: (%.2f, %.2f, %.2f)", worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z);

#endif // DEBUG

	
	//ImGui::SliderFloat3("Player", &worldTransform_.translation_.x, 20.0f, -20.0f);
}

void Player::Draw(Camera& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	// 近接攻撃可視化（デバッグ用）
	if (isAttacking_ && mellModel_) {
		mellModel_->Draw(mellWorldTransform_, viewProjection);
	}

	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}