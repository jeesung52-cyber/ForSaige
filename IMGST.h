#pragma once


#ifndef __IMAGE_ST__
#define __IMAGE_ST__

#define		MAX_WIDTH	20000
#define		MAX_HEIGHT	MAX_WIDTH

#include "Draw_Def.h"


enum class COLOR_TYPE
{
	MONO = 1,
	RGBCOLOR = 3,
	MAX_COLOR_TYPE
};

enum class VISION_RESULT
{	
	NONE = 0,
	CAPTURE,
	GOOD,
	BAD,
	MAX
};

// 깊이 표현용 enum (원하는 이름/값으로 바꿔도 됨)
enum IMG_DEPTH
{
    enIMG_DEPTH_8U = 0,   // 8bit unsigned
    enIMG_DEPTH_16S = 1,   // 16bit short
    enIMG_DEPTH_32F = 2    // 32bit float
};

typedef struct IMGSTRUCT
{
	INT				camidx;
	INT				nShot;
	INT				nCycle;
	IMGROI			roi;

	INT				Flipped;
	BOOL			bFilter;		// brightness correction

	INT				procTime;
	INT				BufferCount;

	COLOR_TYPE		type;
	INT				cx;
	INT				cy;
    INT				cc;             // channel count
	INT				depth;          // ★ 추가: IMG_DEPTH (0:8U, 1:16S, 2:32F)

	SYSTEMTIME		time;
	VISION_RESULT	InspResult;
	BOOL			bCompressed;
	INT				nCompressedSize;

	LPBYTE			pbuf;

    // ----- 헬퍼: 픽셀 1채널당 바이트 수 -----
    int BytesPerSample() const
    {
        switch (depth)
        {
        case enIMG_DEPTH_16S: return 2;
        case enIMG_DEPTH_32F: return 4;
        case enIMG_DEPTH_8U:
        default:
            return 1;
        }
    }

    // ----- 생성자 -----
	IMGSTRUCT::IMGSTRUCT()
	{
		pbuf = NULL;
		procTime = -1;
		nShot = -1;
		nCycle = -1;
		camidx = -1;
		BufferCount = -1;

        cx = cy = 0;
		type = COLOR_TYPE::MONO;
		cc = (int)COLOR_TYPE::MONO;
        depth = enIMG_DEPTH_8U;            // ★ 기본은 8bit

		time = { 0 };
		InspResult = VISION_RESULT::NONE;
		bCompressed = FALSE;
		nCompressedSize = 0;
		Flipped = 0;
		bFilter = FALSE;
		roi.u1 = roi.u2 = roi.v1 = roi.v2 = 0;
	}

    // ----- 소멸자 -----
	IMGSTRUCT::~IMGSTRUCT()
	{
		Clear();
	}

    // ----- 원본 전체 복사 (메타+데이터) -----
	void copy(IMGSTRUCT* psource)
	{
        if (!psource) return;
		if (pbuf == psource->pbuf)		return;

        if (psource->cx > MAX_WIDTH)  return;
        if (psource->cy > MAX_HEIGHT) return;

        // 사이즈/채널/깊이 다르면 Clear 해서 재할당 준비
		if (cx != psource->cx ||
            cy != psource->cy ||
            cc != psource->cc ||
            depth != psource->depth)
		{
			Clear();
		}

		camidx = psource->camidx;
		nShot = psource->nShot;
		nCycle = psource->nCycle;
		procTime = psource->procTime;
		BufferCount = psource->BufferCount;
		type = psource->type;
		cx = psource->cx;
		cy = psource->cy;
		cc = psource->cc;
        depth = psource->depth;      // ★ 깊이 복사
		time = psource->time;
		InspResult = psource->InspResult;
		bCompressed = psource->bCompressed;
		nCompressedSize = psource->nCompressedSize;
		Flipped = psource->Flipped;
		bFilter = psource->bFilter;
		roi = psource->roi;

        if (!bCompressed)
		{
            const int   bytesPerSample = psource->BytesPerSample();
            const size_t dwsize = (size_t)cx * cy * cc * bytesPerSample;

            if (dwsize == 0) return;

			if (pbuf == NULL)
				pbuf = new BYTE[dwsize];

			CopyMemory(pbuf, psource->pbuf, dwsize);
		}
		else
		{
            if (nCompressedSize <= 0) return;

			if (pbuf == NULL)
				pbuf = new BYTE[nCompressedSize];

			CopyMemory(pbuf, psource->pbuf, nCompressedSize);
		}
	}

    // ----- 데이터만 복사 (메타는 최소한만) -----
	void copyData(IMGSTRUCT* psource)
	{
        if (!psource) return;
		if (pbuf == psource->pbuf)		return;

        if (psource->cx > MAX_WIDTH)  return;
        if (psource->cy > MAX_HEIGHT) return;

        // 사이즈/채널/깊이 다르면 Clear 해서 재할당 준비
		if (cx != psource->cx ||
			cy != psource->cy ||
            cc != psource->cc ||
            depth != psource->depth)
		{
			Clear();
		}

		cx = psource->cx;
		cy = psource->cy;
		cc = psource->cc;
        depth = psource->depth;

		bCompressed = psource->bCompressed;
		nCompressedSize = psource->nCompressedSize;

        if (!bCompressed)
		{
            const int   bytesPerSample = BytesPerSample();
            const size_t sz = (size_t)cx * cy * cc * bytesPerSample;

            if (sz == 0) return;

			if (pbuf == NULL)
                pbuf = new BYTE[sz];

            CopyMemory(pbuf, psource->pbuf, sz);
		}
		else
		{
            if (nCompressedSize <= 0) return;

			if (pbuf == NULL)
				pbuf = new BYTE[nCompressedSize];

			CopyMemory(pbuf, psource->pbuf, nCompressedSize);
		}
	}

    // ----- 현재 pbuf의 byte size -----
	INT	GetpbufSize()
	{
        if (!bCompressed)
            return cx * cy * cc * BytesPerSample();
        else
            return nCompressedSize;
	}

    // ----- 유효성 체크 -----
	BOOL valid()
	{
		if (cx <= 0 || cy <= 0 || cc <= 0)		return FALSE;
		if (pbuf == NULL)			return FALSE;
		return TRUE;
	}

    // ----- 버퍼 할당 -----
    // 기존 호출 : Alloc(w, h, camid) / Alloc(w, h, camid, Color) 그대로 사용 가능
    int Alloc(INT width, INT height, INT camid,
        INT Color = (int)COLOR_TYPE::MONO,
        INT depthType = enIMG_DEPTH_8U)   // ★ 깊이 기본 8bit
	{
		if (width == 0 || height == 0 || Color == 0) return  0;

        // 기존과 다르게: 사이즈/채널/깊이 중 하나라도 다르면 Clear
        if ((cx != width || cy != height || cc != Color || depth != depthType) &&
            (cx != 0 || cy != 0 || cc != 0))
		{
			Clear();
		}

		cx = width;
		cy = height;
		cc = Color;
        depth = depthType;

        const int   bytesPerSample = BytesPerSample();
        const size_t bufSize = (size_t)cx * cy * cc * bytesPerSample;

        if (bufSize == 0) return 0;

        if (pbuf == NULL)
        {
            pbuf = new BYTE[bufSize];
        }

        ZeroMemory(pbuf, bufSize);

		camidx = camid;
		nShot = -1;
		BufferCount = 0;

		return 1;
	}

    // ----- 모든 정보/버퍼 초기화 + 메모리 해제 -----
	void Clear()
	{
		if (pbuf != NULL)
        {
            delete[] pbuf;
            pbuf = NULL;
        }

		procTime = -1;
		nShot = -1;
		nCycle = -1;
		camidx = -1;
		BufferCount = -1;
		cx = cy = 0;
		type = COLOR_TYPE::MONO;
		cc = (int)COLOR_TYPE::MONO;
        depth = enIMG_DEPTH_8U;      // ★ 리셋

		time = { 0 };
		InspResult = VISION_RESULT::NONE;
		bCompressed = FALSE;
		nCompressedSize = 0;
		Flipped = 0;
		bFilter = FALSE;
		roi.u1 = roi.u2 = roi.v1 = roi.v2 = 0;
	}

    // ----- 데이터만 0으로 초기화 (버퍼는 유지) -----
	void DataClear()
	{
		if (pbuf == NULL)		return;
        if (cx <= 0 || cy <= 0 || cc <= 0) return;

        const int   bytesPerSample = BytesPerSample();
        const size_t sz = (size_t)cx * cy * cc * bytesPerSample;

        if (sz == 0) return;

        ZeroMemory(pbuf, sz);

		procTime = -1;
		nShot = -1;
		nCycle = 0;
		camidx = -1;
		BufferCount = 0;
		nCompressedSize = 0;
		time = { 0 };
		InspResult = VISION_RESULT::NONE;
	}

} IMGST, * LPIMGST;


#pragma pack(push, 1)
struct PGMIMG
{
private:
	LPBYTE	pdata;

public:
	PGMIMG()
	{
		pdata = NULL;
	}
	~PGMIMG()
	{
		free();
	}
	void	free()
	{
		if (pdata != NULL)
			delete[] pdata, pdata = NULL;
	}
	void	alloc(INT cx, INT cy)
	{
		if (cx <= 0 || cy <= 0)		return;

		if (pdata != NULL)
			delete[] pdata, pdata = NULL;
		INT		totalsize = cx * cy;
		totalsize += sizeof(INT);
		totalsize += sizeof(INT);
		totalsize += sizeof(INT);

		pdata = new BYTE[totalsize];
		ZeroMemory(pdata, totalsize);

		INT* pintd = (INT*)pdata;
		pintd[0] = totalsize;
		pintd[1] = cx;
		pintd[2] = cy;
	}
	INT		Totalsize()
	{
		if (pdata == NULL)		return 0;
		INT* pintd = (INT*)pdata;
		return pintd[0];
	}
	INT		getcx()
	{
		if (pdata == NULL)		return 0;
		INT* pintd = (INT*)pdata;
		return pintd[1];
	}
	INT		getcy()
	{
		if (pdata == NULL)		return 0;
		INT* pintd = (INT*)pdata;
		return pintd[2];
	}
	INT		imgsize()
	{
		return getcx() * getcy();
	}

	LPBYTE	getptr()
	{
		if (pdata == NULL)		return NULL;
		return pdata;
	}
	void setptr(LPBYTE p)
	{
		//free();
		this->pdata = p;
	}
	LPBYTE	getimgptr()
	{
		if (pdata == NULL)		return NULL;
		return &pdata[12];
	}
	BOOL	valid()
	{
		BOOL	b = TRUE;		
		if (pdata == NULL)	b = FALSE;

		INT* pintd = (INT*)pdata;
		if (pintd[0] <= 0)	b = FALSE;
		if (pintd[1] <= 0)	b = FALSE;
		if (pintd[2] <= 0)	b = FALSE;
		
		return b;
	}
};
#pragma pack(pop)

#endif
