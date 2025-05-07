#include "DxLib.h"
#include "Application.h"

// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Application& app = Application::GetInstance();

#ifdef _DEBUG
	AllocConsole();                                        // コンソール
	FILE* out = 0; freopen_s(&out, "CON", "w", stdout); // stdout
	FILE* in = 0; freopen_s(&in, "CON", "r", stdin);   // stdin
#endif

	// アプリケーションの初期化
	if (!app.Init())
	{
		return -1;
	}

	// メインループ
	app.Run();

	// 後処理
	app.Terminate();

	return 0;				// ソフトの終了 
}