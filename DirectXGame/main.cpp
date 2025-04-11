#include <Windows.h>
#include "KamataEngine.h"
#include "GameScene.h"

using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	
	//エンジンの初期化
	KamataEngine::Initialize(L"LE2B_25_ミヤザワ_ハルヒ_AL3");

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	//ゲームシーンのインスタンス生成
	GameScene* gameScene = new GameScene();
	// ゲームシーンの初期化
	gameScene->Initialize();

	//メインループ
	while (true) {

		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}

		//ゲームシーンの更新
		gameScene->Update();

		//描画開始
		dxCommon->PreDraw();

		//ゲームシーンの描画
		gameScene->Draw();

		//描画終了
		dxCommon->PostDraw();
	}

	// ゲームシーンの解放
	delete gameScene;

	//nullptrの代入
	gameScene = nullptr;

	KamataEngine::Finalize();

	return 0;
}
