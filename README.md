# SysResourceMonitor
简单的系统资源显示工具

支持浮窗 和 任务栏标尺 

自用

*************************************************************************************

2019.5.20

磁盘IO

2019.5.10

已添加IntelCPU、Nvidia GPU、网速信息显示

*************************************************************************************

可自行拓展

win10 效果

![win10 效果](https://github.com/visualsmile/SysResourceMonitor/blob/master/VSSysResourceMonitor/win10.png)

win7 效果（win7默认任务栏透明会导致颜色显示不准，截图中将任务栏透明背景设置为接近不透明；目前没有找到处理方案）

![win7 效果（win7默认任务栏透明会导致颜色显示不准，截图中将任务栏透明背景设置为接近不透明；目前没有找到处理方案）](https://github.com/visualsmile/SysResourceMonitor/blob/master/VSSysResourceMonitor/win7.png)

浮窗 效果

![浮窗 效果](https://github.com/visualsmile/SysResourceMonitor/blob/master/VSSysResourceMonitor/浮窗.png)

自定义颜色设置 窗口

![自定义颜色设置 窗口](https://github.com/visualsmile/SysResourceMonitor/blob/master/VSSysResourceMonitor/自定义颜色设置.png)



#三个目录（VS2019）：

VSDeskBand                      独立的DeskBand实现，提供DeskBandOperation，需要优先编译

VSFloatingWidget                浮窗实现，不用编译

VSSysResourceMonitor			主程序目录



#详细说明：

1. VSDeskBand

语言：	C++， VS2019

工程：	VSDeskBand是一个com组件，编译后生成VSDeskBand.dll

		VSDeskBand通过 共享内存 与主体程序进行通讯，共享内存定义在 VSSharedMemStructDef
		
		VSDeskBandOperation 是一个静态库，编译后生成VSDeskBandOperation.lib
		
		
用法：	VSDeskBand.dll放在生成目录下，使用 VSDeskBandOperation.lib 来操作 VSDeskBand；

		主体从程序使用VSDeskBandOperation可以完成DeskBand的注册、卸载、显示、隐藏、更新等

2. VSFloatingWidget

语言：	C++   ，VS2019， Qt5.12

工程：	实现了常见的浮窗功能
		
用法：	直接加入项目继承即可

3. SysResourceMonitor C++， VS2019，Qt5.12

	3.1 SysResourceMonitorService
	
		工程：借助第三方的QTservice（https://github.com/qtproject/qt-solutions）；
		
		提供windows服务管理。主要是避免工具对管理员权限的依赖，将需要管理员权限的模块（目前有处理器温度读取）放到服务中。
		
		用法：提供相应的安装、卸载、启动、停止脚本
		
	3.2 SysResourceMonitor
		
		工程：	主程序
		
