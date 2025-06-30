#include "TransformDataLoader.h"
#include <fstream>   // ファイル読み込み用
#include <sstream>   // 文字列分解用（stringstream）stringをファイルのように扱える

namespace
{
	constexpr int kElementNum = 10;
}

std::vector<ObjectData> TransformDataLoader::LoadDataCSV(const char* fileName)
{
	//データを格納する配列
	std::vector<ObjectData> objects;

	//ファイルを開く
	std::ifstream file(fileName);
	//もしもファイルを開けなかったら
	if (!file.is_open())return objects;//空のリストを返す

	//1行ずつ読み取る用の変数
	std::string line;
	//最初のヘッダーはスキップしたい
	bool isHeader = true;

	//CSVの終わりまで読み取る
	// getlineで読み取っていく(読み取り位置（内部の「ポインタ」）は、ループのたびに前に進みます)
	//1行ずつ読み取っていき読み取る行がなくなったらfalseになる
	while (std::getline(file,line))//1行ずつ読み取る
	{
		//最初の行はスキップする(ヘッダー)
		if (isHeader)
		{
			isHeader = false;
			continue;
		}

		//行をカンマ区切りで1つずつ読み込むための準備
		std::stringstream ss(line);			//文字列をストリーム(getlineで読み取るため)に変換
		std::string part;					//分解して取り出した1要素
		std::vector<std::string> values;	//要素をまとめた配列

		//カンマ区切りで取り出していく
		//ssから,区切りで取り出していきpartに入れていく
		while (std::getline(ss, part, ',')) {
			values.emplace_back(part);           // 分割された項目をリストに追加
		}

		//要素数が10あるかチェック(10列必要)
		if (values.size() < kElementNum)continue;//ない場合は不正な行なので飛ばす

		//構造体にデータを入れていく
		ObjectData objData;
		
		//名前
		objData.name = values[0];
		//座標
		objData.pos.x = std::stof(values[1]);	//std::stofは文字列をfloatに変換する
		objData.pos.y = std::stof(values[2]);	
		objData.pos.z = std::stof(values[3]);	
		//回転
		objData.rot.x = std::stof(values[4]);	
		objData.rot.y = std::stof(values[5]);
		objData.rot.z = std::stof(values[6]);
		//大きさ
		objData.scale.x = std::stof(values[7]);	
		objData.scale.y = std::stof(values[8]);
		objData.scale.z = std::stof(values[9]);

		//配列に追加
		objects.emplace_back(objData);
	}

	return objects;
}
