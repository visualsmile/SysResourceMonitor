
#include <QIcon>
#include "UI/SRMHelpDialog.h"
#include <QVBoxLayout>
#include <QTextEdit>
#include <QString>


const QString c_sHelpText = QString::fromStdWString(L"\
关于管理员权限:\n\
  服务安装/任务栏标尺注册均需要管理员权限;\n\
  第一次运行时, 会自动安装任务栏标尺和后台服务;\n\
  托盘菜单中提供了任务栏标尺和后台服务管理,移除软件前可以先卸载相应组件;\n\n\
关于自定义颜色设置:\n\
  点击对应颜色块可以设置对应区间的颜色;\n\
  输入数值时,后面的数值必须大于前面的数值;");

SRMHelpDialog::SRMHelpDialog(QWidget* pParent /*=nullptr*/)
	: QDialog(pParent)
{
	initLayout();
	initProp();
}

SRMHelpDialog::~SRMHelpDialog()
{
}

void SRMHelpDialog::initLayout()
{
	QVBoxLayout* pMainLayout = new QVBoxLayout;
	QTextEdit* pTextEdit = new QTextEdit(this);
	pTextEdit->setReadOnly(true);
	pTextEdit->setText(c_sHelpText);

	pMainLayout->addWidget(pTextEdit);
	this->setLayout(pMainLayout);
}

void SRMHelpDialog::initProp()
{
	this->setFixedSize(550, 300);
	this->setWindowTitle(QString::fromStdWString(L"SRMResourceMonitor 帮助"));
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	this->setAttribute(Qt::WA_DeleteOnClose, false);
	this->setAttribute(Qt::WA_QuitOnClose, false);
	this->setWindowIcon(QIcon(":/Images/SysResourceMonitor.png"));
}

