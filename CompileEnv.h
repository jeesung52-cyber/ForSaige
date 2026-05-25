
#pragma once

#define MAX_LEN_CELL_ID		30
#define MAX_LEN_MODEL_NAME	30

#define _SAIGE1

#ifdef _SAIGE1
	#define __SAIGE1_USE
#endif

#ifdef __SAIGE2
#define __SAIGE2_USE
#endif
