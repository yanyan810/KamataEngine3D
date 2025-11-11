#define NOMINMAX
#include "CameraController.h"
#include "Player.h"
#include <algorithm>
void CameraController::Initialize(KamataEngine::Camera* camera) { camera_ = camera; }
KamataEngine::Vector3 Lerp(KamataEngine::Vector3 a, KamataEngine::Vector3 b, float t) { return {a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t}; }

void CameraController::Update() {

	// === 安全策（ターゲット未設定なら何もしない） ===
	if (!camera_ || !target_) {
		return;
	}

	// プレイヤーの現在ワールド位置（Vector3）
	const KamataEngine::Vector3 playerPos = target_->GetWorldPosition();

	// 目標カメラ位置（X のみ追従）
	goalPosition_.x = playerPos.x + targetOffset_.x;
	goalPosition_.y = camera_->translation_.y; // 既存値を維持
	goalPosition_.z = camera_->translation_.z; // 既存値を維持

	// スムーズ追従（Xのみ）
	camera_->translation_.x = camera_->translation_.x + (goalPosition_.x - camera_->translation_.x) * kInterpolationRate;

	// X 軸のマージン＆範囲制限のみ有効
	camera_->translation_.x = std::max(camera_->translation_.x, playerPos.x + marginArea_.left);
	camera_->translation_.x = std::min(camera_->translation_.x, playerPos.x + marginArea_.right);
	camera_->translation_.x = std::clamp(camera_->translation_.x, movableArea_.left, movableArea_.right);

	// 行列を更新する
	camera_->UpdateMatrix();
}

void CameraController::Reset() {
	// カメラのリセット処理
}