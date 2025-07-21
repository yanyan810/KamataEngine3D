#include "Enemy.h"
#define NOMINMAX
#include "MapChipFiled.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#define _USE_MATH_DEFINES
#include <numbers>
#include "EnemyState_Approach.h"  // ← これを追加



using namespace KamataEngine;

void Enemy::Initialize(Model* model, const Vector3& position, uint32_t textureHandle) {
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	SetState(new EnemyState_Approach());
}

void Enemy::Update() {
	if (state_) {
		state_->Update(this);
	}
}

void Enemy::SetState(EnemyState* newState) {
	if (state_) {
		delete state_;
	}
	state_ = newState;
}

void Enemy::Move(const Vector3& velocity) {
	worldTransform_.translation_ += velocity;
	WorldTrnasformUpdate(worldTransform_);
}

const Vector3& Enemy::GetPosition() const { return worldTransform_.translation_; }

void Enemy::Draw(const Camera& camera) { model_->Draw(worldTransform_, camera, textureHandle_); }
