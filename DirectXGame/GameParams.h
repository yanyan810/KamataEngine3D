// 共通パラメータ（バランス調整用）
#pragma once


struct GameParams {
	int playerMaxHP = 20;
	int maxAmmo = 1000; // 弾の総数（回復なし）
	float playerRadius = 1.0f;
	float bulletRadius = 4.0f;
	float enemyRadius = 1.0f;
	float invincibleTimeAfterHit = 1.0f; // 被弾後の無敵(秒)

	float moveLimitMinX = -5.0f;
	float moveLimitMaxX = 5.0f;
	float moveLimitMinY = -2.5f;
	float moveLimitMaxY = 2.5f;

};

inline GameParams gParams; // グローバル変数として定義
