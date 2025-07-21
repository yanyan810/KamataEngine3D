// EnemyState_Approach.cpp
#include "EnemyState_Approach.h"
#include "Enemy.h"
#include "EnemyState_Leave.h" // 次の状態に遷移するため

void EnemyState_Approach::Update(Enemy* enemy) {
	const float kMoveSpeed = 0.5f;
	KamataEngine::Vector3 velocity = {0, 0, -kMoveSpeed};

	enemy->Move(velocity);

	if (enemy->GetPosition().z < 0.0f) {
		enemy->SetState(new EnemyState_Leave());
	}
}
