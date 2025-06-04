#define NOMINMAX
#include "Player.h"
#include "MapChipFiled.h"
#include "WorldTransformClass.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <numbers>

float Lerp(float a, float b, float t) { return a + (b - a) * t; }

KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Player::Corner corner) {
	KamataEngine::Vector3 offsetTable[Player::Corner::kNumCenter] = {
	    {+Player::kWidth / 2.0f, -Player::kHeight / 2.0f, 0.0f}, // kRightBottom
	    {-Player::kWidth / 2.0f, -Player::kHeight / 2.0f, 0.0f}, // kLeftBottom
	    {+Player::kWidth / 2.0f, +Player::kHeight / 2.0f, 0.0f}, // kRightTop
	    {-Player::kWidth / 2.0f, +Player::kHeight / 2.0f, 0.0f}, // kLeftTop
	};

	KamataEngine::Vector3 offset = offsetTable[static_cast<uint32_t>(corner)];
	return {center.x + offset.x, center.y + offset.y, center.z + offset.z};
}

void Player::MapCllisionCheckUp(Player::ColisionMapInfo& info) {
	// 上昇あり？
	if (info.velosity_.y <= 0) {
		return;
	}

	Vector3 worldPos = {worldTransform_.translation_.x + info.velosity_.x, worldTransform_.translation_.y + info.velosity_.y, worldTransform_.translation_.z + info.velosity_.z};

	std::array<Vector3, Player::Corner::kNumCenter> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldPos, static_cast<Corner>(i));
	}

	MapChipField::MapChipType mapChipType;
	MapChipField::MapChipType mapChipTypeNext;
	// 真上のあたり判定を行う
	bool hit = false;
	// (自キャラの)左上の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipField::MapChipType::kBlock && mapChipTypeNext != MapChipField::MapChipType::kBlock) {
		hit = true;
	}
	// 右上の判定
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipField::MapChipType::kBlock && mapChipTypeNext != MapChipField::MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightTop]);
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexByPosition(worldTransform_.translation_);

		if (indexSetNow.yIndex != indexSet.yIndex) {
			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// float newTopY = positionsNew[kRightTop].y - kHeight / 2.0f;
			info.velosity_.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
			// info.velosity.y = std::max(0.0f, rect.bottom - newTopY + kBlank);
			DebugText::GetInstance()->ConsolePrintf("info.move.y = %f", rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f));

			// 天井に当たったことを記録する
			info.celling = true;
		}
	}
}
void Player::MapCllisionCheckDown(Player::ColisionMapInfo& info) {

	std::array<Vector3, Player::Corner::kNumCenter> positionsNew;

	Vector3 worldPos = {worldTransform_.translation_.x + info.velosity_.x, worldTransform_.translation_.y + info.velosity_.y, worldTransform_.translation_.z + info.velosity_.z};

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldPos, static_cast<Player::Corner>(i));
	}
	// 下降あり？

	if (info.velosity_.y >= 0) {
		return;
	}
	MapChipField::MapChipType mapChipType;
	MapChipField::MapChipType mapChipTypeNext;
	// 真上のあたり判定を行う
	bool hit = false;

	// 「移動後」のキャラ中心座標
	Vector3 moveLeftBottom = {positionsNew[kLeftBottom].x, positionsNew[kLeftBottom].y - kLandingAdjust, positionsNew[kLeftBottom].z};
	Vector3 moveRightBottom = {positionsNew[kRightBottom].x, positionsNew[kRightBottom].y - kLandingAdjust, positionsNew[kRightBottom].z};

	// 左下の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipField::MapChipType::kBlock && mapChipTypeNext != MapChipField::MapChipType::kBlock) {
		hit = true;
	}
	// 右下の判定
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipField::MapChipType::kBlock && mapChipTypeNext != MapChipField::MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightBottom]);
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexByPosition(worldTransform_.translation_);

		if (indexSetNow.yIndex != indexSet.yIndex) {

			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.velosity_.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + (kHeight / 2.0f + kBlank));
			// 地面に当たったことを記録する
			info.landing = true;
		}
	}
}

void Player::MapCllisionCheckRight(Player::ColisionMapInfo& info) {

	std::array<Vector3, Player::Corner::kNumCenter> positionsNew;

	Vector3 worldPos = {worldTransform_.translation_.x + info.velosity_.x, worldTransform_.translation_.y + info.velosity_.y, worldTransform_.translation_.z + info.velosity_.z};

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldPos, static_cast<Player::Corner>(i));
	}

	//右移動あり？
	if (info.velosity_.x <= 0.0f) {
		return;
	}

		MapChipField::MapChipType mapChipType;
	MapChipField::MapChipType mapChipTypeNext;

	bool hit = false;

	// 「移動後」のキャラ中心座標
	Vector3 moveRightTop = {positionsNew[kRightTop].x, positionsNew[kRightTop].y - kLandingAdjust, positionsNew[kRightTop].z};
	Vector3 moveRightBottom = {positionsNew[kRightBottom].x, positionsNew[kRightBottom].y - kLandingAdjust, positionsNew[kRightBottom].z};


	MapChipField::IndexSet indexSet;

	//右上の判定
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex-1, indexSet.yIndex);
	if (mapChipType == MapChipField::MapChipType::kBlock && mapChipTypeNext != MapChipField::MapChipType::kBlock) {
		hit = true;
	}


	// 右下の判定
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex-1, indexSet.yIndex);
	if (mapChipType == MapChipField::MapChipType::kBlock && mapChipTypeNext != MapChipField::MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightTop]);
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexByPosition(worldTransform_.translation_);
		//// めり込み先ブロックの範囲矩形
		//
		if (indexSetNow.xIndex != indexSet.xIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// info.move.x = std::min(0.0f, rect.left - (worldTransform_.translation_.x + (kWidth / 2.0f + kBlank)));
			info.velosity_.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
			// 壁に当たったことを記録する
			info.isWall = true;
		}
	}


}

void Player::MapCllisionCheckLeft(Player::ColisionMapInfo& info) {

	std::array<Vector3, Player::Corner::kNumCenter> positionsNew;

	Vector3 worldPos = {worldTransform_.translation_.x + info.velosity_.x, worldTransform_.translation_.y + info.velosity_.y, worldTransform_.translation_.z + info.velosity_.z};

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldPos, static_cast<Player::Corner>(i));
	}

	// 右移動あり？
	if (info.velosity_.x > 0.0f) {
		return;
	}

	MapChipField::MapChipType mapChipType;
	MapChipField::MapChipType mapChipTypeNext;

	bool hit = false;

	// 「移動後」のキャラ中心座標
	Vector3 moveLeftTop = {positionsNew[kLeftTop].x, positionsNew[kLeftTop].y - kLandingAdjust, positionsNew[kLeftTop].z};
	Vector3 moveLefttBottom = {positionsNew[kLeftBottom].x, positionsNew[kLeftBottom].y - kLandingAdjust, positionsNew[kLeftBottom].z};

	MapChipField::IndexSet indexSet;

	//左上の判定
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipField::MapChipType::kBlock && mapChipTypeNext != MapChipField::MapChipType::kBlock) {
		hit = true;
	}

	// 左下の判定
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipField::MapChipType::kBlock && mapChipTypeNext != MapChipField::MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftTop]);

		MapChipField::IndexSet indexSetNow;

		indexSetNow = mapChipField_->GetMapChipIndexByPosition(worldTransform_.translation_);

		// めり込み先ブロックの範囲矩形
		if (indexSetNow.xIndex != indexSet.xIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.velosity_.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
			// 壁に当たったことを記録する
			info.isWall = true;
		}
	}
}

void Player::OnSwichGround(const Player::ColisionMapInfo& info) {
	// 自キャラが接地状態？
	if (onGround_) {

		// ジャンプ開始
		if (velosity_.y > 0.0f) {
			onGround_ = false;
		}

		std::array<Vector3, Player::Corner::kNumCenter> positionsNew;
		Vector3 worldPos = {worldTransform_.translation_.x + info.velosity_.x, worldTransform_.translation_.y + info.velosity_.y, worldTransform_.translation_.z + info.velosity_.z};

		for (uint32_t i = 0; i < positionsNew.size(); ++i) {
			positionsNew[i] = CornerPosition(worldPos, static_cast<Player::Corner>(i));
		}
		MapChipField::MapChipType mapChipType;
		// 真上のあたり判定を行う
		bool hit = false;
		// 左下の判定
		MapChipField::IndexSet indexSet;
		indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftBottom]);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		if (mapChipType == MapChipField::MapChipType::kBlock) {
			hit = true;
		}
		// 右下の判定
		indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightBottom]);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		if (mapChipType == MapChipField::MapChipType::kBlock) {
			hit = true;
		}

		// 落下開始
		if (!hit) {
			// 空中状態に切り替える
			onGround_ = false;
		}

	} else {

		// 着地フラグ
		if (info.landing) {
			// 着地状態に切り替える(落下を止める)
			onGround_ = true;
			// 着地時にX座標を減衰
			velosity_.x *= (1.0f - kAttenuationLanding);
			// Y速度をゼロにする
			velosity_.y = 0.0f;
		}
	}
}

void Player::IsWall(Player::ColisionMapInfo& info) {
	if (info.isWall) {
		info.velosity_.x *= (1.0f - kAttenuationWall);
	}
}

void MoveAccordingResult(WorldTransform& worldTransform, const Player::ColisionMapInfo& info) {

	worldTransform.translation_.x += info.velosity_.x;
	worldTransform.translation_.y += info.velosity_.y;
	worldTransform.translation_.z += info.velosity_.z;
}

void Player::IsCelling(Player::ColisionMapInfo& info) {

	if (info.landing) {
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		info.velosity_.y = 0;
	}
}

void Player::MapCollisionCheck(ColisionMapInfo& info) {
	MapCllisionCheckUp(info);
	MapCllisionCheckDown(info);
	MapCllisionCheckRight(info);
	MapCllisionCheckLeft(info);
}

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

void Player::PlayerMove() {

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

	// 接地状態
	if (onGround_) {

		if (KamataEngine::Input::GetInstance()->PushKey(DIK_UP)) {
			// ジャンプ
			if (onGround_) {
				velosity_.y = kJumpAcceleration;
				// onGround_ = false;
			}
		}
		// 空中
	} else {

		// 着地

		velosity_.x += 0.0f;
		velosity_.y -= kGravityAcceleration;
		velosity_.z += 0.0f;
		// 落下速度制限
		velosity_.y = std::max(velosity_.y, -kLimitFallSpeed);
	}
}

void Player::Updata() {
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();

	// 移動処理
	PlayerMove();

	ColisionMapInfo collisionMapInfo;
	collisionMapInfo.velosity_ = velosity_;
	// マップ衝突判定
	MapCollisionCheck(collisionMapInfo);
	OnSwichGround(collisionMapInfo);
	IsCelling(collisionMapInfo);
	IsWall(collisionMapInfo);
	// 判定結果を反映させて移動させる
	velosity_ = collisionMapInfo.velosity_;

	ImGui::Text("isWall  : %s", collisionMapInfo.isWall ? "true" : "false");
	ImGui::Text("velsity.y  : %f", velosity_.y);

	worldTransform_.translation_.x += velosity_.x;
	worldTransform_.translation_.y += velosity_.y;
	worldTransform_.translation_.z += velosity_.z;
}

void Player::Draw() { model_->Draw(worldTransform_, *camera_, textureHandle_); }

void Player::SetPosition(const KamataEngine::Vector3& position) { worldTransform_.translation_ = position; }

const KamataEngine::WorldTransform& Player::GetWoldTransform() const { return worldTransform_; }
