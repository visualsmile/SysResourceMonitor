#ifndef SRMHELPDIALOG_H_D47B6F0D_1A92_4E78_B453_26984AE227D8
#define SRMHELPDIALOG_H_D47B6F0D_1A92_4E78_B453_26984AE227D8

#include <QDialog>

class SRMHelpDialog : public QDialog
{
public:
	SRMHelpDialog(QWidget* pParent = nullptr);
	~SRMHelpDialog();

private:
	void initLayout();
	void initProp();
};

#endif //SRMHELPDIALOG_H_D47B6F0D_1A92_4E78_B453_26984AE227D8