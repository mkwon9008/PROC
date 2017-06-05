//#pragma once
#ifndef __CMONITOR_GRAPH_UNIT_H__
#define __CMONITOR_GRAPH_UNIT_H__

#include <Windows.h>
#include "Resource.h"
#include "CircularDoubleLinkedList.hpp"

// 최대 생성가능 윈도우
#define dfMAXCHILD		50

//타이틀바 최대 문자열 길이
#define MAX_LOADSTRING	100

//최대 리스트 사이즈
#define MAX_LIST_LEN	100

// 경고시 발생 윈도우 메시지
#define UM_ALERT					WM_USER + 1
#define dfALERT_CANCEL_TIMER_ID		200
#define dfALERT_CANCEL_TIME			500

// 폰트사이즈, 타이틀 / 그래프지표
#define dfFONT_TITLE_SIZE	20
#define dfFONT_POINT_SIZE	12

// 타이틀 표시영역 높이
#define dfTITLE_HEIGHT		30

// 세로축 지표 개수
#define dfGRID_VERT_NUM		4

class CMonitorGraphUnit
{
private:
	
	HWND _hWnd;
	HINSTANCE _hInst;

	//child의 memDC 생성.
	HDC _hdc, _backMemDC;
	HBITMAP _hOldBitMap;
	PAINTSTRUCT _ps;
	RECT _window_rt;
	HBITMAP _backBitMap;


	WCHAR _szTitle[MAX_LOADSTRING] = L"MonitorGraph";
	WCHAR _szWindowClass[MAX_LOADSTRING] = L"Child";
	CircularDoubleLinkedList<int> CList = CircularDoubleLinkedList<int>();
	HPEN _hPen;
	int rVal = 0;
	int _putVar = 0;
public:
	enum TYPE
	{
		BAR_SINGLE_VERT,	// 바 하나 세로
		BAR_SINGLE_HORZ,	// 바 하나 가로
		BAR_COLUMN_VERT,	// 바 여러개 세로
		BAR_COLUMN_HORZ,	// 바 여러개 가로
		LINE_SINGLE,		// 라인 그래프 하나
		LINE_MULTI,			// 라인 그래프 여러개
		PIE					// 원 파이 나누기
	};

	// static 맴버 함수의 프로시저에서 This 포인터를 찾기 위한
	// HWND + Class Ptr 의 테이블
	typedef struct ST_HWNDtoTHIS
	{
		HWND				hWnd[dfMAXCHILD] = { 0, };
		CMonitorGraphUnit* pThis[dfMAXCHILD] = { 0, };
	}stHWNDtoTHIS;

	static stHWNDtoTHIS st_HWND_to_THIS;
	

	CMonitorGraphUnit(HINSTANCE hInstance, HWND hWndParent, COLORREF BackColor, TYPE enType, int iPosX, int iPosY, int iWidth, int iHeight)
	{
		//윈도우 클래스 등록
		//MyRegisterClass(hInstance);
		WNDCLASSEXW wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHILDPROCGRAPH));
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CHILDPROCGRAPH);
		wcex.lpszClassName = _szWindowClass;
		wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

		RegisterClassExW(&wcex);

		//윈도우 창 생성
		if (!InitInstance(hInstance, SW_SHOW, hWndParent, iPosX, iPosY, iWidth, iHeight))
		{
			throw;
		}
		HDC _hdc = GetDC(_hWnd);		//_hWnd에서 _hdc로 DC를 받아옴.
		GetClientRect(_hWnd, &_window_rt);	//_hWnd의 RECT사이즈를 _window_rt에 Set.
		_backMemDC = CreateCompatibleDC(_hdc);	//pDC로 지정된 장치와 호환되는 메모리 디바이스 컨텍스트를 만듭니다.
		
		_backBitMap = CreateCompatibleBitmap(_hdc, _window_rt.right, _window_rt.bottom);
		_hOldBitMap = (HBITMAP)SelectObject(_backMemDC, _backBitMap);

		FillRect(_backMemDC, &_window_rt, (HBRUSH)GetStockObject(WHITE_BRUSH));
		ReleaseDC(_hWnd, _hdc);
	}

	~CMonitorGraphUnit()
	{

	}

	// 그래프 정보 입력, 윈도우 이름, 최대치, 경보수치 (0이면 없음)
	void SetInformation(WCHAR *szTitle, int iDataMax, int iDataAlert);

	// 그래프 라인 선 지정, 수시로 호출 가능,
	void SetLineColor(int iWeight, COLORREF BackColor);

	
	// 윈도우 프로시저
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_COMMAND:
			{
				int wmId = LOWORD(wParam);
				switch (wmId)
				{
				case IDM_ABOUT:
					//DialogBoxW(_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
					break;
				case IDM_EXIT:
					DestroyWindow(hWnd);
					break;
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
				}
			}
		break;
		case WM_PAINT:
		{

			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			CMonitorGraphUnit* pThis = GetThis(hWnd);
			PatBlt(pThis->_backMemDC, 0, 0, pThis->_window_rt.right, pThis->_window_rt.bottom, WHITENESS);

			//MoveToEx(pThis->_backMemDC, 0, 0, NULL);
			//LineTo(pThis->_backMemDC, 100, 100);
			
			int iSome = pThis->CList.peek(0);
			MoveToEx(pThis->_backMemDC, 0, iSome, NULL);

			int iCnt = 0;
			while (iCnt < pThis->CList.getSize())
			{
				int popedVar = pThis->CList.peek(iCnt);		//리스트에서 iCnt번째 있는 요소의 data를 popedVar에 넣음.
			
				pThis->_hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));	//펜 셋팅.
				SelectObject(pThis->_backMemDC, pThis->_hPen);			//펜과 DC를 묶음.
				
				LineTo(pThis->_backMemDC, iCnt * 3, popedVar);			//_backMemDC에 iCnt*3(x축), popedVar(Y축)까지 선긋기.
				//InvalidateRect(hWnd, NULL, true);				//WM_PAINT호출. 모두지우기(false).
				DeleteObject(pThis->_hPen);							//다썼으니 hPen반납.
				iCnt++;
			}
			
			BitBlt(hdc, 0, 0, pThis->_window_rt.right, pThis->_window_rt.right, pThis->_backMemDC, 0, 0, SRCCOPY);

			EndPaint(hWnd, &ps);
		}

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);

		}
		return 0;
	}

	
	
	// 데이터 넣기.
	BOOL InsertData(int iData)
	{
		if (CList.getSize() > MAX_LIST_LEN)
		{
			CList.popHead();
		}

		rVal = iData; //랜덤값 산출, 350그래프 보정.
		CList.InsertTail(rVal); //뒤에 랜덤값 푸쉬.

		InvalidateRect(_hWnd, NULL, true);
		return true;
	}

	BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, HWND hWndParent, int iPosX, int iPosY, int iWidth, int iHeight)
	{
		_hInst = hInstance;

		_hWnd = CreateWindowW(_szWindowClass, _szTitle, WS_CHILD | WS_VISIBLE | WS_CAPTION | WS_CLIPSIBLINGS,
			iPosX, iPosY, iWidth, iHeight, hWndParent, NULL, hInstance, NULL);

		DWORD Error = 1;
		if (!_hWnd)
		{
			return false;
		}

		ShowWindow(_hWnd, nCmdShow);
		UpdateWindow(_hWnd);
		PutThis();
		return true;
	}

protected:

	
	// 윈도우 핸들, this 포인터 매칭 테이블 관리.
	BOOL PutThis(void)
	{
		for (int iCount = 0; iCount < dfMAXCHILD; iCount++)
		{
			if (st_HWND_to_THIS.hWnd[iCount] == NULL)
			{
				st_HWND_to_THIS.hWnd[iCount] = _hWnd;
				st_HWND_to_THIS.pThis[iCount] = this;
				return true;
			}			
		}
		return false;
	}

	static CMonitorGraphUnit* GetThis(HWND hWnd)
	{
		for (int iCount = 0; iCount < dfMAXCHILD; iCount++)
		{
			if (st_HWND_to_THIS.hWnd[iCount] == hWnd)
			{
				return st_HWND_to_THIS.pThis[iCount];
			}
		}
		return NULL;
	}

	BOOL CreateGraphWindow(void);
	void DestroyGraphWindow(void);


	private:
		// 부모 윈도우 핸들, 내 윈도우 핸들, 인스턴스 핸들
		// 윈도우 위치,크기,색상, 그래프 타입 등.. 자료
		TYPE		_enGraphType;

};



#endif


