#include "Enemy.h"
#define NOMINMAX
#include "MapChipFiled.h"
#include "WorldTransformClass.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#define _USE_MATH_DEFINES
#include <numbers>

// ―――― 度 → ラジアン関数 ――――
inline float DegToRad(float deg) { return deg * std::numbers::pi_v<float> / 180.0f; }

//float Lerp(float a, float b, float t) { return a + (b - a) * t; }


void Enemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	// モデルの読み込み
	model_ = model;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = -std::numbers::pi_v<float> / 2.0f;

	// カメラの初期化
	camera_ = camera;

	// 速度を設定する
	velostity_ = {-kWalkSpeed, 0, 0};

	walkTimer_ = 0.0f;

	#include <numbers>

	// pi = std::numbers::pi_v<float>;     // float版π

}

void Enemy::OnCollision(const Player* player) { 
	(void)player; 
}

AABB Enemy::GetAABB() {

	AABB aabb;
	aabb.min = {worldTransform_.translation_.x - kWidth / 2.0f, worldTransform_.translation_.y - kHeight / 2.0f, worldTransform_.translation_.z - kWidth / 2.0f};
	aabb.max = {worldTransform_.translation_.x + kWidth / 2.0f, worldTransform_.translation_.y + kHeight / 2.0f, worldTransform_.translation_.z + kWidth / 2.0f};

	return aabb;
}


void Enemy::WorldPosUpdate() {

	worldTransform_.translation_.x += velostity_.x;
	worldTransform_.translation_.y += velostity_.y;
	worldTransform_.translation_.z += velostity_.z;
}

void Enemy::Update() {
	
	// 位置の更新
	WorldPosUpdate();


	//タイマーを加算
	walkTimer_ += 1.0f / 60.0f;

	// 顔を上下に動かすアニメーションの計算
	float param = std::sin(2.0f * std::numbers::pi_v<float> * walkTimer_ / kWalkMotionTime);
	float degree = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
	param = DegToRad(degree);
	
	worldTransform_.rotation_.x = param;

	// ワールドトランスフォームの更新
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();

	//ワールド行列の更新
	WorldTrnasformUpdate(worldTransform_);



}

void Enemy::Draw() {
	// モデルの描画
	if (model_) {
		model_->Draw(worldTransform_, *camera_, textureHandle_);
	}
}