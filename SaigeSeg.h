#pragma once

#include "CompileEnv.h"

#define MAX_SEGMENT_CLASS_NUM			10
#define MAX_DEFFET_CNT					50

#ifdef __SAIGE1_USE
	#include <limits.h>
	#include <stdlib.h>
	#include <string>
	#include <vector>
	#include "Saige/include_Ver10/SegmentationTestDLL.h"
#endif

#ifdef __SAIGE2_USE
	#include <limits.h>
	#include <stdlib.h>
	#include <string>
	#include <vector>
	#include "SaigeVisionAPI.h"
	#include "SaigeVisionUtil.h"
#endif

enum AI_MODEL_STATE
{
	AI_MODEL_UNLOAD = 0,
	AI_MODEL_LOADING = 1,
	AI_MODEL_COMPLETE_LOAD = 2
};

#define AIMODEL_MISS_MATCH_ERRROR		LICENSE_EXPIRED + 1
#define AIMODEL_FILE_NAME_ERROR			LICENSE_EXPIRED + 2

typedef struct _tagSAIGE_PARAM_CLS
{
	int							nGpuIdx;
	int							nVerbosity;
	int							nClass;
	int							nMaxClassName;
	std::vector<std::string>	classNames;
	std::vector<int>			classColor;
	std::vector<int>			additionalScoreOutput;
	std::vector<int>			scoreOutput;
	std::vector<int>			resultScore;
} SAIGE_PARAM_SEG;

typedef struct stAIBLOBClassInfo
{
	bool isdefect{ false };
	unsigned int class_id{ 0 };
	unsigned int area{ 0 };
	unsigned int center_x{ 0 };
	unsigned int center_y{ 0 };
	double segmentscore[3]{ 0 };
	int contour_size{ 0 };
	std::vector<CPoint> contour_points;
	CRect rtRect{ CRect() };
} stAIBLOBClassInfo;

class CSaigeSeg
{
public:
	CSaigeSeg();
	virtual ~CSaigeSeg();

	int LoadModel(const wchar_t* szModelPath);
	void CloseModel();

	int RunInspection(const cv::Mat& cvInputImage, std::vector<stAIBLOBClassInfo>& vtBlobOut);
	int RunInspection(const cv::Mat& cvInputImage, std::vector<stAIBLOBClassInfo>& vtBlobOut,
		const std::vector<int>& vMinScoreThres, const std::vector<int>& vPixelSizeThres);

	int GetClassCount() const { return m_nSegClassNum; }
	CString GetErrorInfo(int nErrorType);

private:
	int CreateSegModel(const wchar_t* szModelPath,
		const std::vector<int>* pvMinScoreThres = nullptr,
		const std::vector<int>* pvPixelSizeThres = nullptr);

#ifdef __SAIGE1_USE
	SegTestHandle m_pSegHandle;
	SegTestResult m_pAISegResult;
	std::vector<SR_SegmentedObject> m_segObj;
#endif

#ifdef __SAIGE2_USE
	SaigeInferenceHandle m_pHandle;
	SaigeInferenceResult m_pAIResult;
	SaigeModelInfo* m_pAIModelInfo;
	SaigeInferenceOption* m_pAIOptions;
#endif

	int m_nSegModelState;
	int m_nSegModelInit;
	int m_nSegClassNum;
	int m_nImgWidth;
	int m_nImgHeight;

	std::string m_strTrainedSegModelPath;
	std::vector<int> m_vSegMinScoreThres;
	std::vector<int> m_vSegPixelSizeThres;
};
