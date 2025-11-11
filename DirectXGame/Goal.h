#pragma once
#include "KamataEngine.h" // KamataEngine::Vector3
class Player;

class Goal {
public:
	enum class Type { LineX, Box };

	// --- セットアップ ---
	void InitializeLineX(float x, float margin = 0.0f) {
		type_ = Type::LineX;
		lineX_ = x;
		margin_ = margin;
		reached_ = false;
	}
	void InitializeBox(const KamataEngine::Vector3& center, const KamataEngine::Vector3& halfExtents) {
		type_ = Type::Box;
		center_ = center;
		half_ = halfExtents;
		reached_ = false;
	}

	// --- 判定更新（毎フレ呼ぶ）---
	void Update(const Player& player);

	// --- 結果取得 ---
	bool IsReached() const { return reached_; }
	void Reset() { reached_ = false; }

	// 任意：一度到達したらリセットするまで再判定しない
	void SetOneShot(bool v) { oneShot_ = v; }

	// 任意：エディタ用にゴールの種類や形状を取れるように
	Type GetType() const { return type_; }

	// --- （任意）デバッグ描画 ---
	void DrawDebug3D() const; // 中身は空でもOK。後で好きな実装に置換。

private:
	// 形状
	Type type_ = Type::LineX;

	// LineX 用
	float lineX_ = 0.0f;
	float margin_ = 0.0f; // プレイヤー中心からの許容

	// Box 用（AABB：center ± half）
	KamataEngine::Vector3 center_{0, 0, 0};
	KamataEngine::Vector3 half_{0.5f, 0.5f, 0.5f};

	// 状態
	bool reached_ = false;
	bool oneShot_ = true;
};
