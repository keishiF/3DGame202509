#include "EffectManager.h"
#include "Effect.h"
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include <filesystem>
#include <string>

namespace
{
	constexpr int     kEffectPerticleLimit = 20000;
	const std::string kSourceFolder = "Data/Effect/";
	const std::string kEffectFileExtension = ".efkefc";
	constexpr float   kDefaultEffectSize = 100.0f;
}

EffectManager::EffectManager()
{
}

EffectManager::~EffectManager()
{
}

EffectManager& EffectManager::GetInstance()
{
	static EffectManager instance;
	return instance;
}

void EffectManager::Init()
{
	// 一連のおまじない
	SetUseDirect3DVersion(DX_DIRECT3D_11);
	// 初期化処理に失敗したらアプリ終了
	if (Effekseer_Init(kEffectPerticleLimit) == -1) { DxLib_End(); }

	// フルスクリーン対応
	SetChangeScreenModeGraphicsSystemResetFlag(false);
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();

	// 歪みON
	Effekseer_InitDistortion();

	// Zバッファの設定が必要なのだが、それはほかの場所で実行しているのでよし

	// エフェクトの読み込み
	LoadEffectSources();
}

void EffectManager::Update()
{
	// これで一括更新
	UpdateEffekseer3D();

	// 再生が終了したエフェクトを削除
	EffectList_t playingEffects;
	for (auto& effect : m_effects)
	{
		// 再生してない奴は置いていく
		if (!effect->IsPlaying()) continue;

		// 拾う
		playingEffects.emplace_back(effect);
	}
	// 再生中のやつだけにする
	m_effects = playingEffects;
}

void EffectManager::Draw() const
{
	// Effekseerのエフェクトはこれですべて一括描画される
	// 多分ドローコールを減らさないと負荷がやばいからだと思う
	DrawEffekseer3D();
}

std::weak_ptr<Effect> EffectManager::GenerateEffect(const std::string& fileName, const Vector3& initPos)
{
	// ソースからエフェクト生成
	const int sourceHandle = m_effectSources[fileName];
	const int effectHandle = PlayEffekseer3DEffect(sourceHandle);

	// 自前クラスのインスタンス化
	auto effect = std::make_shared<Effect>();
	effect->Init(effectHandle, initPos);

	// 保存
	m_effects.emplace_back(effect);

	// 共有
	return effect;
}

void EffectManager::ClearEffects()
{
	// 削除関数

	m_effects.clear();
}

void EffectManager::LoadEffectSources()
{
	// エフェクトファイルを、特定のフォルダから一括で読み込む

	namespace fs = std::filesystem;

	// ここでこのゲームで使う画像を一括で読み込んでしまおう
	// 指定したフォルダの中のすべての画像ファイルを読み込む
	for (const auto& file : fs::directory_iterator(kSourceFolder))
	{
		std::string path = file.path().string();
		// 相対パスではなくファイル名だけで取れるようにする
		std::string pathCopy = path;
		std::string fileName = pathCopy.erase(0, kSourceFolder.size());

		// もし正しいファイル拡張子でないとき、飛ばす
		if (file.path().extension().string() != kEffectFileExtension)
		{
#if _DEBUG
			printf("エフェクトファイルでないものを検知\n");
			printf("　パス：%s\n", path.c_str());
#endif
			continue;
		}

		// 読み込み
		const int handle = LoadEffekseerEffect(path.c_str(), kDefaultEffectSize);
		m_effectSources[fileName] = handle;

#if _DEBUG
		assert(handle != -1 && "読み込みに失敗した");
		printf("画像ファイルを読み込みました\n");
		printf("　相対パス　　：%s\n", path.c_str());
		printf("　連想配列キー：%s\n", fileName.c_str());
		printf("　ハンドル　　：%d\n", handle);
#endif
	}
}