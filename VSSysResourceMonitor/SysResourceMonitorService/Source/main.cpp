/****************************************************************************
-i 安装服务
-u 卸载服务
-e 作为独立exe执行
-s 启动服务
-t 停止服务
-p 暂停服务
-r 恢复暂停的服务
-c 将指定的命令码发送到服务
-v 显示版本和状态信息
****************************************************************************/

#include <QCoreApplication>
#include "SysResourceMonitorService.h"

int main(int argc, char **argv)
{
	SysResourceMonitorService service(argc, argv);
    return service.exec();
}
