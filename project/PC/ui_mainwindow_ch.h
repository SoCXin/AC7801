/********************************************************************************
** Form generated from reading UI file 'mainwindow_ch.ui'
**
** Created by: Qt User Interface Compiler version 5.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_CH_H
#define UI_MAINWINDOW_CH_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *exitAction;
    QAction *scanNodeAction;
    QAction *contactUsAction;
    QAction *aboutAction;
    QAction *openFirmwareFileAction;
    QAction *actionCAN_BaudRateCalculator;
    QWidget *centralWidget;
    QGridLayout *gridLayout_4;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_8;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QTableWidget *nodeListTableWidget;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QTableWidget *cmdListTableWidget;
    QFrame *frame;
    QGridLayout *gridLayout_3;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_6;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *deviceIndexComboBox;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QComboBox *channelIndexComboBox;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_4;
    QLineEdit *startAddrLineEdit;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_6;
    QComboBox *newBaudRateComboBox;
    QPushButton *setbaudRatePushButton;
    QHBoxLayout *horizontalLayout_7;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QComboBox *baudRateComboBox;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_5;
    QLineEdit *firmwareLineEdit;
    QCheckBox *allNodeCheckBox;
    QPushButton *openFirmwareFilePushButton;
    QPushButton *enterUpdatePushButton;
    QPushButton *updateFirmwarePushButton;
    QPushButton *executeFirmwarePushButton;
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *menu_2;
    QMenu *menu_3;
    QMenu *menu_4;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(726, 486);
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/easyicon_net.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        exitAction = new QAction(MainWindow);
        exitAction->setObjectName(QStringLiteral("exitAction"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/images/gif_48_026.gif"), QSize(), QIcon::Normal, QIcon::Off);
        exitAction->setIcon(icon1);
        scanNodeAction = new QAction(MainWindow);
        scanNodeAction->setObjectName(QStringLiteral("scanNodeAction"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/images/gif_48_086.gif"), QSize(), QIcon::Normal, QIcon::Off);
        scanNodeAction->setIcon(icon2);
        contactUsAction = new QAction(MainWindow);
        contactUsAction->setObjectName(QStringLiteral("contactUsAction"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/images/gif_48_079.gif"), QSize(), QIcon::Normal, QIcon::Off);
        contactUsAction->setIcon(icon3);
        contactUsAction->setVisible(false);
        aboutAction = new QAction(MainWindow);
        aboutAction->setObjectName(QStringLiteral("aboutAction"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/images/gif_48_052.gif"), QSize(), QIcon::Normal, QIcon::Off);
        aboutAction->setIcon(icon4);
        aboutAction->setVisible(false);
        openFirmwareFileAction = new QAction(MainWindow);
        openFirmwareFileAction->setObjectName(QStringLiteral("openFirmwareFileAction"));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/images/gif_48_045.gif"), QSize(), QIcon::Normal, QIcon::Off);
        openFirmwareFileAction->setIcon(icon5);
        actionCAN_BaudRateCalculator = new QAction(MainWindow);
        actionCAN_BaudRateCalculator->setObjectName(QStringLiteral("actionCAN_BaudRateCalculator"));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/images/calculator.ico"), QSize(), QIcon::Normal, QIcon::Off);
        actionCAN_BaudRateCalculator->setIcon(icon6);
        actionCAN_BaudRateCalculator->setVisible(false);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout_4 = new QGridLayout(centralWidget);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setMinimumSize(QSize(330, 0));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        nodeListTableWidget = new QTableWidget(groupBox);
        if (nodeListTableWidget->columnCount() < 3)
            nodeListTableWidget->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        nodeListTableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        nodeListTableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        nodeListTableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        nodeListTableWidget->setObjectName(QStringLiteral("nodeListTableWidget"));

        gridLayout->addWidget(nodeListTableWidget, 0, 0, 1, 1);


        horizontalLayout_8->addWidget(groupBox);

        groupBox_2 = new QGroupBox(centralWidget);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setMinimumSize(QSize(350, 0));
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        cmdListTableWidget = new QTableWidget(groupBox_2);
        if (cmdListTableWidget->columnCount() < 2)
            cmdListTableWidget->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        cmdListTableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        cmdListTableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem4);
        if (cmdListTableWidget->rowCount() < 11)
            cmdListTableWidget->setRowCount(11);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        cmdListTableWidget->setVerticalHeaderItem(0, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        cmdListTableWidget->setVerticalHeaderItem(1, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        cmdListTableWidget->setVerticalHeaderItem(2, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        cmdListTableWidget->setVerticalHeaderItem(3, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        cmdListTableWidget->setVerticalHeaderItem(4, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        cmdListTableWidget->setVerticalHeaderItem(5, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        cmdListTableWidget->setVerticalHeaderItem(6, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        cmdListTableWidget->setVerticalHeaderItem(7, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        cmdListTableWidget->setVerticalHeaderItem(8, __qtablewidgetitem13);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        cmdListTableWidget->setVerticalHeaderItem(9, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        cmdListTableWidget->setVerticalHeaderItem(10, __qtablewidgetitem15);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        __qtablewidgetitem16->setFlags(Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsEnabled);
        cmdListTableWidget->setItem(0, 0, __qtablewidgetitem16);
        QTableWidgetItem *__qtablewidgetitem17 = new QTableWidgetItem();
        cmdListTableWidget->setItem(0, 1, __qtablewidgetitem17);
        QTableWidgetItem *__qtablewidgetitem18 = new QTableWidgetItem();
        __qtablewidgetitem18->setFlags(Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsEnabled);
        cmdListTableWidget->setItem(1, 0, __qtablewidgetitem18);
        QTableWidgetItem *__qtablewidgetitem19 = new QTableWidgetItem();
        cmdListTableWidget->setItem(1, 1, __qtablewidgetitem19);
        QTableWidgetItem *__qtablewidgetitem20 = new QTableWidgetItem();
        __qtablewidgetitem20->setFlags(Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsEnabled);
        cmdListTableWidget->setItem(2, 0, __qtablewidgetitem20);
        QTableWidgetItem *__qtablewidgetitem21 = new QTableWidgetItem();
        cmdListTableWidget->setItem(2, 1, __qtablewidgetitem21);
        QTableWidgetItem *__qtablewidgetitem22 = new QTableWidgetItem();
        __qtablewidgetitem22->setFlags(Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsEnabled);
        cmdListTableWidget->setItem(3, 0, __qtablewidgetitem22);
        QTableWidgetItem *__qtablewidgetitem23 = new QTableWidgetItem();
        cmdListTableWidget->setItem(3, 1, __qtablewidgetitem23);
        QTableWidgetItem *__qtablewidgetitem24 = new QTableWidgetItem();
        __qtablewidgetitem24->setFlags(Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsEnabled);
        cmdListTableWidget->setItem(4, 0, __qtablewidgetitem24);
        QTableWidgetItem *__qtablewidgetitem25 = new QTableWidgetItem();
        cmdListTableWidget->setItem(4, 1, __qtablewidgetitem25);
        QTableWidgetItem *__qtablewidgetitem26 = new QTableWidgetItem();
        __qtablewidgetitem26->setFlags(Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsEnabled);
        cmdListTableWidget->setItem(5, 0, __qtablewidgetitem26);
        QTableWidgetItem *__qtablewidgetitem27 = new QTableWidgetItem();
        cmdListTableWidget->setItem(5, 1, __qtablewidgetitem27);
        QTableWidgetItem *__qtablewidgetitem28 = new QTableWidgetItem();
        __qtablewidgetitem28->setFlags(Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsEnabled);
        cmdListTableWidget->setItem(6, 0, __qtablewidgetitem28);
        QTableWidgetItem *__qtablewidgetitem29 = new QTableWidgetItem();
        cmdListTableWidget->setItem(6, 1, __qtablewidgetitem29);
        QTableWidgetItem *__qtablewidgetitem30 = new QTableWidgetItem();
        __qtablewidgetitem30->setFlags(Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsEnabled);
        cmdListTableWidget->setItem(7, 0, __qtablewidgetitem30);
        QTableWidgetItem *__qtablewidgetitem31 = new QTableWidgetItem();
        cmdListTableWidget->setItem(7, 1, __qtablewidgetitem31);
        QTableWidgetItem *__qtablewidgetitem32 = new QTableWidgetItem();
        __qtablewidgetitem32->setFlags(Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsEnabled);
        cmdListTableWidget->setItem(8, 0, __qtablewidgetitem32);
        QTableWidgetItem *__qtablewidgetitem33 = new QTableWidgetItem();
        cmdListTableWidget->setItem(8, 1, __qtablewidgetitem33);
        QTableWidgetItem *__qtablewidgetitem34 = new QTableWidgetItem();
        __qtablewidgetitem34->setFlags(Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsEnabled);
        cmdListTableWidget->setItem(9, 0, __qtablewidgetitem34);
        QTableWidgetItem *__qtablewidgetitem35 = new QTableWidgetItem();
        cmdListTableWidget->setItem(9, 1, __qtablewidgetitem35);
        QTableWidgetItem *__qtablewidgetitem36 = new QTableWidgetItem();
        __qtablewidgetitem36->setFlags(Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsEnabled);
        cmdListTableWidget->setItem(10, 0, __qtablewidgetitem36);
        QTableWidgetItem *__qtablewidgetitem37 = new QTableWidgetItem();
        cmdListTableWidget->setItem(10, 1, __qtablewidgetitem37);
        cmdListTableWidget->setObjectName(QStringLiteral("cmdListTableWidget"));

        gridLayout_2->addWidget(cmdListTableWidget, 0, 0, 1, 1);


        horizontalLayout_8->addWidget(groupBox_2);


        verticalLayout_2->addLayout(horizontalLayout_8);

        frame = new QFrame(centralWidget);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::Panel);
        frame->setFrameShadow(QFrame::Sunken);
        gridLayout_3 = new QGridLayout(frame);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        deviceIndexComboBox = new QComboBox(frame);
        deviceIndexComboBox->setObjectName(QStringLiteral("deviceIndexComboBox"));
        deviceIndexComboBox->setMinimumSize(QSize(80, 0));

        horizontalLayout->addWidget(deviceIndexComboBox);


        horizontalLayout_6->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_2 = new QLabel(frame);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setMinimumSize(QSize(48, 0));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(label_2);

        channelIndexComboBox = new QComboBox(frame);
        channelIndexComboBox->setObjectName(QStringLiteral("channelIndexComboBox"));
        channelIndexComboBox->setMinimumSize(QSize(80, 0));

        horizontalLayout_2->addWidget(channelIndexComboBox);


        horizontalLayout_6->addLayout(horizontalLayout_2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_4 = new QLabel(frame);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_4->addWidget(label_4);

        startAddrLineEdit = new QLineEdit(frame);
        startAddrLineEdit->setObjectName(QStringLiteral("startAddrLineEdit"));
        startAddrLineEdit->setMinimumSize(QSize(60, 0));

        horizontalLayout_4->addWidget(startAddrLineEdit);


        horizontalLayout_6->addLayout(horizontalLayout_4);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        label_6 = new QLabel(frame);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setEnabled(false);

        horizontalLayout_9->addWidget(label_6);

        newBaudRateComboBox = new QComboBox(frame);
        newBaudRateComboBox->setObjectName(QStringLiteral("newBaudRateComboBox"));
        newBaudRateComboBox->setEnabled(false);
        newBaudRateComboBox->setMinimumSize(QSize(80, 0));

        horizontalLayout_9->addWidget(newBaudRateComboBox);


        horizontalLayout_6->addLayout(horizontalLayout_9);

        setbaudRatePushButton = new QPushButton(frame);
        setbaudRatePushButton->setObjectName(QStringLiteral("setbaudRatePushButton"));
        setbaudRatePushButton->setEnabled(false);
        setbaudRatePushButton->setMinimumSize(QSize(80, 0));

        horizontalLayout_6->addWidget(setbaudRatePushButton);


        verticalLayout->addLayout(horizontalLayout_6);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_3 = new QLabel(frame);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_3->addWidget(label_3);

        baudRateComboBox = new QComboBox(frame);
        baudRateComboBox->setObjectName(QStringLiteral("baudRateComboBox"));
        baudRateComboBox->setMinimumSize(QSize(80, 0));

        horizontalLayout_3->addWidget(baudRateComboBox);


        horizontalLayout_7->addLayout(horizontalLayout_3);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label_5 = new QLabel(frame);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setMinimumSize(QSize(48, 0));
        label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_5->addWidget(label_5);

        firmwareLineEdit = new QLineEdit(frame);
        firmwareLineEdit->setObjectName(QStringLiteral("firmwareLineEdit"));

        horizontalLayout_5->addWidget(firmwareLineEdit);

        allNodeCheckBox = new QCheckBox(frame);
        allNodeCheckBox->setObjectName(QStringLiteral("allNodeCheckBox"));
        allNodeCheckBox->setMinimumSize(QSize(60, 0));

        horizontalLayout_5->addWidget(allNodeCheckBox);

        openFirmwareFilePushButton = new QPushButton(frame);
        openFirmwareFilePushButton->setObjectName(QStringLiteral("openFirmwareFilePushButton"));
        openFirmwareFilePushButton->setMinimumSize(QSize(60, 0));

        horizontalLayout_5->addWidget(openFirmwareFilePushButton);


        horizontalLayout_7->addLayout(horizontalLayout_5);

        enterUpdatePushButton = new QPushButton(frame);
        enterUpdatePushButton->setObjectName(QStringLiteral("enterUpdatePushButton"));
        enterUpdatePushButton->setMinimumSize(QSize(60, 0));
        enterUpdatePushButton->setMaximumSize(QSize(40, 16777215));

        horizontalLayout_7->addWidget(enterUpdatePushButton);

        updateFirmwarePushButton = new QPushButton(frame);
        updateFirmwarePushButton->setObjectName(QStringLiteral("updateFirmwarePushButton"));
        updateFirmwarePushButton->setMinimumSize(QSize(60, 0));
        updateFirmwarePushButton->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout_7->addWidget(updateFirmwarePushButton);

        executeFirmwarePushButton = new QPushButton(frame);
        executeFirmwarePushButton->setObjectName(QStringLiteral("executeFirmwarePushButton"));
        executeFirmwarePushButton->setMinimumSize(QSize(60, 0));

        horizontalLayout_7->addWidget(executeFirmwarePushButton);


        verticalLayout->addLayout(horizontalLayout_7);


        gridLayout_3->addLayout(verticalLayout, 0, 0, 1, 1);


        verticalLayout_2->addWidget(frame);


        gridLayout_4->addLayout(verticalLayout_2, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 726, 23));
        menu = new QMenu(menuBar);
        menu->setObjectName(QStringLiteral("menu"));
        menu_2 = new QMenu(menuBar);
        menu_2->setObjectName(QStringLiteral("menu_2"));
        menu_3 = new QMenu(menuBar);
        menu_3->setObjectName(QStringLiteral("menu_3"));
        menu_3->setEnabled(false);
        menu_4 = new QMenu(menuBar);
        menu_4->setObjectName(QStringLiteral("menu_4"));
        menu_4->setEnabled(false);
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menu->menuAction());
        menuBar->addAction(menu_2->menuAction());
        menuBar->addAction(menu_4->menuAction());
        menuBar->addAction(menu_3->menuAction());
        menu->addAction(openFirmwareFileAction);
        menu->addAction(exitAction);
        menu_2->addAction(scanNodeAction);
        menu_3->addAction(contactUsAction);
        menu_3->addAction(aboutAction);
        menu_4->addAction(actionCAN_BaudRateCalculator);
        mainToolBar->addAction(openFirmwareFileAction);
        mainToolBar->addAction(scanNodeAction);
        mainToolBar->addAction(contactUsAction);
        mainToolBar->addAction(aboutAction);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionCAN_BaudRateCalculator);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Ginkgo USB-CAN Bootloader V1.1", Q_NULLPTR));
        exitAction->setText(QApplication::translate("MainWindow", "\351\200\200\345\207\272", Q_NULLPTR));
        scanNodeAction->setText(QApplication::translate("MainWindow", "\346\211\253\346\217\217\350\212\202\347\202\271", Q_NULLPTR));
        contactUsAction->setText(QApplication::translate("MainWindow", "\350\201\224\347\263\273\346\210\221\344\273\254", Q_NULLPTR));
        aboutAction->setText(QApplication::translate("MainWindow", "\345\205\263\344\272\216\350\275\257\344\273\266", Q_NULLPTR));
        openFirmwareFileAction->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200\346\226\207\344\273\266", Q_NULLPTR));
        actionCAN_BaudRateCalculator->setText(QApplication::translate("MainWindow", "STM32 CAN\346\263\242\347\211\271\347\216\207\350\256\241\347\256\227\345\231\250", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("MainWindow", "\350\212\202\347\202\271\345\210\227\350\241\250", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem = nodeListTableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("MainWindow", "\350\212\202\347\202\271\345\234\260\345\235\200", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem1 = nodeListTableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("MainWindow", "\345\233\272\344\273\266\347\261\273\345\236\213", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem2 = nodeListTableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("MainWindow", "\345\233\272\344\273\266\347\211\210\346\234\254", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "\345\221\275\344\273\244\345\210\227\350\241\250", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem3 = cmdListTableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem3->setText(QApplication::translate("MainWindow", "\345\220\215\347\247\260", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem4 = cmdListTableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem4->setText(QApplication::translate("MainWindow", "\345\200\274\357\274\210Hex\357\274\211", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem5 = cmdListTableWidget->verticalHeaderItem(0);
        ___qtablewidgetitem5->setText(QApplication::translate("MainWindow", "1", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem6 = cmdListTableWidget->verticalHeaderItem(1);
        ___qtablewidgetitem6->setText(QApplication::translate("MainWindow", "2", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem7 = cmdListTableWidget->verticalHeaderItem(2);
        ___qtablewidgetitem7->setText(QApplication::translate("MainWindow", "3", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem8 = cmdListTableWidget->verticalHeaderItem(3);
        ___qtablewidgetitem8->setText(QApplication::translate("MainWindow", "4", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem9 = cmdListTableWidget->verticalHeaderItem(4);
        ___qtablewidgetitem9->setText(QApplication::translate("MainWindow", "5", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem10 = cmdListTableWidget->verticalHeaderItem(5);
        ___qtablewidgetitem10->setText(QApplication::translate("MainWindow", "6", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem11 = cmdListTableWidget->verticalHeaderItem(6);
        ___qtablewidgetitem11->setText(QApplication::translate("MainWindow", "7", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem12 = cmdListTableWidget->verticalHeaderItem(7);
        ___qtablewidgetitem12->setText(QApplication::translate("MainWindow", "8", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem13 = cmdListTableWidget->verticalHeaderItem(8);
        ___qtablewidgetitem13->setText(QApplication::translate("MainWindow", "9", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem14 = cmdListTableWidget->verticalHeaderItem(9);
        ___qtablewidgetitem14->setText(QApplication::translate("MainWindow", "10", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem15 = cmdListTableWidget->verticalHeaderItem(10);
        ___qtablewidgetitem15->setText(QApplication::translate("MainWindow", "11", Q_NULLPTR));

        const bool __sortingEnabled = cmdListTableWidget->isSortingEnabled();
        cmdListTableWidget->setSortingEnabled(false);
        QTableWidgetItem *___qtablewidgetitem16 = cmdListTableWidget->item(0, 0);
        ___qtablewidgetitem16->setText(QApplication::translate("MainWindow", "CmdFaild", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem17 = cmdListTableWidget->item(0, 1);
        ___qtablewidgetitem17->setText(QApplication::translate("MainWindow", "00", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem18 = cmdListTableWidget->item(1, 0);
        ___qtablewidgetitem18->setText(QApplication::translate("MainWindow", "OnlineCheck", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem19 = cmdListTableWidget->item(1, 1);
        ___qtablewidgetitem19->setText(QApplication::translate("MainWindow", "01", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem20 = cmdListTableWidget->item(2, 0);
        ___qtablewidgetitem20->setText(QApplication::translate("MainWindow", "EraseFlash", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem21 = cmdListTableWidget->item(2, 1);
        ___qtablewidgetitem21->setText(QApplication::translate("MainWindow", "03", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem22 = cmdListTableWidget->item(3, 0);
        ___qtablewidgetitem22->setText(QApplication::translate("MainWindow", "SetBaudRate", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem23 = cmdListTableWidget->item(3, 1);
        ___qtablewidgetitem23->setText(QApplication::translate("MainWindow", "04", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem24 = cmdListTableWidget->item(4, 0);
        ___qtablewidgetitem24->setText(QApplication::translate("MainWindow", "BlockWriteInfo", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem25 = cmdListTableWidget->item(4, 1);
        ___qtablewidgetitem25->setText(QApplication::translate("MainWindow", "05", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem26 = cmdListTableWidget->item(5, 0);
        ___qtablewidgetitem26->setText(QApplication::translate("MainWindow", "WriteBlockFlash", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem27 = cmdListTableWidget->item(5, 1);
        ___qtablewidgetitem27->setText(QApplication::translate("MainWindow", "06", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem28 = cmdListTableWidget->item(6, 0);
        ___qtablewidgetitem28->setText(QApplication::translate("MainWindow", "BlockReadInfo", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem29 = cmdListTableWidget->item(6, 1);
        ___qtablewidgetitem29->setText(QApplication::translate("MainWindow", "07", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem30 = cmdListTableWidget->item(7, 0);
        ___qtablewidgetitem30->setText(QApplication::translate("MainWindow", "ReadBlockFlash", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem31 = cmdListTableWidget->item(7, 1);
        ___qtablewidgetitem31->setText(QApplication::translate("MainWindow", "08", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem32 = cmdListTableWidget->item(8, 0);
        ___qtablewidgetitem32->setText(QApplication::translate("MainWindow", "ExcuteApp", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem33 = cmdListTableWidget->item(8, 1);
        ___qtablewidgetitem33->setText(QApplication::translate("MainWindow", "09", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem34 = cmdListTableWidget->item(9, 0);
        ___qtablewidgetitem34->setText(QApplication::translate("MainWindow", "EnterUpdateMode", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem35 = cmdListTableWidget->item(9, 1);
        ___qtablewidgetitem35->setText(QApplication::translate("MainWindow", "0A", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem36 = cmdListTableWidget->item(10, 0);
        ___qtablewidgetitem36->setText(QApplication::translate("MainWindow", "CmdSuccess", Q_NULLPTR));
        QTableWidgetItem *___qtablewidgetitem37 = cmdListTableWidget->item(10, 1);
        ___qtablewidgetitem37->setText(QApplication::translate("MainWindow", "0B", Q_NULLPTR));
        cmdListTableWidget->setSortingEnabled(__sortingEnabled);

        label->setText(QApplication::translate("MainWindow", "\350\256\276\345\244\207\345\217\267\357\274\232", Q_NULLPTR));
        deviceIndexComboBox->clear();
        deviceIndexComboBox->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "0", Q_NULLPTR)
         << QApplication::translate("MainWindow", "1", Q_NULLPTR)
         << QApplication::translate("MainWindow", "2", Q_NULLPTR)
         << QApplication::translate("MainWindow", "3", Q_NULLPTR)
         << QApplication::translate("MainWindow", "4", Q_NULLPTR)
         << QApplication::translate("MainWindow", "5", Q_NULLPTR)
         << QApplication::translate("MainWindow", "6", Q_NULLPTR)
         << QApplication::translate("MainWindow", "7", Q_NULLPTR)
         << QApplication::translate("MainWindow", "8", Q_NULLPTR)
         << QApplication::translate("MainWindow", "9", Q_NULLPTR)
        );
        label_2->setText(QApplication::translate("MainWindow", "\351\200\232\351\201\223\345\217\267\357\274\232", Q_NULLPTR));
        channelIndexComboBox->clear();
        channelIndexComboBox->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "0", Q_NULLPTR)
         << QApplication::translate("MainWindow", "1", Q_NULLPTR)
        );
        label_4->setText(QApplication::translate("MainWindow", "\350\265\267\345\247\213\345\234\260\345\235\200(Hex)\357\274\232", Q_NULLPTR));
        startAddrLineEdit->setText(QApplication::translate("MainWindow", "08006000", Q_NULLPTR));
        label_6->setText(QApplication::translate("MainWindow", "\346\226\260\346\263\242\347\211\271\347\216\207\357\274\232", Q_NULLPTR));
        newBaudRateComboBox->clear();
        newBaudRateComboBox->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "1000 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "900 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "800 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "666 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "600 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "500 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "400 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "300 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "250 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "225 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "200 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "160 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "150 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "144 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "125 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "120 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "100 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "90 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "80 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "75 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "60 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "50 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "40 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "30 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "20 Kbps", Q_NULLPTR)
        );
        setbaudRatePushButton->setText(QApplication::translate("MainWindow", "\350\256\276\347\275\256\346\263\242\347\211\271\347\216\207", Q_NULLPTR));
        label_3->setText(QApplication::translate("MainWindow", "\346\263\242\347\211\271\347\216\207\357\274\232", Q_NULLPTR));
        baudRateComboBox->clear();
        baudRateComboBox->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "1000 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "900 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "800 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "666 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "600 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "500 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "400 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "300 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "250 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "225 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "200 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "160 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "150 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "144 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "125 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "120 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "100 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "90 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "80 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "75 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "60 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "50 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "40 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "30 Kbps", Q_NULLPTR)
         << QApplication::translate("MainWindow", "20 Kbps", Q_NULLPTR)
        );
        baudRateComboBox->setCurrentText(QApplication::translate("MainWindow", "500 Kbps", Q_NULLPTR));
        label_5->setText(QApplication::translate("MainWindow", "\345\233\272\344\273\266\357\274\232", Q_NULLPTR));
        allNodeCheckBox->setText(QApplication::translate("MainWindow", "\346\211\200\346\234\211\350\212\202\347\202\271", Q_NULLPTR));
        openFirmwareFilePushButton->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200\346\226\207\344\273\266", Q_NULLPTR));
        enterUpdatePushButton->setText(QApplication::translate("MainWindow", "\350\277\233\345\205\245\345\215\207\347\272\247", Q_NULLPTR));
        updateFirmwarePushButton->setText(QApplication::translate("MainWindow", "\346\233\264\346\226\260\345\233\272\344\273\266", Q_NULLPTR));
        executeFirmwarePushButton->setText(QApplication::translate("MainWindow", "\346\211\247\350\241\214\345\233\272\344\273\266", Q_NULLPTR));
        menu->setTitle(QApplication::translate("MainWindow", "\346\226\207\344\273\266", Q_NULLPTR));
        menu_2->setTitle(QApplication::translate("MainWindow", "\346\223\215\344\275\234", Q_NULLPTR));
        menu_3->setTitle(QApplication::translate("MainWindow", "\345\270\256\345\212\251", Q_NULLPTR));
        menu_4->setTitle(QApplication::translate("MainWindow", "\345\267\245\345\205\267", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_CH_H
