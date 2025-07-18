#include <Windows.h>
#include "KamataEngine.h"
#include "GameScene.h"
#include "TitleScene.h"
using namespace KamataEngine;




GameScene* gameScene = nullptr; // ゲームシーンのインスタンス





// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	
	//エンジンの初期化
	KamataEngine::Initialize(L"LE2B_25_ミヤザワ_ハルヒ_TR");

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	//ImGuiManagerインスタンスの取得
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	
gameScene = new GameScene();
	// ゲームシーンの初期化
	gameScene->Initialize();	

	/*#ifdef _DEBUG
	scene = Scene::kGame;

	gameScene = new GameScene();
	gameScene->Initialize();
#else
	scene = Scene::kTitle;

	titleScene = new TitleScene();
	titleScene->Initialize();
#endif*/
	



	//メインループ
	while (true) {

		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}

		imguiManager->Begin();

		

	
		

		//ImGui受付終了
		imguiManager->End();

		//描画開始
		dxCommon->PreDraw();

		
		//軸表示の描画
		AxisIndicator::GetInstance()->Draw();

		//ImGui描画
		imguiManager->Draw();

		//描画終了
		dxCommon->PostDraw();

		if (KamataEngine::Input::GetInstance()->PushKey(DIK_ESCAPE)) {
			return 0; // 左キーが押されたら終了
		}

	}

	// ゲームシーンの解放
	delete gameScene;
	
	//nullptrの代入
	gameScene = nullptr;

	KamataEngine::Finalize();

	return 0;
}
