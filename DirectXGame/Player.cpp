#define NOMINMAX
#include "Player.h"
#include "WorldTransformClass.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <numbers>

float Lerp(float a, float b, float t) { return a + (b - a) * t; }

void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {

	// NULLポインタチェック
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void Player::Updata() {
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();

	bool landing = false;

	// 地面との当たり判定
	// 下降中？
	if (velosity_.y < 0.0f) {

		// Y座標が地面以下になったら着地
		if (worldTransform_.translation_.y < 2.0f) {
			landing = true;
		}
	}

	// 移動処理
	// 接地状態
	if (onGround_) {
		if (KamataEngine::Input::GetInstance()->PushKey(DIK_RIGHT) || KamataEngine::Input::GetInstance()->PushKey(DIK_LEFT)) {

			// 左右加速
			KamataEngine::Vector3 acceleration = {};
			if (KamataEngine::Input::GetInstance()->PushKey(DIK_RIGHT)) {
				// 左移動中の右入力
				if (velosity_.x < 0.0f) {
					// 速度と逆方向の場合ブレーキ
					velosity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x += kAcceleration;

				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnTimer_ = 1.0f;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
				}

			} else if (KamataEngine::Input::GetInstance()->PushKey(DIK_LEFT)) {

				if (velosity_.x > 0.0f) {
					// 速度と逆方向の場合ブレーキ
					velosity_.x *= (1.0f - kAttenuation);
				}

				acceleration.x -= kAcceleration;

				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnTimer_ = 1.0f;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
				}
			}
			velosity_.x += acceleration.x;

			// 最大速度制限
			velosity_.x = std::clamp(velosity_.x, -kLimitRunSpeed, kLimitRunSpeed);

		} else {

			velosity_.x *= (1.0f - kAttenuation);
		}

		if (turnTimer_ > 0.0f) {
			turnTimer_ -= 1.0f / 30.0f;
			// 左右の自キャラ角度テーブル
			float destinationRotationYTable[] = {
			    std::numbers::pi_v<float> / 2.0f,
			    std::numbers::pi_v<float> * 3.0f / 2.0f,

			};
			// 状態に応じた角度を取得する
			float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

			// 補間率
			float t = 1.0f - turnTimer_;
			t = std::clamp(t, 0.0f, 1.0f); // 念のため

			// 自キャラの角度を設定する
			worldTransform_.rotation_.y = worldTransform_.rotation_.y = Lerp(turnFirstRotationY_, destinationRotationY, t);
			;
		}
		if (KamataEngine::Input::GetInstance()->PushKey(DIK_UP)) {
			// ジャンプ
			if (onGround_) {
				velosity_.y = kJumpAcceleration;
				onGround_ = false;
			}
		}
		// 空中
	} else {

		// 着地
		if (landing) {
			worldTransform_.translation_.y = 2.0f;
			// 摩擦で横方向速度が減衰する
			velosity_.x *= (1.0f - kAttenuation);
			// 下方向をリセット
			velosity_.y = 0.0f;
			// 接地状態にする
			onGround_ = true;
		} else {
			// 落下速度
			velosity_.x += 0;
			velosity_.y += -kGravityAcceleration;
			velosity_.z = 0;
			// 落下速度制限
			velosity_.y = std::max(velosity_.y, -kLimitFallSpeed);
		
		}
	}

	worldTransform_.translation_.x += velosity_.x;
	worldTransform_.translation_.y += velosity_.y;
	worldTransform_.translation_.z += velosity_.z;
}

void Player::Draw() { model_->Draw(worldTransform_, *camera_, textureHandle_); }

void Player::SetPosition(const KamataEngine::Vector3& position) { worldTransform_.translation_ = position; }

const KamataEngine::WorldTransform& Player::GetWoldTransform() const { 
	return worldTransform_; 
}
