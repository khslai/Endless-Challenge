//=============================================================================
//
// デバッグ表示処理 [DebugProcess.cpp]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#define _CRT_SECURE_NO_WARNINGS
#include "main.h"
#include "DebugProcess.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
char		g_aStrDebug[1024] = { "\0" };	// デバッグ情報

//=============================================================================
// デバッグ表示処理の初期化
//=============================================================================
HRESULT InitDebugProcess(bool FirstInit)
{
	// 情報クリア
	memset(g_aStrDebug, 0, sizeof(g_aStrDebug));

	return S_OK;
}

//=============================================================================
// デバッグ表示処理の終了処理
//=============================================================================
void UninitDebugProcess(void)
{

	return;
}

//=============================================================================
// デバッグ表示処理の更新処理
//=============================================================================
void UpdateDebugProcess(void)
{

	return;
}

//=============================================================================
// デバッグ表示処理の描画処理
//=============================================================================
void DrawDebugProcess(void)
{
	RECT rect = { 0, 0, Screen_Width, Screen_Height };
	LPD3DXFONT Font = GetFont(FontSize_40);

	// 情報表示
	Font->DrawText(NULL, g_aStrDebug, -1, &rect, DT_LEFT, YELLOW(255));

	// 情報クリア
	memset(g_aStrDebug, 0, sizeof g_aStrDebug);
}

//=============================================================================
// デバッグ表示の登録
//=============================================================================
void PrintDebugMsg(const char *fmt, ...)
{
	va_list list;			// 可変引数を処理する為に使用する変数
	char *pCur;
	char aBuf[256] = { "\0" };
	char aWk[32];

	// 可変引数にアクセスする前の初期処理
	va_start(list, fmt);

	pCur = (char *)fmt;
	for (; *pCur; ++pCur)
	{
		if (*pCur != '%')
		{
			sprintf(aWk, "%c", *pCur);
		}
		else
		{
			pCur++;

			switch (*pCur)
			{
			case 'd':
				// 可変引数にアクセスしてその変数を取り出す処理
				sprintf(aWk, "%d", va_arg(list, int));
				break;

			case 'f':
				// 可変引数にアクセスしてその変数を取り出す処理
				sprintf(aWk, "%.2f", va_arg(list, double));		// double型で指定
				break;

			case 's':
				// 可変引数にアクセスしてその変数を取り出す処理
				sprintf(aWk, "%s", va_arg(list, char*));
				break;

			case 'c':
				// 可変引数にアクセスしてその変数を取り出す処理
				sprintf(aWk, "%c", va_arg(list, char));
				break;

			default:
				sprintf(aWk, "%c", *pCur);
				break;
			}
		}
		strcat(aBuf, aWk);
}

	// 可変引数にアクセスした後の終了処理
	va_end(list);

	// 連結
	if ((strlen(g_aStrDebug) + strlen(aBuf)) < ((sizeof g_aStrDebug) - 1))
	{
		strcat(g_aStrDebug, aBuf);
	}

	return;
}

