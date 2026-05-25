#ifndef PCH_H
#define PCH_H

#include "framework.h"
#include "CompileEnv.h"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#ifdef __SAIGE1_USE
	#include "Saige/include_Ver10/SegmentationTestDLL.h"
	#pragma comment(lib, "Saige/lib_Ver10/Segmentation.lib")
#endif

#ifdef __SAIGE2_USE
	#include <limits.h>
	#include <stdlib.h>
	#include <string>
	#include <vector>
	#include "SaigeVisionAPI.h"
	#include "SaigeVisionUtil.h"
	#pragma comment(lib, "SaigeVision.lib")
#endif

#endif
