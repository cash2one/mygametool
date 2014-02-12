/********************************************************************************
** Form generated from reading UI file 'designerTs2780.ui'
**
** Created: Thu Dec 20 17:34:44 2012
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef DESIGNERTS2780_H
#define DESIGNERTS2780_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QTreeWidget>
#include <QtGui/QWidget>
#include <QtGui/QFileDialog>
#include <QtGui/QContextMenuEvent>


#include "precompile.h"
#include "PakFile.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
	QAction *actionOpen;
	QWidget *centralwidget;
	QTreeWidget *treeWidget;
	QMenuBar *menubar;
	QMenu *menuFile;
	QMenu *menuHelp;
	QStatusBar *statusbar;

	void setupUi(QMainWindow *MainWindow)
	{
		if (MainWindow->objectName().isEmpty())
			MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
		MainWindow->resize(500, 400);
		actionOpen = new QAction(MainWindow);
		actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
		centralwidget = new QWidget(MainWindow);
		centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
		treeWidget = new QTreeWidget(centralwidget);
		treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
		treeWidget->setEnabled(true);
		treeWidget->setGeometry(QRect(20, 30, 441, 301));
		treeWidget->setMouseTracking(false);
		treeWidget->setAutoFillBackground(false);
		treeWidget->setDragEnabled(false);
		treeWidget->setAutoExpandDelay(0);
		treeWidget->setWordWrap(false);
		treeWidget->setColumnCount(3);
		treeWidget->header()->setCascadingSectionResizes(true);
		treeWidget->header()->setDefaultSectionSize(150);
		treeWidget->header()->setHighlightSections(true);
		treeWidget->header()->setProperty("showSortIndicator", QVariant(true));
		MainWindow->setCentralWidget(centralwidget);
		menubar = new QMenuBar(MainWindow);
		menubar->setObjectName(QString::fromUtf8("menubar"));
		menubar->setGeometry(QRect(0, 0, 500, 19));
		menuFile = new QMenu(menubar);
		menuFile->setObjectName(QString::fromUtf8("menuFile"));
		menuHelp = new QMenu(menubar);
		menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
		MainWindow->setMenuBar(menubar);
		statusbar = new QStatusBar(MainWindow);
		statusbar->setObjectName(QString::fromUtf8("statusbar"));
		MainWindow->setStatusBar(statusbar);

		menubar->addAction(menuFile->menuAction());
		menubar->addAction(menuHelp->menuAction());
		menuFile->addAction(actionOpen);

		retranslateUi(MainWindow);
		QObject::connect(actionOpen, SIGNAL(activated()), MainWindow, SLOT(OnOpenFile()));

		QMetaObject::connectSlotsByName(MainWindow);
	} // setupUi

	void retranslateUi(QMainWindow *MainWindow)
	{
		MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
		actionOpen->setText(QApplication::translate("MainWindow", "Open", 0, QApplication::UnicodeUTF8));
		QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
		___qtreewidgetitem->setText(2, QApplication::translate("MainWindow", "CRC", 0, QApplication::UnicodeUTF8));
		___qtreewidgetitem->setText(1, QApplication::translate("MainWindow", "SIZE", 0, QApplication::UnicodeUTF8));
		___qtreewidgetitem->setText(0, QApplication::translate("MainWindow", "NAME", 0, QApplication::UnicodeUTF8));
		menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
		menuHelp->setTitle(QApplication::translate("MainWindow", "Help", 0, QApplication::UnicodeUTF8));
	} // retranslateUi

};

namespace Ui {
	class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE



class QtListDialog : public QMainWindow, Ui::MainWindow
{
	Q_OBJECT

public:

	QString	m_strFileName;

	QMenu*		m_pPopupMenu;
	QAction*	m_pSaveAs;

public:

	void OnSaveAs()
	{
		int iii;
	}

	QtListDialog()
		:m_pPopupMenu(NULL)
	{
		this->setupUi(this);

		m_pSaveAs = new QAction(tr("SaveAs"), this);
		m_pSaveAs->setCheckable(true);
		QObject::connect(m_pSaveAs, SIGNAL(triggered()), this, SLOT(OnSaveAs()));
	}

	protected slots:

	void OnOpenFile()
	{
		//QFileDialog::getOpenFileName(this, tr("Open PAK File"), "", tr("Pak Files (*.pak)"));
		QFileDialog filedialog(this);
		filedialog.setFileMode(QFileDialog::ExistingFile);
		filedialog.setViewMode(QFileDialog::List);
		filedialog.setNameFilter(tr("Pak File (*.pak)"));

		QStringList filenames;
		if(filedialog.exec())
		{
			filenames = filedialog.selectedFiles();
			m_strFileName = filenames[0];

			CPakFile pakFile;
			char filename[100] = {0};
			int size = m_strFileName.size();
			for( int i = 0; i < size; ++i)
			{
				filename[i] =  *(char*)&m_strFileName.at(i);
			}
			//memcpy(filename, (const char*)m_strFileName.data(), m_strFileName.size());
			filename[m_strFileName.size()] = '\0';
			pakFile.Open(filename);

			pakFile.CompressFile();

			CPakFile::iterator it = pakFile.Begin();
			for( ; it != pakFile.End(); ++it)
			{
				const CPakFile::stFileBaseInfo& info = *it;

				char buffer[100] = {0};
				QTreeWidgetItem* newItem = new QTreeWidgetItem(treeWidget);
				newItem->setText(0, tr(info.szName));
			
				_snprintf(buffer, 100, "%d", info.size);
				newItem->setText(1, tr(buffer));

				_snprintf(buffer, 100, "0x%08x", info.dwCRC);
				newItem->setText(2, tr(buffer));

				treeWidget->addTopLevelItem(newItem);
			}
		}
	}

	void contextMenuEvent(QContextMenuEvent *event)
	{
		if(m_pPopupMenu)
		{
			delete m_pPopupMenu;
			m_pPopupMenu = NULL;
		}

		m_pPopupMenu = new QMenu(this);
		//m_pPopupMenu->addMenu("SaveAs");
		m_pPopupMenu->addAction(m_pSaveAs);

		m_pPopupMenu->exec(event->globalPos());

	}


};

#endif // DESIGNERTS2780_H
