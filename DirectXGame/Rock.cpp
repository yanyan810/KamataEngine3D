#include "Rock.h"
using namespace KamataEngine;

void Rock::SetGraphics(Model* model, Camera* camera) {
	s_model_ = model;
	s_camera_ = camera;
}

void Rock::Initialize(const Vector3& min, const Vector3& max) {
	aabb_.min = min;
	aabb_.max = max;
	// 中心とサイズを計算
	Vector3 size = {aabb_.max.x - aabb_.min.x, aabb_.max.y - aabb_.min.y, aabb_.max.z - aabb_.min.z};
	Vector3 center = {(aabb_.min.x + aabb_.max.x) * 0.5f, (aabb_.min.y + aabb_.max.y) * 0.5f, (aabb_.min.z + aabb_.max.z) * 0.5f};
	// 行列更新
	world_.Initialize();
	world_.scale_ = size;
	world_.rotation_ = {0, -1.55f, 0};
	world_.translation_ = center;
	world_.matWorld_ = MakeAffineMatrix(world_.scale_, world_.rotation_, world_.translation_);
	world_.TransferMatrix();
}

void Rock::OnCollision() { isDead_ = true; }

void Rock::DrawWithAlpha(float a) {
	if (!s_model_ || !s_camera_)
		return;

	// 0..1 クランプ
	if (a < 0.0f)
		a = 0.0f;
	if (a > 1.0f)
		a = 1.0f;

	// --- 方式A: ModelにSetColor(Vector4)がある場合 ---
	// 使えるならこのままが一番簡単（白×αで乗算）
	// s_model_->SetColor({1, 1, 1, a});
	// s_model_->Draw(world_, *s_camera_);
	// s_model_->SetColor({1, 1, 1, 1});

	// --- 方式B: Drawの直前にマテリアル定数バッファ等でαだけ一時的に上書きする ---
	// あなたのエンジンAPIに合わせて、材質カラー or 乗算カラーの α を一時反映してください。
	// 例:
	// auto old = s_model_->GetMulColor();               // {r,g,b,a}
	// s_model_->SetMulColor({old.x, old.y, old.z, a});
	s_model_->Draw(world_, *s_camera_);
	// s_model_->SetMulColor(old);

	// SetColor/SetMulColor 等が無い場合は、モデルのマテリアルCBに α を一時的に書き込む実装に置き換えてください。
}


// Rock.cpp
void Rock::Update(float dt) {

	KamataEngine::Vector3 velocity_ = {0.0f, 0.0f, -50.0f};

	// 移動（Z軸）
	aabb_.min.z += velocity_.z * dt;
	aabb_.max.z += velocity_.z * dt;

	// 中心を再計算
	Vector3 size = {aabb_.max.x - aabb_.min.x, aabb_.max.y - aabb_.min.y, aabb_.max.z - aabb_.min.z};
	Vector3 center = {(aabb_.min.x + aabb_.max.x) * 0.5f, (aabb_.min.y + aabb_.max.y) * 0.5f, (aabb_.min.z + aabb_.max.z) * 0.5f};

	// 行列更新
	world_.translation_ = center;
	world_.scale_ = size;
	world_.matWorld_ = MakeAffineMatrix(world_.scale_, world_.rotation_, world_.translation_);
	world_.TransferMatrix();
}



void Rock::Draw() {
	if (!s_model_ || !s_camera_)
		return;

	Vector3 size = {aabb_.max.x - aabb_.min.x, aabb_.max.y - aabb_.min.y, aabb_.max.z - aabb_.min.z};
	Vector3 center = {(aabb_.min.x + aabb_.max.x) * 0.5f, (aabb_.min.y + aabb_.max.y) * 0.5f, (aabb_.min.z + aabb_.max.z) * 0.5f};


	s_model_->Draw(world_, *s_camera_);
}
