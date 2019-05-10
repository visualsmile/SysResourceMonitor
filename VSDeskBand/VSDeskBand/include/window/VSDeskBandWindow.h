#ifndef VSDeskBandWindow_H
#define VSDeskBandWindow_H

/************************************************************************/
/* DeskBand的窗口，会显示在任务栏中
  如果想显示自己的窗口样式，修改这个类即可
*/
/************************************************************************/
#include "window/IVSDeskBandWindowInf.h"
#include "Def/VSSharedMemStructDef.h"
#include <Uxtheme.h>
#include <string>
#include <list>

const int c_sMaxTextLength = 1024; // 窗口消息中宽字符 字节长度限制

class VSDeskBandWindow : public IVSDeskBandWindowInf
{
    friend LRESULT CALLBACK deskBandWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    VSDeskBandWindow();
    virtual ~VSDeskBandWindow();

public:
    virtual BOOL createNewDeskBandWindow(HWND hParent, HINSTANCE dllInstance);

    virtual BOOL showDeskBandWindow(BOOL bShow = TRUE);

    virtual BOOL closeDeskBandWindow();

    virtual HWND getDeskBandWindowHandle();

    virtual SIZE getDeskBandWindowMinSize(BOOL bNormalViewMode = TRUE);

    virtual void updateDeskBandWindow();

private:
    void setErrorText();
    void readSharedMemData();
    LRESULT OnPaint();
    int paintOneCol(/*HTHEME hTheme, */HDC hCacheDC, int nLeft, int nTop, int nCol);
    HFONT& borrowFontInfo(int nTextHeight);

private:
	int calcTextHeight();
	int calcColWidth(HDC hCacheDC, int nCol);

private:
    static const UINT m_nTimer = 100;		// 计时器编号，随便取了个100
    HWND m_hDeskBandWindow;
	HINSTANCE m_dllInstance;

private: // 共享内存相关
    VSSharedMemStruct m_oSharedMemStruct;	// 共享内存区数据
    HANDLE m_hFile;							// 共享区句柄
    HANDLE m_hReadMutexSemaphore;			// 全局互斥量
    void *m_pBuffer;						// 共享区指针
	std::list<std::pair<int, HFONT>> m_oFontList; // 缓存的字体指针
};

#endif //VSDeskBandWindow_H