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

KamataEngine::Vector3 Player::MapClisionCheckUp(Player::ColisionMapInfo& info) {
	// 上向きでなければ無視
	if (info.velosity_.y <= 0.0f) {
		return info.velosity_;
	}

	// 「移動後」のキャラ中心座標
	KamataEngine::Vector3 movedCenter = {worldTransform_.translation_.x + info.velosity_.x,
	    worldTransform_.translation_.y + info.velosity_.y,
	    worldTransform_.translation_.z + info.velosity_.z,
	
	};

	// 左上 & 右上コーナーのワールド座標
	KamataEngine::Vector3 lu = CornerPosition(movedCenter, Corner::kLeftTop);
	KamataEngine::Vector3 ru = CornerPosition(movedCenter, Corner::kRightTop);

	// 2 点のどちらかでもブロックと重なっていれば衝突
	bool hit = false;
	MapChipField::IndexSet hitIndex{};

	for (const KamataEngine::Vector3& p : {lu, ru}) {
		auto idx = mapChipField_->GetMapChipIndexByPosition(p);
		if (mapChipField_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex) == MapChipField::MapChipType::kBlock) {
			hit = true;
			hitIndex = idx; // 最後に当たった方で OK
		}
	}

	if (hit) {
		// 衝突したブロックの矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(hitIndex.xIndex, hitIndex.yIndex);

		// キャラの上端がブロックの下端に接触するように押し戻す
		float charTopAfterMove = movedCenter.y + kHeight / 2.0f; // 上端
		float pushBack = rect.bottom - charTopAfterMove;         // 押し戻し量 (負数)

		info.velosity_.y -= pushBack; // y 方向速度に補正を加える
		info.isCeiling = true;
	}
	return info.velosity_;
}

// KamataEngine::Vector3 MapClisionCheckDown(const Player& player, MapChipField* mapChipField, const Player::ColisionMapInfo& info) {
//	std::array<KamataEngine::Vector3, Player::kNumCenter> positionsNew;
//
//	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
//		positionsNew[i] = CornerPosition(player.worldTransform_.translation_, static_cast<Player::Corner>(i));
//	}
//
//	if (info.velosity_.y <= 0.0f) {
//		return {0.0f, 0.0f, 0.0f};
//	}
//
//	// 下方向の当たり判定を行う
//	MapChipField::MapChipType mapChipType;
//
//	bool hit = false;
//
//	MapChipField::IndexSet indexSet;
//	// 左下点の当たり判定
//	indexSet = mapChipField->GetMapChipIndexByPosition(positionsNew[Player::kLeftBottom]);
//	mapChipType = mapChipField->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
//	if (mapChipType == MapChipField::MapChipType::kBlock) {
//		hit = true;
//	}
//	//右下点の当たり判定
//	indexSet = mapChipField->GetMapChipIndexByPosition(positionsNew[Player::kRightBottom]);
//	mapChipType = mapChipField->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
//	if (mapChipType == MapChipField::MapChipType::kBlock) {
//		hit = true;
//	}
//
//	  return info.velosity_; // 衝突していないならそのまま返す
//
// }

// KamataEngine::Vector3 MapClisionCheckSide(const Player& player, const Player::ColisionMapInfo& info) {
//	std::array<KamataEngine::Vector3, Player::kNumCenter> positionsNew;
//
//	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
//		positionsNew[i] = CornerPosition(player.worldTransform_.translation_, static_cast<Player::Corner>(i));
//	}
//
//	if (info.velosity_.x == 0.0f) {
//		return {0.0f, 0.0f, 0.0f};
//	}
//
//	return info.velosity_.x;
// }

void MoveAccordingResult(WorldTransform& worldTransform, const Player::ColisionMapInfo& info) {

	worldTransform.translation_.x += info.velosity_.x;
	worldTransform.translation_.y += info.velosity_.y;
	worldTransform.translation_.z += info.velosity_.z;
}

void IsCelling(Player::ColisionMapInfo& info) {

	if (info.isCeiling) {
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		info.velosity_.y = 0;
	}
}

void Player::MapCollisionCheck(ColisionMapInfo& info) { MapClisionCheckUp(info); }

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
	bool landing = false;

	// 地面との当たり判定
	// 下降中？
	if (velosity_.y < 0.0f) {

		// Y座標が地面以下になったら着地
		if (worldTransform_.translation_.y < 2.0f) {
			landing = true;
		}
	}

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

	// 接地状態
	if (onGround_) {

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
	// 判定結果を反映させて移動させる

	velosity_ = collisionMapInfo.velosity_;

	worldTransform_.translation_.x += velosity_.x;
	worldTransform_.translation_.y += velosity_.y;
	worldTransform_.translation_.z += velosity_.z;
}

void Player::Draw() { model_->Draw(worldTransform_, *camera_, textureHandle_); }

void Player::SetPosition(const KamataEngine::Vector3& position) { worldTransform_.translation_ = position; }

const KamataEngine::WorldTransform& Player::GetWoldTransform() const { return worldTransform_; }
