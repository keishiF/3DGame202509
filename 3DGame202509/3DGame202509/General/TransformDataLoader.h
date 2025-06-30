#pragma once
#include <DxLib.h>
#include <string>
#include <vector>
// オブジェクトの情報を格納する構造体
struct ObjectData {
    std::string name; // オブジェクト名
    VECTOR pos;       // 位置
    VECTOR rot;       // 回転（ラジアン）
    VECTOR scale;     // スケール
};

class TransformDataLoader
{
public:
    TransformDataLoader();
	virtual ~TransformDataLoader();

    /// <summary>
    /// CSVファイルからオブジェクトのTranceform情報を取得
    /// </summary>
    /// <param name="fileName">csv</param>
    /// <returns></returns>
    static std::vector<ObjectData> LoadDataCSV(const char* fileName);
};

