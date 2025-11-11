#include "Enemy.h"
#define NOMINMAX
#include "MapChipFiled.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#define _USE_MATH_DEFINES
#include "EnemyState_Approach.h" // ← これを追加
#include <numbers>
#include "Player.h"

using namespace KamataEngine;

void Enemy::Initialize(Model* model, const Vector3& position, uint32_t textureHandle) {
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	//worldTransform_.translation_.x += 20.0f;

	  worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};

	   // ★ これを最初に入れる（静止してても毎フレーム更新）
	  WorldTrnasformUpdate(worldTransform_);

	input_ = Input::GetInstance();

	SetState(new EnemyState_Approach());

	ApproachInitialize();

	//SetCollisionAttribute(kCollisionAttributeEnemy);

	// 自分は「プレイヤーと自弾」に当たりたい場合
//	SetCollisionMask(kCollisionAttributePlayer | kCollisionAttributePlayerBullet);

	// Enemy::Initialize()
	SetCollisionAttribute(kCollisionAttributeEnemy);
	// 弾だけ当たるように（プレイヤー本体は外す）
	SetCollisionMask(kCollisionAttributePlayerBullet);

 OutputDebugStringA(std::format("[Enemy] Initialize pos=({:.2f},{:.2f},{:.2f})\n", worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z).c_str());

}

Enemy::~Enemy() {

	for (EnemyBullet* bullet_ : bullets_) {
		delete bullet_;
	}

	for (TimedeCall& timedCall : timedCalls_) {
		timedCall.~TimedeCall(); // 明示的にデストラクタを呼び出す
	}

}

Vector3 Enemy::GetWorldPosition() {
	Vector3 worldPos;
	worldPos = worldTransform_.translation_;
	return worldPos;
}

void Enemy::DecrementFireTimer() {
	if (fireTimer > 0) {
		fireTimer--;
	}
}

bool Enemy::IsFireTimerExpired() const { return fireTimer <= 0; }

void Enemy::Fire() {

	assert(player_); // プレイヤーが設定されていることを確認

	//球の速さ
	const float kBulletSpeed = 1.0f;

	//自キャラのワールド座標を取得する
	Vector3 playerPosition = player_->GetPosition();
	//敵キャラのワールド座標を取得する
	Vector3 enemyPosition = GetWorldPosition();
	//敵から自キャラの差分のベクトルを求める
	Vector3 vector = playerPosition - enemyPosition;
	//ベクトルの正規化
	vector = Normalize(vector);
	//ベクトルの長さを早さに合わせる
	vector *= kBulletSpeed;


	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize( worldTransform_.translation_, vector, player_);
	bullets_.push_back(newBullet);

	
}

void Enemy::ApproachInitialize() {
	fireTimer = kFIreInterval;

	// 最初の発射を予約
	std::function<void(void)> callback = std::bind(&Enemy::FireReset, this);
	timedCalls_.emplace_back(callback, kFIreInterval);
}


void Enemy::FireReset() {
	Fire(); 
	// 発射タイマーをリセット
	std::function<void(void)> callback = std::bind(&Enemy::FireReset, this);

	//時限発動イベントを生成
	TimedeCall* timedCall = new TimedeCall(callback, kFIreInterval);

	//時限発動イベントを時限発動イベントリストに追加
	timedCalls_.push_back(*timedCall);

}
void Enemy::Update() {
	// ★ ワールド更新は先頭で
	WorldTrnasformUpdate(worldTransform_);

	// === プレイヤー追従（XZ平面を想定） ===
	if (player_) {
		const Vector3 p = player_->GetWorldPosition();
		const Vector3 e = worldTransform_.translation_;

		Vector3 dir = p - e;
		dir.y = 0.0f; // 上下は固定（必要なら外してOK）

		const float len2 = dir.x * dir.x + dir.z * dir.z;
		if (len2 > 0.0001f) {
			dir = Normalize(dir);

			// 追従スピード（好みで 0.05〜0.20 くらい）
			const float speed = 0.08f;
			worldTransform_.translation_ += dir * speed;

			// 顔の向き（モデルの正面が +Z 前提：+X 右→ ±90°）
			worldTransform_.rotation_.y = std::atan2(dir.x, dir.z);
		}
	}

	// 行列更新
	WorldTrnasformUpdate(worldTransform_);

	// --- 以降は必要なら弾やタイマーを復帰 ---
	// for (EnemyBullet* b : bullets_) { b->Update(); }
	// bullets_.remove_if([](EnemyBullet* b){ if (b->IsDead()){ delete b; return true; } return false; });

	#ifdef DEBUG
	ImGui::Text("EnemyPos: (%.2f, %.2f, %.2f)", worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z);
#endif // DEBUG
}


void Enemy::SetState(EnemyState* newState) {
	// フェーズ変更時に発射予約を中断
	timedCalls_.clear(); // これで発射予約をすべて破棄

	if (state_) {
		delete state_;
	}
	state_ = newState;
	state_->Enter(this);
}


void Enemy::Move(const Vector3& velocity) {
	worldTransform_.translation_ += velocity;
	WorldTrnasformUpdate(worldTransform_);
}

const Vector3& Enemy::GetPosition() const { return worldTransform_.translation_; }

void Enemy::Draw(const Camera& camera) {
	OutputDebugStringA("[Enemy] Draw\n");
	model_->Draw(worldTransform_, camera, textureHandle_);
	for (EnemyBullet* bullet_ : bullets_) {
		bullet_->Draw(camera); // 弾の描画もここで
	}



}
