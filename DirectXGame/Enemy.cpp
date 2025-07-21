#include "Enemy.h"
#define NOMINMAX
#include "MapChipFiled.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#define _USE_MATH_DEFINES
#include <numbers>

using namespace KamataEngine;

void Enemy::Initialize(Model* model, const Vector3& position, uint32_t textureHandle) {

	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	stateFunc_ = &Enemy::ApproachUpdate; // 初期状態

}

void Enemy::ApproachUpdate() {
	const float kMoveSpeed = 0.5f;

	Vector3 velocity = {0, 0, -kMoveSpeed};

	worldTransform_.translation_ += velocity;

	if (worldTransform_.translation_.z < 0.0f) {

		stateFunc_ = &Enemy::LeaveUpdate; // 状態遷移
	}

	WorldTrnasformUpdate(worldTransform_);
}

void Enemy::LeaveUpdate() {
	const float kMoveSpeed = 0.05f;

	Vector3 velocity = {-kMoveSpeed, kMoveSpeed, -kMoveSpeed};

	// 移動
	worldTransform_.translation_ += velocity;

	WorldTrnasformUpdate(worldTransform_);
}

void Enemy::Update() {

	if (stateFunc_) {
		(this->*stateFunc_)(); // 現在の状態関数を実行
	}
}

void Enemy::Draw(const Camera& camera) { model_->Draw(worldTransform_, camera, textureHandle_); }