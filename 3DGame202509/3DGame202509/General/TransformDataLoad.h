#pragma once
#include "DxLib.h"
#include <string>
#include <vector>

struct ObjectData
{
	std::string name; // オブジェクト名
	VECTOR pos;		  // 位置
	VECTOR rot;		  // 回転
	VECTOR scale;	  // 拡大量
};

class TransformDataLoad
{
public:
    TransformDataLoad();
    virtual ~TransformDataLoad();

    /// <summary>
    /// CSVファイルからオブジェクトのTranceform情報を取得
    /// </summary>
    /// <param name="fileName">csv</param>
    /// <returns></returns>
    static std::vector<ObjectData> LoadDataCSV(const char* fileName);
};

