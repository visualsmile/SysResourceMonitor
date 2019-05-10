
#ifndef IVSDeskBandWindowInf_H
#define IVSDeskBandWindowInf_H

/************************************************************************/
/* DeskBand窗口接口
   DeskBand窗口需要以下接口
/************************************************************************/
#include <Unknwn.h>
#include <Strsafe.h>

// 窗口信息相关
static LPCTSTR lsVSDeskBandWindowClassName = L"VSDeskBandWindowClass";        // 窗口类名，win下每个窗口需要有自己的窗口类
static LPCTSTR lsVSDeskBandWindowName = L"VSDeskBandWindow";                  // 窗口名称，DeskBand窗口的名称                      
static const int c_nUpdateText = 1;      // 更新显示内容

class IVSDeskBandWindowInf
{
public:
    virtual ~IVSDeskBandWindowInf() {};

    virtual BOOL createNewDeskBandWindow(HWND hParent, HINSTANCE dllInstance) = 0;

    virtual BOOL showDeskBandWindow(BOOL bShow = TRUE) = 0;

    virtual BOOL closeDeskBandWindow() = 0;

    virtual HWND getDeskBandWindowHandle() = 0;

    virtual SIZE getDeskBandWindowMinSize(BOOL bNormalViewMode = TRUE) = 0;

    virtual void updateDeskBandWindow() = 0;
};

#endif //IVSDeskBandWindowInf_H