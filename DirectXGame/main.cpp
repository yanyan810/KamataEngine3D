#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include <Windows.h>
using namespace KamataEngine;

#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")

void ReportLiveObjects() {
	IDXGIDebug1* pDebug = nullptr;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug)))) {
		pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		pDebug->Release();
	}
}

enum class Scene { Title, Game };

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	KamataEngine::Initialize(L"LE2B_25_ミヤザワ_ハルヒ_AL3");

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	ImGuiManager* imgui = ImGuiManager::GetInstance();

	Scene scene = Scene::Title;
	TitleScene* titleScene = new TitleScene();
	titleScene->Initialize();

	GameScene* gameScene = nullptr;

	while (true) {
		if (KamataEngine::Update()) {
			break;
		}

		imgui->Begin();

		// ===== Update =====
		switch (scene) {
		case Scene::Title:
			titleScene->Update();
			if (titleScene->IsFinished()) {
				delete titleScene;
				titleScene = nullptr;
				scene = Scene::Game;
				gameScene = new GameScene();
				gameScene->Initialize();
			}
			break;
		case Scene::Game:
			gameScene->Update();
			if (gameScene->IsFinished()) {
				delete gameScene;
				gameScene = nullptr;
				scene = Scene::Title;
				titleScene = new TitleScene();
				titleScene->Initialize();
			}
			break;
		}

		imgui->End();

		// ===== Draw =====
		dxCommon->PreDraw();

		switch (scene) {
		case Scene::Title:
			titleScene->Draw();
			break;
		case Scene::Game:
			gameScene->Draw();
			break;
		}

		AxisIndicator::GetInstance()->Draw();
		imgui->Draw();
		dxCommon->PostDraw();

		if (KamataEngine::Input::GetInstance()->PushKey(DIK_ESCAPE)) {
			ReportLiveObjects();
			return 0;
		}
	}

	// 後始末（どちらか生きている方のみ解放）
	delete titleScene;
	titleScene = nullptr;
	delete gameScene;
	gameScene = nullptr;

	ReportLiveObjects();
	KamataEngine::Finalize();
	return 0;
}
