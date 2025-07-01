#pragma once
#include "DxLib.h"
#include <string>
#include <vector>

struct ObjectData
{
	std::string name; // �I�u�W�F�N�g��
	VECTOR pos;		  // �ʒu
	VECTOR rot;		  // ��]
	VECTOR scale;	  // �g���
};

class TransformDataLoad
{
public:
    TransformDataLoad();
    virtual ~TransformDataLoad();

    /// <summary>
    /// CSV�t�@�C������I�u�W�F�N�g��Tranceform�����擾
    /// </summary>
    /// <param name="fileName">csv</param>
    /// <returns></returns>
    static std::vector<ObjectData> LoadDataCSV(const char* fileName);
};

