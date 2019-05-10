#ifndef SRMSYSTEMTRAY_H_113AF347_51F7_47B6_9CC2_50E05DAB63DA
#define SRMSYSTEMTRAY_H_113AF347_51F7_47B6_9CC2_50E05DAB63DA
/************************************************************************/
/* »ŒŒÒ¿∏Õ–≈Ã                                                                     */
/************************************************************************/
#include <QObject>
class QMenu;
class QAction;
class SRMFloatingWidget;
class QSystemTrayIcon;

class SRMSystemTray : public QObject
{
	Q_OBJECT

public:
	SRMSystemTray(SRMFloatingWidget* pFloatingWidget);
	~SRMSystemTray();
	void show();
	void hide();

private:
	void createAutoRunMenu(QMenu * pTrayMenu);
	void createFloatingWidgetModelMenu(QMenu * pTrayMenu);
	void createFloatingWidgetPositionMenu(QMenu* pTrayMenu);
	void createDeskBandMenu(QMenu* pTrayMenu);
	void createOptionMenu(QMenu* pTrayMenu);
	void createServiceMenu(QMenu* pTrayMenu);
	void createCustomMenu(QMenu* pTrayMenu);
	void createHelpMenu(QMenu* pTrayMenu);
	void createQuitMenu(QMenu* pTrayMenu);

private:
	void autoRunChangedSlot();
	void floatingWidgetModeMenuSlot(QAction* pAction);
	void floatingWidgetPositionMenuSlot(QAction* pAction);
	void deskBandOperationChangedSlot(QAction* pAction);
	void serviceOperationChangedSlot(QAction* pAction);
	void customSettingSlot();
	void helpSlot();	
	void quitSlot();

private:
	bool showDeskBand();
	void hideDeskBand();

private:
	QMenu* m_pTrayMenu;
	QSystemTrayIcon* m_pSystemTray;

private:
	SRMFloatingWidget* m_pFloatingWidget;
};
#endif //SRMSYSTEMTRAY_H_113AF347_51F7_47B6_9CC2_50E05DAB63DA