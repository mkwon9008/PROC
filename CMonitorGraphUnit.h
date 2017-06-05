//#pragma once
#ifndef __CMONITOR_GRAPH_UNIT_H__
#define __CMONITOR_GRAPH_UNIT_H__

#include <Windows.h>
#include "Resource.h"
#include "CircularDoubleLinkedList.hpp"

// �ִ� �������� ������
#define dfMAXCHILD		50

//Ÿ��Ʋ�� �ִ� ���ڿ� ����
#define MAX_LOADSTRING	100

//�ִ� ����Ʈ ������
#define MAX_LIST_LEN	100

// ���� �߻� ������ �޽���
#define UM_ALERT					WM_USER + 1
#define dfALERT_CANCEL_TIMER_ID		200
#define dfALERT_CANCEL_TIME			500

// ��Ʈ������, Ÿ��Ʋ / �׷�����ǥ
#define dfFONT_TITLE_SIZE	20
#define dfFONT_POINT_SIZE	12

// Ÿ��Ʋ ǥ�ÿ��� ����
#define dfTITLE_HEIGHT		30

// ������ ��ǥ ����
#define dfGRID_VERT_NUM		4

class CMonitorGraphUnit
{
private:
	
	HWND _hWnd;
	HINSTANCE _hInst;

	//child�� memDC ����.
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
		BAR_SINGLE_VERT,	// �� �ϳ� ����
		BAR_SINGLE_HORZ,	// �� �ϳ� ����
		BAR_COLUMN_VERT,	// �� ������ ����
		BAR_COLUMN_HORZ,	// �� ������ ����
		LINE_SINGLE,		// ���� �׷��� �ϳ�
		LINE_MULTI,			// ���� �׷��� ������
		PIE					// �� ���� ������
	};

	// static �ɹ� �Լ��� ���ν������� This �����͸� ã�� ����
	// HWND + Class Ptr �� ���̺�
	typedef struct ST_HWNDtoTHIS
	{
		HWND				hWnd[dfMAXCHILD] = { 0, };
		CMonitorGraphUnit* pThis[dfMAXCHILD] = { 0, };
	}stHWNDtoTHIS;

	static stHWNDtoTHIS st_HWND_to_THIS;
	

	CMonitorGraphUnit(HINSTANCE hInstance, HWND hWndParent, COLORREF BackColor, TYPE enType, int iPosX, int iPosY, int iWidth, int iHeight)
	{
		//������ Ŭ���� ���
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

		//������ â ����
		if (!InitInstance(hInstance, SW_SHOW, hWndParent, iPosX, iPosY, iWidth, iHeight))
		{
			throw;
		}
		HDC _hdc = GetDC(_hWnd);		//_hWnd���� _hdc�� DC�� �޾ƿ�.
		GetClientRect(_hWnd, &_window_rt);	//_hWnd�� RECT����� _window_rt�� Set.
		_backMemDC = CreateCompatibleDC(_hdc);	//pDC�� ������ ��ġ�� ȣȯ�Ǵ� �޸� ����̽� ���ؽ�Ʈ�� ����ϴ�.
		
		_backBitMap = CreateCompatibleBitmap(_hdc, _window_rt.right, _window_rt.bottom);
		_hOldBitMap = (HBITMAP)SelectObject(_backMemDC, _backBitMap);

		FillRect(_backMemDC, &_window_rt, (HBRUSH)GetStockObject(WHITE_BRUSH));
		ReleaseDC(_hWnd, _hdc);
	}

	~CMonitorGraphUnit()
	{

	}

	// �׷��� ���� �Է�, ������ �̸�, �ִ�ġ, �溸��ġ (0�̸� ����)
	void SetInformation(WCHAR *szTitle, int iDataMax, int iDataAlert);

	// �׷��� ���� �� ����, ���÷� ȣ�� ����,
	void SetLineColor(int iWeight, COLORREF BackColor);

	
	// ������ ���ν���
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
				int popedVar = pThis->CList.peek(iCnt);		//����Ʈ���� iCnt��° �ִ� ����� data�� popedVar�� ����.
			
				pThis->_hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));	//�� ����.
				SelectObject(pThis->_backMemDC, pThis->_hPen);			//��� DC�� ����.
				
				LineTo(pThis->_backMemDC, iCnt * 3, popedVar);			//_backMemDC�� iCnt*3(x��), popedVar(Y��)���� ���߱�.
				//InvalidateRect(hWnd, NULL, true);				//WM_PAINTȣ��. ��������(false).
				DeleteObject(pThis->_hPen);							//�ٽ����� hPen�ݳ�.
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

	
	
	// ������ �ֱ�.
	BOOL InsertData(int iData)
	{
		if (CList.getSize() > MAX_LIST_LEN)
		{
			CList.popHead();
		}

		rVal = iData; //������ ����, 350�׷��� ����.
		CList.InsertTail(rVal); //�ڿ� ������ Ǫ��.

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

	
	// ������ �ڵ�, this ������ ��Ī ���̺� ����.
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
		// �θ� ������ �ڵ�, �� ������ �ڵ�, �ν��Ͻ� �ڵ�
		// ������ ��ġ,ũ��,����, �׷��� Ÿ�� ��.. �ڷ�
		TYPE		_enGraphType;

};



#endif


