#include "window/VSDeskBandWindow.h"

#define GWL_USERDATA (-21)  // 用户数据偏移量，这是一个系统中的定义，必须是-21
#define RECTWIDTH(x)   ((x).right - (x).left)
#define RECTHEIGHT(x)  ((x).bottom - (x).top)

// 窗口事件处理回调
LRESULT CALLBACK deskBandWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static VSDeskBandWindow  *pThis = nullptr;

    switch (uMsg)
    {
        case WM_TIMER:
        {
            pThis->readSharedMemData();
            RECT rc;
            GetClientRect(pThis->m_hDeskBandWindow, &rc);
            InvalidateRect(pThis->m_hDeskBandWindow, &rc, TRUE);
            pThis->updateDeskBandWindow();
        }
        break;
        case WM_NCCREATE:
        {
            LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
            pThis = (VSDeskBandWindow*)(lpcs->lpCreateParams);
            SetWindowLong(hwnd, GWL_USERDATA, GetWindowLong(hwnd, GWL_USERDATA));

            //set the window handle
            if (pThis)
                pThis->m_hDeskBandWindow = hwnd;
        }
        break;
        case WM_PAINT:
        {
            if (pThis)
                return pThis->OnPaint();
        }
        break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

VSDeskBandWindow::VSDeskBandWindow()
    : m_hDeskBandWindow(nullptr), m_dllInstance(nullptr), m_hFile(nullptr), m_hReadMutexSemaphore(nullptr), m_pBuffer(nullptr)
{
    setErrorText();
}

VSDeskBandWindow::~VSDeskBandWindow()
{
    UnmapViewOfFile(m_pBuffer);
    CloseHandle(m_hFile);              // 共享区句柄
    CloseHandle(m_hReadMutexSemaphore);    // 全局互斥量
    this->closeDeskBandWindow();
	for (auto it = m_oFontList.begin(); it != m_oFontList.end(); ++it)
	{
		HFONT hFont = (*it).second;
		DeleteObject(hFont);
	}
	m_oFontList.clear();
}

BOOL VSDeskBandWindow::createNewDeskBandWindow(HWND hParent, HINSTANCE dllInstance)
{
	m_dllInstance = dllInstance;
    //If the window doesn't exist yet, create it now.
    if (m_hDeskBandWindow == nullptr)
    {
        //Can't create a child window without a parent.
        if (hParent == nullptr)
        {
            return FALSE;
        }

        //If the window class has not been registered, then do so.
        WNDCLASS wc;

        if (!GetClassInfo(dllInstance, lsVSDeskBandWindowClassName, &wc))
        {
            ZeroMemory(&wc, sizeof(wc));
            wc.style = CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS;
            wc.lpfnWndProc = (WNDPROC)deskBandWindowProc;
            wc.cbClsExtra = 0;
            wc.cbWndExtra = 0;
            wc.hInstance = dllInstance;
            wc.hIcon = NULL;
            wc.hCursor = LoadCursor(NULL, IDC_ARROW);
            wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);// (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
            wc.lpszMenuName = NULL;
            wc.lpszClassName = lsVSDeskBandWindowClassName;

            if (!RegisterClass(&wc))
            {
                //If RegisterClass fails, CreateWindow below will fail.
            }
        }

        RECT  rc;

        GetClientRect(hParent, &rc);

        //Create the window. The WndProc will set m_hWnd.
        m_hDeskBandWindow = CreateWindowEx(0,
            lsVSDeskBandWindowClassName,
            lsVSDeskBandWindowName,
            WS_CHILD | WS_CLIPSIBLINGS,
            rc.left,
            rc.top,
            rc.right - rc.left,
            rc.bottom - rc.top,
            hParent,
            NULL,
            dllInstance,
            (LPVOID)this);
    }

    if (nullptr != m_hDeskBandWindow)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL VSDeskBandWindow::showDeskBandWindow(BOOL bShow)
{
    if (m_hDeskBandWindow == nullptr)
        return S_FALSE;

	if (bShow)
	{
		SetTimer(m_hDeskBandWindow, m_nTimer, 1000, NULL);
		return ShowWindow(m_hDeskBandWindow, SW_NORMAL);
	}
	else
	{
		KillTimer(m_hDeskBandWindow, m_nTimer);
		return ShowWindow(m_hDeskBandWindow, SW_HIDE);
	}
}

BOOL VSDeskBandWindow::closeDeskBandWindow()
{
    // 隐藏窗口
    ShowWindow(m_hDeskBandWindow, SW_HIDE);

    if (IsWindow(m_hDeskBandWindow))
    {
		KillTimer(m_hDeskBandWindow, m_nTimer);
        DestroyWindow(m_hDeskBandWindow);
        m_hDeskBandWindow = NULL;
		UnregisterClass(lsVSDeskBandWindowClassName, m_dllInstance);
    }

    
    return TRUE;
}

HWND VSDeskBandWindow::getDeskBandWindowHandle()
{
    return m_hDeskBandWindow;
}

// 目前只实现横向任务栏
SIZE VSDeskBandWindow::getDeskBandWindowMinSize(BOOL bNormalViewMode)
{
    int nWindowW = 100;
    readSharedMemData();

    // 读取任务栏高度 // 以前是根据任务栏高度来确定字体大小。现在改为根据屏幕分辨率确定字体大小，这个就没用了
    //RECT oSysTaskBarRect;
    //HWND hTaskBar = FindWindow(L"Shell_TrayWnd", L"");
    //UNREFERENCED_PARAMETER(hTaskBar);
    //GetWindowRect(hTaskBar, &oSysTaskBarRect);
    //int nTaskHeight = oSysTaskBarRect.bottom - oSysTaskBarRect.top;

    // 计算显示所有信息需要的窗口宽度
    if (m_oSharedMemStruct.getColCount() > 0)
    {
        HFONT& hFontTemp = borrowFontInfo(calcTextHeight(calcTaskBarHeight()));
        HDC hCacheDC = CreateCompatibleDC(NULL);  // 构造一个设备句柄，并设置字体,用来计算字体宽度
		HFONT hOldCacheFont = (HFONT)SelectObject(hCacheDC, hFontTemp);    	

		int nTotalWidth = 0;
		for (int nCol = 0; nCol < m_oSharedMemStruct.getColCount(); ++nCol)
		{
			nTotalWidth += calcColWidth(hCacheDC, nCol);
		}

		nWindowW = nTotalWidth 
			+ m_oSharedMemStruct.getLeftSpace() 
			+ m_oSharedMemStruct.getRightSpace() 
			+ m_oSharedMemStruct.getColCount() * (m_oSharedMemStruct.getColSpace() - 1);

		SelectObject(hCacheDC, hOldCacheFont);
        DeleteDC(hCacheDC);
    }

    // 目前只考虑了任务栏在下面的情况
    SIZE oSize;
    if (bNormalViewMode)
    {
        oSize.cx = nWindowW;
        oSize.cy = 30;
    }
    else
    {
        oSize.cx = 30;
        oSize.cy = nWindowW;
    }
    return oSize;
}

void VSDeskBandWindow::updateDeskBandWindow()
{  
    UpdateWindow(m_hDeskBandWindow);
}

void VSDeskBandWindow::setErrorText()
{
    m_oSharedMemStruct.reset();
    VSShareMemTextNode oTextNode(255, 0, 0, std::wstring(L"没有检测到输入"));
	m_oSharedMemStruct.setRowCount(1);
	m_oSharedMemStruct.setColCount(1);
	m_oSharedMemStruct.setTextNode(0, 0, oTextNode);
}

void VSDeskBandWindow::readSharedMemData()
{
    if (nullptr == m_hFile)
    {
        m_hFile = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, c_lsSRMShareMemoryName);
        if (nullptr == m_hFile)
        {
            setErrorText();
            return;
        }
        else
        {
            m_pBuffer = ::MapViewOfFile(m_hFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        }
    }

    if (nullptr == m_hReadMutexSemaphore)
    {
        m_hReadMutexSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, c_lsSRMMutexSemaphoreName);
        if (m_hReadMutexSemaphore == nullptr)
        {
            setErrorText();
            return;
        }
    }
    
    {
        m_oSharedMemStruct.reset();
        WaitForSingleObject(m_hReadMutexSemaphore, INFINITE); // （信号量句柄，等待时间）
        m_oSharedMemStruct.readFromStream(m_pBuffer);
        ReleaseSemaphore(m_hReadMutexSemaphore, 1, NULL); // （信号量句柄，释放的数量，out释放之前资源数量）

        if (m_oSharedMemStruct.getMessageID() == 0)
        {
            setErrorText();
        }      
    }
}


LRESULT VSDeskBandWindow::OnPaint()
{
	// begin
	PAINTSTRUCT oPaintStruct;
	BeginPaint(m_hDeskBandWindow , &oPaintStruct);

	// 1. 构造临时显示句柄（缓存） fixbug：采用拷贝
	/*HDC hCacheDC = ::CreateCompatibleDC(oPaintStruct.hdc);
	HBITMAP hCacheBitMap = ::CreateCompatibleBitmap(oPaintStruct.hdc, RECTWIDTH(oPaintStruct.rcPaint), RECTHEIGHT(oPaintStruct.rcPaint));
	HBITMAP hOldCacheBitMap = (HBITMAP)SelectObject(hCacheDC, hCacheBitMap);*/

	// 2. 设置字体
	int nTaskBarHeight = calcTaskBarHeight();
	int nTextHeight = calcTextHeight(nTaskBarHeight);
	/*HFONT hOldCacheFont = (HFONT)*/SelectObject(/*hCacheDC*/oPaintStruct.hdc, borrowFontInfo(nTextHeight));

	// 3. 绘制背景
	DrawThemeParentBackground(m_hDeskBandWindow, /*hCacheDC*/oPaintStruct.hdc, &oPaintStruct.rcPaint);

	// 4. 绘制文字(采用列对齐的方式)
	//HTHEME hTheme = OpenThemeData(NULL, L"TextStyle");
	int nLeft = m_oSharedMemStruct.getLeftSpace();
	int nTop = (nTaskBarHeight - nTextHeight * m_oSharedMemStruct.getRowCount()) / 2;
	if (nTop < 0) nTop = 0;
	for (int nCol = 0; nCol < m_oSharedMemStruct.getColCount(); ++nCol)
	{
		nLeft += paintOneCol(/*hTheme, *//*hCacheDC*/oPaintStruct.hdc, nLeft, nTop, nCol);
		nLeft += m_oSharedMemStruct.getColSpace();
	}
	//CloseThemeData(hTheme);

	// 5. 拷贝缓存图像到窗口上
	//BitBlt(oPaintStruct.hdc, 0, 0, RECTWIDTH(oPaintStruct.rcPaint), RECTHEIGHT(oPaintStruct.rcPaint), hCacheDC, 0, 0, SRCCOPY);

	// 6. 释放资源
	//SelectObject(hCacheDC, hOldCacheBitMap);
	//SelectObject(hCacheDC, hOldCacheFont);
	//DeleteObject(hCacheBitMap);
	//DeleteDC(hCacheDC);

	// end
	EndPaint(m_hDeskBandWindow, &oPaintStruct);
	return LRESULT(0);
}

// 绘制一列，并返回列宽
int VSDeskBandWindow::paintOneCol(/*HTHEME hTheme, */HDC hCacheDC, int nLeft, int nTop, int nCol)
{
	int nColWidth = calcColWidth(hCacheDC, nCol);

	RECT oCurCellRect;
	oCurCellRect.left = nLeft;
	oCurCellRect.right = nLeft + nColWidth;
	oCurCellRect.top = nTop;
	for (int nRow = 0; nRow < m_oSharedMemStruct.getRowCount(); ++nRow)
	{
		VSShareMemTextNode& oTextNode = m_oSharedMemStruct.getTextNode(nRow, nCol);

		SIZE oTextSize;
		GetTextExtentPoint32(hCacheDC, oTextNode.m_sInfo.c_str(), ((int)oTextNode.m_sInfo.length()), &oTextSize);
		oCurCellRect.bottom = oCurCellRect.top + oTextSize.cy;

		//DTTOPTS oDttOpts = { sizeof(oDttOpts) };
		//oDttOpts.dwFlags = DTT_COMPOSITED | DTT_TEXTCOLOR;
		//oDttOpts.crText = PALETTERGB(oTextNode.m_nRed, oTextNode.m_nGreen, oTextNode.m_nBlue);
		//DrawThemeTextEx(hTheme, hCacheDC, 0, 0, oTextNode.m_sInfo.c_str(), -1, DT_SINGLELINE | DT_CENTER | DT_VCENTER, &oCurCellRect, &oDttOpts);
		
		int nOldMode = SetBkMode(hCacheDC, TRANSPARENT);
		SetTextColor(hCacheDC, PALETTERGB(oTextNode.m_nRed, oTextNode.m_nGreen, oTextNode.m_nBlue));
		DrawText(hCacheDC, oTextNode.m_sInfo.c_str(), -1, &oCurCellRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		SetBkMode(hCacheDC, nOldMode);

		// 下一行的 top
		oCurCellRect.top += oTextSize.cy;
	}

	return nColWidth;
}

HFONT& VSDeskBandWindow::borrowFontInfo(int nTextHeight)
{
	for (auto it = m_oFontList.begin(); it != m_oFontList.end(); ++it)
	{
		if (it->first == nTextHeight)
			return it->second;
	}

	// 只缓存最近的3个
	if (m_oFontList.size() >= 3)
	{
		HFONT pTail = m_oFontList.back().second;
		DeleteObject(pTail);
		m_oFontList.pop_back();
	}

	HFONT pNewFont = CreateFont(nTextHeight,	// nHeight      高度
		0,                                      // nWidth       宽度，取默认
		0,                                      // nEscapement  倾斜
		0,                                      // nOrientation 倾斜
		FW_THIN,                                // nWeight       粗细
		false,                                  // bItalic      斜体
		false,                                  // bUnderline   下划线
		false,                                  // cStrikeOut   删除线
		ANSI_CHARSET,                           // nCharSet     字符集，取ANSI
		OUT_DEFAULT_PRECIS,                     // nOutPrecision输出精度
		CLIP_DEFAULT_PRECIS,                    // nClipPrecision 裁剪精度
		DEFAULT_QUALITY,                        // nQuality     质量
		DEFAULT_PITCH | FF_SWISS,               // nPitchAndFamily
		L"Microsoft YaHei UI"                   // lpszFacename 字体模板名称，取微软雅黑
	);
	m_oFontList.push_front(std::make_pair(nTextHeight, pNewFont));
	return m_oFontList.front().second;
}

int VSDeskBandWindow::calcTaskBarHeight()
{
	RECT oRect = { 0 };
	SystemParametersInfo(SPI_GETWORKAREA, 0, &oRect, 0);
	return GetSystemMetrics(SM_CYSCREEN) - oRect.bottom;	
}

//************************************
// Remark:    字体太小没啥意义，不考虑超过两行的情况
// FullName:  VSDeskBandWindow::calcTextHeight
// Returns:   int
// Parameter: int nTaskBarHeight
//************************************
int VSDeskBandWindow::calcTextHeight(int nTaskBarHeight)
{
	if (nTaskBarHeight <= 40)
		return nTaskBarHeight / 2;
	else
		return 20;
}

int VSDeskBandWindow::calcColWidth(HDC hCacheDC, int nCol)
{
	int nColMaxWidth = 0;
	for (int nRow = 0; nRow < m_oSharedMemStruct.getRowCount(); ++nRow)
	{
		VSShareMemTextNode& oTextNode = m_oSharedMemStruct.getTextNode(nRow, nCol);
		SIZE oTextSize;
		GetTextExtentPoint32(hCacheDC, oTextNode.m_sInfo.c_str(), ((int)oTextNode.m_sInfo.length()), &oTextSize);

		if (oTextSize.cx > nColMaxWidth)
			nColMaxWidth = oTextSize.cx;
	}

	return nColMaxWidth;
}
