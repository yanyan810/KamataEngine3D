#include "Enemy.h"
#define NOMINMAX
#include "MapChipFiled.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#define _USE_MATH_DEFINES
#include "EnemyState_Approach.h" // ← これを追加
#include <numbers>

using namespace KamataEngine;

void Enemy::Initialize(Model* model, const Vector3& position, uint32_t textureHandle) {
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	input_ = Input::GetInstance();

	SetState(new EnemyState_Approach());

	ApproachInitialize();
}

Enemy::~Enemy() {

	for (EnemyBullet* bullet_ : bullets_) {
		delete bullet_;
	}
}

void Enemy::DecrementFireTimer() {
	if (fireTimer > 0) {
		fireTimer--;
	}
}

bool Enemy::IsFireTimerExpired() const { return fireTimer <= 0; }

void Enemy::Fire() {

	
		EnemyBullet* newBullet = new EnemyBullet();
		newBullet->Initialize(model_, worldTransform_.translation_);

		// 球を登録する
		bullets_.push_back(newBullet);
	
}

void Enemy::ApproachInitialize() { fireTimer = kFIreInterval; }

void Enemy::Update() {

	// 　ですフラグの立った球を削除
	bullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}

		return false;
	});

	if (state_) {
		state_->Update(this);
	}

	for (EnemyBullet* bullet_ : bullets_) {
		bullet_->Update(); // 弾の更新処理をここに入れる
	}

	ImGui::Text("Fire Timer: %d", fireTimer);

}

void Enemy::SetState(EnemyState* newState) {
	if (state_) {
		delete state_;
	}
	state_ = newState;
	state_->Enter(this); // 状態切替時に初期化処理を呼ぶ
}

void Enemy::Move(const Vector3& velocity) {
	worldTransform_.translation_ += velocity;
	WorldTrnasformUpdate(worldTransform_);
}

const Vector3& Enemy::GetPosition() const { return worldTransform_.translation_; }

void Enemy::Draw(const Camera& camera) {
	model_->Draw(worldTransform_, camera, textureHandle_);
	for (EnemyBullet* bullet_ : bullets_) {
		bullet_->Draw(camera); // 弾の描画もここで
	}



}
