#ifndef VSDeskBandGlobalDef_H
#define VSDeskBandGlobalDef_H
/************************************************************************/
/* com组件变量定义
   外面可能需要用到这些定义
*/
/************************************************************************/
#include "wtypes.h"

/************************************************************************/
/* COM 组件的定义，用来识别COM组件                                                            */
/************************************************************************/
// 1. 工具栏菜单显示名称（可以修改这个定义，来改变工具栏选项名称）
static LPCWSTR lsVSDeskBandMenuName = L"VSDeskBand";
// 2. com对象名称，可以在注册表里面用这个名称查到GUID/CLSID
static LPCWSTR lsVSDeskBandCOM_Name = L"VSDeskBand.VSDeskBandCOM";
// 3. CLSID 字符串, 注意要有大括号
static LPCWSTR lsCLSID_VSDeskBandCOM_GUID = L"{86BB96B4-BB0D-414D-BA76-CE8A06BA7CB6}";
// 4. CLSID GUID定义
static GUID CLSID_VSDeskBandCOM = { 0x86BB96B4, 0xBB0D, 0x414D,{ 0xBA, 0x76, 0xCE, 0x8A, 0x06, 0xBA, 0x7C, 0xB6 } };		


/************************************************************************/
/* Object对象定义，用来标识COM组件内部的接口对象
   通过这些GUID可以 QueryInterface到对应的接口
   目前这个GUID，是将 VSDeskBandObject 对象直接返回；
   如果需要添加其他对象或接口，可以自己定义GUID，然后在 VSDeskBandObject::QueryInterface中将其返回即可。
*/
/************************************************************************/
// 1. VSDeskBandObject对象
static LPCWSTR lsIVSDeskBandObject_GUID = L"{BD783BE6-299D-4D68-9DBA-654CD01B71E0}";
static GUID IID_VSDeskBandObject = { 0xBD783BE6, 0x299D, 0x4D68,{ 0x9D, 0xBA, 0x65, 0x4C, 0xD0, 0x1B, 0x71, 0xE0 } };

#endif //VSDeskBandGlobalDef_H
