#pragma once

class Enemy; // 前方宣言

class EnemyState {
public:
	virtual ~EnemyState() = default;

	// 状態ごとの処理
	virtual void Update(Enemy* enemy) = 0;
};
