#include "Goal.h"
#include "Player.h"
#include <algorithm>

using KamataEngine::Vector3;

static inline bool PointInAABB(const Vector3& p, const Vector3& c, const Vector3& h) {
	return (p.x >= c.x - h.x && p.x <= c.x + h.x) && (p.y >= c.y - h.y && p.y <= c.y + h.y) && (p.z >= c.z - h.z && p.z <= c.z + h.z);
}

void Goal::Update(const Player& player) {
	if (oneShot_ && reached_)
		return; // 既に到達済みならスキップ

	const Vector3 pos = player.GetWorldPosition();

	switch (type_) {
	case Type::LineX: {
		// X が lineX_ - margin_ を越えたら達成
		if (pos.x >= (lineX_ - margin_)) {
			reached_ = true;
		}
		break;
	}
	case Type::Box: {
		if (PointInAABB(pos, center_, half_)) {
			reached_ = true;
		}
		break;
	}
	}
}

void Goal::DrawDebug3D() const {
	// ★必要になった時に実装：
	// 例：Boxならワイヤーフレームキューブを描く、LineXなら細長い板メッシュを置く 等
	// 今は空実装でOK（ビルド通すため）
}
