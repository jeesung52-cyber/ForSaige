#include "pch.h"
#include "SaigeSeg.h"
#include <algorithm>

CSaigeSeg::CSaigeSeg()
{
#ifdef __SAIGE1_USE
	m_pSegHandle = nullptr;
	m_pAISegResult = nullptr;
#endif

	m_nSegModelState = AI_MODEL_UNLOAD;
	m_nSegModelInit = AI_MODEL_UNLOAD;
	m_nSegClassNum = 0;
	m_nImgWidth = 0;
	m_nImgHeight = 0;
	m_strTrainedSegModelPath = "";
}

CSaigeSeg::~CSaigeSeg()
{
	CloseModel();
}

int CSaigeSeg::LoadModel(const wchar_t* szModelPath)
{
	if (!szModelPath || wcslen(szModelPath) == 0)
		return AIMODEL_FILE_NAME_ERROR;

	m_strTrainedSegModelPath = CW2A(szModelPath);
	m_nSegModelInit = AI_MODEL_LOADING;

	std::vector<int> emptyVec;
	return CreateSegModel(szModelPath, nullptr, nullptr);
}

void CSaigeSeg::CloseModel()
{
#ifdef __SAIGE1_USE
	if (m_pSegHandle)
	{
		SR_DestroySegTestHandle(m_pSegHandle);
		m_pSegHandle = nullptr;
	}
	if (m_pAISegResult)
	{
		SR_DeleteSegTestResult(m_pAISegResult);
		m_pAISegResult = nullptr;
	}
#endif

#ifdef __SAIGE2_USE
	if (m_pHandle)
	{
		SaigeDestroyInferenceHandle(m_pHandle);
		m_pHandle = nullptr;
	}
	if (m_pAIModelInfo)
	{
		delete m_pAIModelInfo;
		m_pAIModelInfo = nullptr;
	}
	if (m_pAIOptions)
	{
		delete m_pAIOptions;
		m_pAIOptions = nullptr;
	}
#endif

	m_nSegModelState = AI_MODEL_UNLOAD;
	m_nSegModelInit = AI_MODEL_UNLOAD;
}

int CSaigeSeg::CreateSegModel(const wchar_t* szModelPath,
	const std::vector<int>* pvMinScoreThres,
	const std::vector<int>* pvPixelSizeThres)
{
#ifdef __SAIGE1_USE
	if (m_pSegHandle)
	{
		SR_DestroySegTestHandle(m_pSegHandle);
		m_pSegHandle = nullptr;
	}

	eErrorType srErr;
	srErr = SR_CreateSegTestHandleByWideCharPath_ErrorType(&m_pSegHandle, szModelPath);
	if (srErr != NO_ERROR_OUT)
	{
		m_nSegModelState = srErr;
		return srErr;
	}

	srErr = SR_PrintSegTestParametersWithoutBG(m_pSegHandle);
	if (srErr != NO_ERROR_OUT)
	{
		m_nSegModelState = srErr;
		return srErr;
	}

	int numOfClass = 0;
	srErr = SR_GetSegTestNumberOfClassesWithoutBG(m_pSegHandle, &numOfClass);
	if (srErr != NO_ERROR_OUT)
	{
		m_nSegModelState = srErr;
		return srErr;
	}
	m_nSegClassNum = numOfClass;

	int maxInspHeight = 0, maxInspWidth = 0;
	srErr = SR_GetSegTestMaxInspectionSize(m_pSegHandle, &maxInspHeight, &maxInspWidth);
	if (srErr != NO_ERROR_OUT)
	{
		m_nSegModelState = srErr;
		return srErr;
	}
	m_nImgWidth = maxInspWidth;
	m_nImgHeight = maxInspHeight;

	if (m_nSegClassNum > 0)
	{
		m_vSegMinScoreThres.resize(m_nSegClassNum);
		m_vSegPixelSizeThres.resize(m_nSegClassNum);

		if (pvMinScoreThres && pvMinScoreThres->size() == m_nSegClassNum)
		{
			for (int i = 0; i < m_nSegClassNum; i++)
				m_vSegMinScoreThres[i] = (*pvMinScoreThres)[i];
		}
		else
		{
			for (int i = 0; i < m_nSegClassNum; i++)
				m_vSegMinScoreThres[i] = 100;
		}

		if (pvPixelSizeThres && pvPixelSizeThres->size() == m_nSegClassNum)
		{
			for (int i = 0; i < m_nSegClassNum; i++)
				m_vSegPixelSizeThres[i] = (*pvPixelSizeThres)[i];
		}
		else
		{
			for (int i = 0; i < m_nSegClassNum; i++)
				m_vSegPixelSizeThres[i] = 0;
		}

		srErr = SR_SetSegTestMinScoreThresholdWithoutBG(m_pSegHandle, m_nSegClassNum,
			m_vSegMinScoreThres.data(), false);
		if (srErr != NO_ERROR_OUT)
		{
			m_nSegModelState = srErr;
			return srErr;
		}

		srErr = SR_SetSegTestMinPixelSizeThresholdWithoutBG(m_pSegHandle, m_nSegClassNum,
			m_vSegPixelSizeThres.data(), false);
		if (srErr != NO_ERROR_OUT)
		{
			m_nSegModelState = srErr;
			return srErr;
		}
	}

	srErr = SR_SetSegTestIsToCalcObjectScores(m_pSegHandle, true, true);
	if (srErr != NO_ERROR_OUT)
	{
		m_nSegModelState = srErr;
		return srErr;
	}

	srErr = SR_LoadSegTestNetworkInGPU(m_pSegHandle, 0);
	if (srErr != NO_ERROR_OUT)
	{
		m_nSegModelState = srErr;
		return srErr;
	}

	m_nSegModelInit = AI_MODEL_COMPLETE_LOAD;
	m_nSegModelState = NO_ERROR_OUT;
	return NO_ERROR_OUT;

#endif

#ifdef __SAIGE2_USE
	// TODO: Saige v2 model loading
#endif

	return -1;
}

int CSaigeSeg::RunInspection(const cv::Mat& cvInputImage, std::vector<stAIBLOBClassInfo>& vtBlobOut)
{
	std::vector<int> emptyVec;
	return RunInspection(cvInputImage, vtBlobOut, emptyVec, emptyVec);
}

int CSaigeSeg::RunInspection(const cv::Mat& cvInputImage, std::vector<stAIBLOBClassInfo>& vtBlobOut,
	const std::vector<int>& vMinScoreThres, const std::vector<int>& vPixelSizeThres)
{
	vtBlobOut.clear();

	if (cvInputImage.empty())
		return EMPTY_IMAGE;

	if (m_nSegModelInit != AI_MODEL_COMPLETE_LOAD)
		return NOT_LOAD_NETWORK_IN_GPU;

#ifdef __SAIGE1_USE
	eErrorType srErr;
	int nMethod = cvInputImage.channels();
	int nSizeX = cvInputImage.cols;
	int nSizeY = cvInputImage.rows;

	if (m_pAISegResult)
	{
		SR_DeleteSegTestResult(m_pAISegResult);
		m_pAISegResult = nullptr;
	}

	srErr = SR_MakeSegTestResult_ErrorType(&m_pAISegResult);
	if (srErr != NO_ERROR_OUT)
	{
		m_nSegModelState = srErr;
		return srErr;
	}

	int numOfClass = m_nSegClassNum;

	if (vMinScoreThres.size() == numOfClass || vPixelSizeThres.size() == numOfClass)
	{
		int nChange = 0;

		if (vMinScoreThres.size() == numOfClass)
		{
			for (int i = 0; i < numOfClass; i++)
			{
				if (vMinScoreThres[i] != m_vSegMinScoreThres[i]) nChange = 1;
			}
		}

		if (vPixelSizeThres.size() == numOfClass)
		{
			for (int i = 0; i < numOfClass; i++)
			{
				if (vPixelSizeThres[i] != m_vSegPixelSizeThres[i]) nChange = 1;
			}
		}

		if (nChange != 0)
		{
			if (vMinScoreThres.size() == numOfClass)
			{
				m_vSegMinScoreThres = vMinScoreThres;
			}

			if (vPixelSizeThres.size() == numOfClass)
			{
				m_vSegPixelSizeThres = vPixelSizeThres;
				srErr = SR_SetSegTestMinPixelSizeThresholdWithoutBG(m_pSegHandle, numOfClass,
					m_vSegPixelSizeThres.data(), true);
				if (srErr != NO_ERROR_OUT)
				{
					m_nSegModelState = srErr;
					return srErr;
				}
			}
		}
	}

	int m_outputMode = SR_MULTI_CLASS_CV_CONTOUR_OUTPUT;

	switch (nMethod)
	{
	case 1:
		srErr = SR_ApplySegTestFrom8bitCharGrayArray(m_pSegHandle, cvInputImage.data,
			nSizeY, nSizeX, m_pAISegResult, m_outputMode);
		break;
	case 3:
		srErr = SR_ApplySegTestFrom8bitCharColorArray(m_pSegHandle, cvInputImage.data,
			nSizeY, nSizeX, m_pAISegResult, m_outputMode);
		break;
	default:
		return INAPPROPRIATE_INPUT;
	}

	if (srErr != NO_ERROR_OUT)
	{
		m_nSegModelState = srErr;
		return srErr;
	}

	if (m_outputMode == SR_ONE_CLASS_CV_CONTOUR_OUTPUT ||
		m_outputMode == SR_MULTI_CLASS_CV_CONTOUR_OUTPUT)
	{
		int segObjCnt = 0;
		srErr = SR_GetSegTestNumberOfSegmentedObjects(m_pAISegResult, &segObjCnt);
		if (srErr != NO_ERROR_OUT)
		{
			m_nSegModelState = srErr;
			return srErr;
		}

		if (segObjCnt > 0)
		{
			m_segObj.resize(segObjCnt);
			srErr = SR_GetSegTestSegmentedObjects(m_pAISegResult, m_segObj.data());
			if (srErr != NO_ERROR_OUT)
			{
				m_nSegModelState = srErr;
				return srErr;
			}

			double dMaxRange = 255.0;

			for (size_t i = 0; i < m_segObj.size(); i++)
			{
				int nClassIndex = (std::max)(0, (int)m_segObj[i].classIdx);
				stAIBLOBClassInfo blobInfo;

				blobInfo.class_id = nClassIndex;
				blobInfo.area = m_segObj[i].area;
				blobInfo.center_x = m_segObj[i].xCenter;
				blobInfo.center_y = m_segObj[i].yCenter;

				blobInfo.rtRect.top = m_segObj[i].yCenter - m_segObj[i].height / 2;
				blobInfo.rtRect.left = m_segObj[i].xCenter - m_segObj[i].width / 2;
				blobInfo.rtRect.bottom = m_segObj[i].yCenter + m_segObj[i].height / 2;
				blobInfo.rtRect.right = m_segObj[i].xCenter + m_segObj[i].width / 2;

				blobInfo.segmentscore[0] = (m_segObj[i].scores[0] / dMaxRange) * 100;
				blobInfo.segmentscore[1] = (m_segObj[i].scores[1] / dMaxRange) * 100;
				blobInfo.segmentscore[2] = (m_segObj[i].scores[2] / dMaxRange) * 100;

				if (m_segObj[i].nContours != 0)
				{
					blobInfo.contour_size = m_segObj[i].nPointsOnContour[0];
					blobInfo.contour_points.reserve(m_segObj[i].nPointsOnContour[0]);
					int pointsIdx = 0;
					for (size_t i_p = 0; i_p < m_segObj[i].nPointsOnContour[0]; i_p++)
					{
						CPoint ptContour(m_segObj[i].pointsOnContour[pointsIdx],
							m_segObj[i].pointsOnContour[pointsIdx + 1]);
						blobInfo.contour_points.push_back(ptContour);
						pointsIdx += 2;
					}
				}

				vtBlobOut.push_back(blobInfo);
			}
		}

		m_segObj.clear();
		std::vector<SR_SegmentedObject>().swap(m_segObj);
	}

	m_nSegModelState = NO_ERROR_OUT;
	return NO_ERROR_OUT;

#endif

#ifdef __SAIGE2_USE
	// TODO: Saige v2 inference
#endif

	return -1;
}

CString CSaigeSeg::GetErrorInfo(int nErrorType)
{
	switch (nErrorType)
	{
	case NO_ERROR_OUT:					return _T("SUCCESS");
	case NO_LICENCE:					return _T("NO_LICENCE: Feature not found");
	case NO_LICENSE_DONGLE:				return _T("NO_LICENSE_DONGLE: USB dongle not found");
	case USE_NOT_ALLOCATED_HANDLER:		return _T("Handler not allocated");
	case OUT_OF_GPU_MEMORY:				return _T("Out of GPU memory");
	case INAPPROPRIATE_GPU_NUMBER:		return _T("Inappropriate GPU number");
	case FILE_NOT_FOUND:				return _T("Model file not found");
	case EMPTY_IMAGE:					return _T("Input image is empty");
	case NOT_LOAD_NETWORK_IN_GPU:		return _T("Network not loaded in GPU");
	case FORWARD_ALGO_TIMEOUT:			return _T("Inspection timeout");
	case LICENSE_EXPIRED:				return _T("License expired");
	case LOW_VERSION_FILE:				return _T("Model file version is low");
	case AIMODEL_MISS_MATCH_ERRROR:		return _T("Class count mismatch");
	case AIMODEL_FILE_NAME_ERROR:		return _T("Model file name is empty");
	default:							return _T("UNKNOWN_ERROR");
	}
}
