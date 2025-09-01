#include "Player.h"
using namespace KamataEngine;

void Player::SetGraphics(Model* model, Camera* camera) {
	s_model_ = model;
	s_camera_ = camera;
}

void Player::Initialize(const Vector3& pos, const GameParams& params) {
	pos_ = pos;
	params_ = params;
	hp_ = params.playerMaxHP;
	ammo_ = params.maxAmmo;
	invincibleTimer_ = 0.0f;

	objectColor_.Initialize();
	color_ = {1.0f, 1.0f, 1.0f, 0.5f}; // 初期カラー（白）

	world_.Initialize();
	world_.scale_ = {1.0f, 1.0f, 1.0f};
	world_.rotation_ = {0.0f, 1.55f, 0.0f};
	world_.translation_ = pos_;
	world_.matWorld_ = MakeAffineMatrix(world_.scale_, world_.rotation_, world_.translation_);
	world_.TransferMatrix();
}

void Player::SetDeadPose() {
	// 仰向けの角度に調整（例：背中を下に向ける）
	world_.translation_ = {0.0f, 0.0f, -30.0f};
	world_.rotation_ = {1.5f, 0.0f, 0.0f}; // X軸に回転（正面を上に）
	world_.matWorld_ = MakeAffineMatrix(world_.scale_, world_.rotation_, world_.translation_);
	world_.TransferMatrix();
}


void Player::OnHit() {
	// ★ 回転中は無敵
	if (rotationTime_ > 0.0f)
		return;

	hp_--;
	invincibleTimer_ = params_.invincibleTimeAfterHit; // ←通常ダメージ用だけに使う
	FireRadialBullets();
}

void Player::Oncollision() {
	if (rotationTime_ > 0.0f || invincibleTimer_ > 0.0f)
		return;

	if (hp_ > 0) {
		hp_ -= 1;
		ammo_ -= 300;
		invincibleTimer_ = params_.invincibleTimeAfterHit;

		FireRadialBullets(); // 💥イクラ発射！
	}
}

// Player.cpp
void Player::TitleHit() {
	// 既に無敵中は再発動しない
	if (invincibleTimer_ > 0.0f)
		return;

	// 無敵だけ付与（体力/弾は触らない）
	invincibleTimer_ = params_.invincibleTimeAfterHit;

	// ★回転は開始しない（常に0を維持）
	rotationTime_ = 0.0f;

	// イクラ弾を放射
	FireRadialBullets();
}

void Player::UpdateDemo(float dt) {
	// 一定時間ごとに方向をランダムに変える
	static float changeDirTimer = 0.0f;
	static Vector3 dir{0, 0, 0};

	changeDirTimer -= dt;
	if (changeDirTimer <= 0.0f) {
		// 新しい方向を完全ランダムに
		float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * 3.14159f;
		dir.x = std::cos(angle);
		dir.y = std::sin(angle);

		changeDirTimer = 0.5f + (rand() % 100) / 50.0f; // 0.5〜2.5秒で切替
	}

	// 移動
	pos_.x += dir.x * moveSpeed_;
	pos_.y += dir.y * moveSpeed_;

	// ゆらゆら泳ぐ動きも追加
	static float swimTimer = 0.0f;
	swimTimer += dt * 10.0f;
	pos_.y += std::sin(swimTimer) * 0.05f;

	// 画面端でバウンドするようにする（はみ出さないように）
	if (pos_.x <= params_.moveLimitMinX || pos_.x >= params_.moveLimitMaxX) {
		dir.x = -dir.x;
	}
	if (pos_.y <= params_.moveLimitMinY || pos_.y >= params_.moveLimitMaxY) {
		dir.y = -dir.y;
	}

	// 移動制限
	pos_.x = std::clamp(pos_.x, params_.moveLimitMinX, params_.moveLimitMaxX);
	pos_.y = std::clamp(pos_.y, params_.moveLimitMinY, params_.moveLimitMaxY);

	// WorldTransform 同期
	world_.translation_ = pos_;
	world_.matWorld_ = MakeAffineMatrix(world_.scale_, world_.rotation_, world_.translation_);
	world_.TransferMatrix();
	
		if (invincibleTimer_ > 0.0f) {
		invincibleTimer_ -= dt;
		if (invincibleTimer_ < 0.0f)
			invincibleTimer_ = 0.0f;
	}

}


void Player::FireRadialBullets() {
	const int bulletCount = 16; // 弾の数
	const float speed = 15.0f;

	for (int i = 0; i < bulletCount; ++i) {
		float angle = (2.0f * 3.14159265f * i) / bulletCount;

		Vector3 dir = {
		    std::cos(angle), std::sin(angle),
		    0.0f // ★ Z方向に飛ばさない
		};
		Vector3 velocity = dir * speed;

		if (onFireCallback_) {
			onFireCallback_(pos_, velocity);
		}
	}
}


bool Player::TryFire() {
	Input* input = Input::GetInstance();

	if (input->TriggerKey(DIK_SPACE) && ammo_ > 0) {
		ammo_ -= 200;

		// ★ 回転してないときだけ回転開始
		if (rotationTime_ <= 0.0f) {
			rotationDuration_ = 1.0f;
			rotationTime_ = rotationDuration_;
			rotationStartZ_ = world_.rotation_.z;
		}

		// ★ 弾は毎回出す（連打可能）
		FireRadialBullets();
		return true;
	}
	
	return false;
}



void Player::Update(float dt) {
	Input* input = Input::GetInstance();
	Vector3 direction{};

	// 入力方向
	if (input->PushKey(DIK_LEFT) || input->PushKey(DIK_A))
		direction.x -= 1.0f;
	if (input->PushKey(DIK_RIGHT) || input->PushKey(DIK_D))
		direction.x += 1.0f;
	if (input->PushKey(DIK_UP) || input->PushKey(DIK_W))
		direction.y += 1.0f;
	if (input->PushKey(DIK_DOWN) || input->PushKey(DIK_S))
		direction.y -= 1.0f;

	// 正規化
	if (direction.x != 0.0f || direction.y != 0.0f) {
		float len = std::sqrt(direction.x * direction.x + direction.y * direction.y);
		direction.x /= len;
		direction.y /= len;
	}

	// 入力移動
	pos_.x += direction.x * moveSpeed_;
	pos_.y += direction.y * moveSpeed_;

	if (rotationTime_ <= 0.0f && invincibleTimer_ <= 0.0f) {
		// --- 常に上下に揺れる処理 ---
		static float swimTimer = 0.0f;
		swimTimer += dt * 10.0f;               // 揺れの速さ
		pos_.y += std::sin(swimTimer) * 0.05f; // 揺れの振幅
	}
	// 回転演出（無敵回転）
	if (rotationTime_ > 0.0f) {
		rotationTime_ -= dt;

		float t = std::clamp(1.0f - (rotationTime_ / rotationDuration_), 0.0f, 1.0f);
		float spin = std::sin(t * 3.1415f * 4.0f);
		world_.rotation_.z = rotationStartZ_ + spin * 3.1415f * 2.0f;

		if (rotationTime_ <= 0.0f) {
			rotationTime_ = 0.0f;
			world_.rotation_.z = rotationStartZ_;
		}
	}

	// 無敵タイマー処理
	if (invincibleTimer_ > 0.0f) {
		invincibleTimer_ -= dt;
		if (invincibleTimer_ < 0.0f)
			invincibleTimer_ = 0.0f;
	}

	// 移動制限
	pos_.x = std::clamp(pos_.x, params_.moveLimitMinX, params_.moveLimitMaxX);
	pos_.y = std::clamp(pos_.y, params_.moveLimitMinY, params_.moveLimitMaxY);

	// WorldTransform 同期
	world_.translation_ = pos_;
	world_.matWorld_ = MakeAffineMatrix(world_.scale_, world_.rotation_, world_.translation_);
	world_.TransferMatrix();

}


void Player::Draw() {
	if (!s_model_ || !s_camera_)
		return;

	


	if (rotationTime_ > 0.0f || invincibleTimer_ > 0.0f) {
		
		objectColor_.SetColor(color_); // カラー設定

		s_model_->Draw(world_, *s_camera_,&objectColor_);



	} else {
		s_model_->Draw(world_, *s_camera_);
	}

}
