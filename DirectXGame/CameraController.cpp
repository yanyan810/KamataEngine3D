#define NOMINMAX
#include "CameraController.h"
#include "Player.h"
#include <algorithm>
void CameraController::Initialize(KamataEngine::Camera* camera) { camera_ = camera; }
KamataEngine::Vector3 Lerp(KamataEngine::Vector3 a, KamataEngine::Vector3 b, float t) { return {a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t}; }

void CameraController::Update() {
	if (!camera_ || !target_)
		return;
	const auto playerPos = target_->GetWorldPosition();

	if (mode_ == Mode::SideScrollByPlayer) {

		// === ★ スクロール凍結中：カメラ固定＋プレイヤーは画面端まで可動 ===
		if (scrollFrozen_) {
			camera_->translation_.x = frozenCamX_;

			// ★停止中は広めの帯でロック（画面半分 ≒ 画面端まで）
			float minX = frozenCamX_ - frozenPlayerHalf_;
			float maxX = frozenCamX_ + frozenPlayerHalf_;
			if (target_)
				target_->EnableLockX(minX, maxX);

			camera_->UpdateMatrix();
			return;
		}

		float camX = camera_->translation_.x;

		// カメラ中心基準の“デッドゾーン”左右端
		float leftEdge = camX + windowLeft_;
		float rightEdge = camX + windowRight_;

		float desiredCamX = camX;

		// 右端を越えたら、その分だけ前へ送る
		if (playerPos.x > rightEdge) {
			desiredCamX += (playerPos.x - rightEdge);
		}
		// 左端を越えたら（＝左へ出た）
		else if (playerPos.x < leftEdge) {
			if (allowBackScroll_) {
				desiredCamX += (playerPos.x - leftEdge); // 戻りスクロール許可
			} // 許可しない場合はカメラは戻さない（横スク風）
		}

		// ステージ可動範囲でクランプ
		desiredCamX = std::clamp(desiredCamX, movableArea_.left, movableArea_.right);
		camera_->translation_.x = desiredCamX;

		// 画面外に出ないようプレイヤーの X を帯でロック
		float minX = camera_->translation_.x - playHalfWidth_;
		float maxX = camera_->translation_.x + playHalfWidth_;
		if (target_)
			target_->EnableLockX(minX, maxX);

		camera_->UpdateMatrix();
		return;
	}

	// === 既存の Follow（従来処理を残す） ===
	goalPosition_.x = playerPos.x + targetOffset_.x;
	goalPosition_.y = camera_->translation_.y;
	goalPosition_.z = camera_->translation_.z;

	camera_->translation_.x = camera_->translation_.x + (goalPosition_.x - camera_->translation_.x) * kInterpolationRate;

	camera_->translation_.x = std::max(camera_->translation_.x, playerPos.x + marginArea_.left);
	camera_->translation_.x = std::min(camera_->translation_.x, playerPos.x + marginArea_.right);
	camera_->translation_.x = std::clamp(camera_->translation_.x, movableArea_.left, movableArea_.right);

	camera_->UpdateMatrix();
}

void CameraController::Reset() {
	// カメラのリセット処理
}
void CameraController::SetScrollFrozen(bool f) {
	scrollFrozen_ = f;
	if (scrollFrozen_ && camera_) {
		// 凍結開始時のカメラXを保存
		frozenCamX_ = camera_->translation_.x;
	}
}
