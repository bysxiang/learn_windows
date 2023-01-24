#include <Windows.h>
#include "sysmets.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("SysMets3");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Program requires Windows NT!"),
			szAppName, MB_ICONERROR);
		return 0;
	}
	hwnd = CreateWindow(szAppName, TEXT("Get System Metrics No. 3"),
		WS_OVERLAPPEDWINDOW | WS_VSCROLL |
		WS_HSCROLL,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// ��̬�����ӱ����������ʼ��һ��
	static int cxChar, cxCaps, cyChar, cxClient, cyClient, iMaxWidth;
	HDC hdc;
	int i, x, y, iVertPos, iHorzPos, iPaintBeg, iPaintEnd;
	PAINTSTRUCT ps;
	// ��������Ϣ
	SCROLLINFO si;
	TCHAR szBuffer[10];
	TEXTMETRIC tm;

	switch (message)
	{
		case WM_CREATE:
			// ��¼�ַ�ƽ�����
			hdc = GetDC(hwnd);
			GetTextMetrics(hdc, &tm);
			cxChar = tm.tmAveCharWidth;
			//cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2; // ���������Ч�ڵ�ǰ��
			cxCaps = (tm.tmPitchAndFamily == TMPF_FIXED_PITCH ? 3 : 2) * cxChar / 2;
			cyChar = tm.tmHeight + tm.tmExternalLeading;
			ReleaseDC(hwnd, hdc);
			// ������3�еĿ��
			iMaxWidth = 40 * cxChar + 22 * cxCaps;
			
			return 0;
		case WM_SIZE:
			cxClient = LOWORD(lParam);
			cyClient = HIWORD(lParam);

			// ���ô�ֱ��������Χ��ҳ���С
			si.cbSize = sizeof(si);
			si.fMask = SIF_RANGE | SIF_PAGE;
			si.nMin = 0;
			si.nMax = NUMLINES - 1;
			si.nPage = cyClient / cyChar;
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

			// ����ˮƽ��������Χ��ҳ���С
			si.cbSize = sizeof(si);
			si.fMask = SIF_RANGE | SIF_PAGE;
			si.nMin = 0;
			si.nMax = 2 + iMaxWidth / cxChar;
			si.nPage = cxClient / cxChar;
			SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);

			return 0;
		case WM_VSCROLL:
			// ��ȡ���д�ֱ��������Ϣ
			si.cbSize = sizeof(si);
			si.fMask = SIF_ALL;
			GetScrollInfo(hwnd, SB_VERT, &si);
			// ����λ���Ա��Ժ���бȽ�
			iVertPos = si.nPos;
			switch (LOWORD(wParam))
			{
				case SB_TOP:
					si.nPos = si.nMin;
					break;
				case SB_BOTTOM:
					si.nPos = si.nMax;
					break;
				case SB_LINEUP:
					si.nPos -= 1;
					break;
				case SB_LINEDOWN:
					si.nPos += 1;
					break;
				case SB_PAGEUP:
					si.nPos -= si.nPage;
					break;
				case SB_PAGEDOWN:
					si.nPos += si.nPage;
					break;
				case SB_THUMBTRACK:
					si.nPos = si.nTrackPos;
					break;
				default:
					break;
			}
			// ����λ��Ȼ��������� ���� Windows �ĵ����������������õ�ֵ��ͬ��
			si.fMask = SIF_POS;
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
			GetScrollInfo(hwnd, SB_VERT, &si);
			// ���λ�øı䣬�������ڲ�������
			if (si.nPos != iVertPos)
			{
				ScrollWindow(hwnd, 0, cyChar * (iVertPos - si.nPos), NULL, NULL);
				UpdateWindow(hwnd);
			}
			return 0;
		case WM_PAINT:
			hdc = BeginPaint(hwnd, &ps);
			// ��ȡ��ֱ������λ��
			si.cbSize = sizeof(si);
			si.fMask = SIF_POS;
			GetScrollInfo(hwnd, SB_VERT, &si);
			iVertPos = si.nPos;
			// ��ȡˮƽ������λ��
			GetScrollInfo(hwnd, SB_HORZ, &si);
			iHorzPos = si.nPos;
			// �ҵ��ػ漫��
			iPaintBeg = max(0, iVertPos + ps.rcPaint.top / cyChar);
			iPaintEnd = min(NUMLINES - 1, iVertPos + ps.rcPaint.bottom / cyChar);
			for (i = iPaintBeg; i <= iPaintEnd; i++)
			{
				x = cxChar * (1 - iHorzPos);
				y = cyChar * (i - iVertPos);
				TextOut(hdc, x, y, sysmetrics[i].szLabel, lstrlen(sysmetrics[i].szLabel));
				TextOut(hdc, x + 22 * cxCaps, y, sysmetrics[i].szDesc, lstrlen(sysmetrics[i].szDesc));
				SetTextAlign(hdc, TA_RIGHT | TA_TOP);
				TextOut(hdc, x + 22 * cxCaps + 40 * cxChar, y, szBuffer, wsprintf(szBuffer, TEXT("%5d"),
					GetSystemMetrics(sysmetrics[i].Index)));
				SetTextAlign(hdc, TA_LEFT | TA_TOP);
			}
			EndPaint(hwnd, &ps);

			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);

			return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}