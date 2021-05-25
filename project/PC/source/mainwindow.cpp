#include "mainwindow.h"
#ifdef LANGUE_EN
#include "ui_mainwindow_en.h"
#else
#include "ui_mainwindow_ch.h"
#endif

typedef  struct {
  unsigned char   SJW;
  unsigned char   BS1;
  unsigned char   BS2;
  unsigned short  PreScale;
} tCAN_BaudRate;

tCAN_BaudRate  CAN_BaudRateInitTab[]= {      // CLK=36MHz
{1,2,1,9},       // 1M
{1,5,2,5},       // 900K
{1,10,4,3},       // 800K
{1,7,1,6},       // 666K
{1,3,1,12},       // 600K
{1,4,1,12},       // 500K
{1,7,1,10},       // 400K
{1,6,1,15},      // 300K
{1,6,1,18},      // 250K
{1,6,1,20},      // 225K
{1,15,2,10},      // 200K
{1,12,2,15},      // 160K
{1,6,1,30},      // 150K
{1,3,1,50},      // 144K
{1,6,1,36},     // 125K
{1,12,2,20},     // 120K
{1,6,1,45},      // 100K
{1,6,1,50},     // 90K
{1,4,1,75},     // 80K
{1,6,1,60},      // 75K
{1,6,1,75},      // 60K
{1,6,1,90},      // 50K
{1,7,1,100},      // 40K
{1,6,1,150},     // 30K
{1,6,1,225},     // 20K
};
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->cmdListTableWidget->setColumnWidth(0,180);
#ifdef LANGUE_EN
    ui->nodeListTableWidget->setColumnWidth(0,100);
    ui->nodeListTableWidget->setColumnWidth(1,100);
    ui->nodeListTableWidget->setColumnWidth(2,150);
#endif
    for(int i=0;i<ui->cmdListTableWidget->rowCount();i++){
        ui->cmdListTableWidget->setRowHeight(i,20);
    }
}

MainWindow::~MainWindow()
{
    CBL_CloseDevice(ui->deviceIndexComboBox->currentIndex());
    delete ui;
}

uint16_t MainWindow::crc16_ccitt(const uint8_t *buf, uint32_t len)
{
    static const uint16_t crc16tab[256]= {
        0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
        0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
        0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
        0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
        0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
        0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
        0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
        0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
        0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
        0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
        0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
        0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
        0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
        0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
        0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
        0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
        0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
        0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
        0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
        0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
        0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
        0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
        0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
        0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
        0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
        0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
        0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
        0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
        0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
        0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
        0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
        0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
    };
    register uint32_t counter;
    register uint16_t crc = 0;
    for( counter = 0; counter < len; counter++)
            crc = (crc<<8) ^ crc16tab[((crc>>8) ^ *(uint8_t *)buf++)&0x00FF];
    return crc;
}

void MainWindow::on_openFirmwareFilePushButton_clicked()
{
    QString fileName;
    fileName=QFileDialog::getOpenFileName(this,
                                          tr("Open files"),
                                          "",
                                          "Binary Files (*.bin);;Hex Files (*.hex);;All Files (*.*)");
    if(fileName.isNull()){
        return;
    }
    ui->firmwareLineEdit->setText(fileName);
}

int MainWindow::CAN_GetBaudRateNum(unsigned int BaudRate)
{
    switch(BaudRate){
        case 1000000 :return 0;
        case 900000 :return 1;
        case 800000 :return 2;
        case 666000 :return 3;
        case 600000 :return 4;
        case 500000 :return 5;
        case 400000 :return 6;
        case 300000 :return 7;
        case 250000 :return 8;
        case 225000:return 9;
        case 200000 :return 10;
        case 160000:return 11;
        case 150000 :return 12;
        case 144000:return 13;
        case 125000 :return 14;
        case 120000:return 15;
        case 100000 :return 16;
        case 90000 :return 17;
        case 80000 :return 18;
        case 75000:return 19;
        case 60000 :return 20;
        case 50000 :return 21;
        case 40000 :return 22;
        case 30000 :return 23;
        case 20000 :return 24;
        default:return 0;
    }
}
bool MainWindow::DeviceConfig(void)
{
    int ret;
    ret = CBL_ScanDevice(1);
    if(ret <= 0){
#ifdef LANGUE_EN
        QMessageBox::warning(this,"Warning","No device connected!");
#else
        QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("无设备连接！"));
#endif
        return false;
    }
    ret = CBL_OpenDevice(ui->deviceIndexComboBox->currentIndex());
    if(ret != CBL_ERR_SUCCESS){
#ifdef LANGUE_EN
        QMessageBox::warning(this,"Warning","Open device faild!");
#else
        QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("打开设备失败！"));
#endif
        return false;
    }
    CBL_CMD_LIST CMD_List;
    QString cmdStr[]={"CmdFaild","OnlineCheck","EraseFlash","SetBaudRate","BlockWriteInfo",
                      "WriteBlockFlash","BlockReadInfo","ReadBlockFlash","ExcuteApp","EnterUpdateMode","CmdSuccess"};
    uint8_t cmdData[16];
    for(int i=0;i<ui->cmdListTableWidget->rowCount();i++){
        if(ui->cmdListTableWidget->item(i,0)->text()==cmdStr[i]){
            cmdData[i] = ui->cmdListTableWidget->item(i,1)->text().toInt(NULL,16);
        }
    }
    CMD_List.CmdFaild = cmdData[0];
    CMD_List.OnlineCheck = cmdData[1];
    CMD_List.EraseFlash = cmdData[2];
    CMD_List.SetBaudRate = cmdData[3];
    CMD_List.BlockWriteInfo = cmdData[4];
    CMD_List.WriteBlockFlash = cmdData[5];
    CMD_List.BlockReadInfo = cmdData[6];
    CMD_List.ReadBlockFlash = cmdData[7];
    CMD_List.ExcuteApp = cmdData[8];
    CMD_List.EnterUpdateMode = cmdData[9];
    CMD_List.CmdSuccess = cmdData[10];
    VCI_INIT_CONFIG_EX CAN_InitConfig;
    QString str = ui->baudRateComboBox->currentText();
    str.resize(str.length()-4);
    int baud = str.toInt(NULL,10)*1000;
    CAN_InitConfig.CAN_BRP = CAN_BaudRateInitTab[CAN_GetBaudRateNum(baud)].PreScale;
    CAN_InitConfig.CAN_SJW = CAN_BaudRateInitTab[CAN_GetBaudRateNum(baud)].SJW;
    CAN_InitConfig.CAN_BS1 = CAN_BaudRateInitTab[CAN_GetBaudRateNum(baud)].BS1;
    CAN_InitConfig.CAN_BS2 = CAN_BaudRateInitTab[CAN_GetBaudRateNum(baud)].BS2;

    ret = CBL_Config(ui->deviceIndexComboBox->currentIndex(),
                     ui->channelIndexComboBox->currentIndex(),
                     &CAN_InitConfig,
                     &CMD_List);
    if(ret!=CBL_ERR_SUCCESS){
#ifdef LANGUE_EN
        QMessageBox::warning(this,"Warning","Config device faild!");
#else
        QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("配置设备失败！"));
#endif
        return false;
    }
    return true;
}
void MainWindow::on_updateFirmwarePushButton_clicked()
{
    QTime time;
    time.start();
    int ret;
    bool ConfFlag;
    uint32_t appversion,appType;
    uint8_t FirmwareData[1026]={0};
    if(ui->allNodeCheckBox->isChecked()){
        if(ui->nodeListTableWidget->rowCount()<=0){
#ifdef LANGUE_EN
            QMessageBox::warning(this,"Warning","No CAN node!");
#else
            QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("无任何节点！"));
#endif
            return;
        }
    }else{
        if(ui->nodeListTableWidget->currentIndex().row()<0){
#ifdef LANGUE_EN
            QMessageBox::warning(this,"Warning","Please Select a CAN node!");
#else
            QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("请选择节点！"));
#endif
            return;
        }
    }
    uint16_t NodeAddr;
    ConfFlag = DeviceConfig();
    if(!ConfFlag){
        CBL_CloseDevice(ui->deviceIndexComboBox->currentIndex());
        return;
    }
    if(ui->allNodeCheckBox->isChecked()){
        NodeAddr = 0x00;
        ret = CBL_ExcuteApp(ui->deviceIndexComboBox->currentIndex(),
                            ui->channelIndexComboBox->currentIndex(),
                            NodeAddr,
                            0x00);
        if(ret != CBL_ERR_SUCCESS){
#ifdef LANGUE_EN
            QMessageBox::warning(this,"Warning","Execute firmware faild!");
#else
            QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("执行固件程序失败！"));
#endif
            CBL_CloseDevice(ui->deviceIndexComboBox->currentIndex());
            return;
        }
        Sleep(500);
    }else{
        NodeAddr = ui->nodeListTableWidget->item(ui->nodeListTableWidget->currentIndex().row(),0)->text().toInt(NULL,16);
        ret = CBL_NodeCheck(ui->deviceIndexComboBox->currentIndex(),
                            ui->channelIndexComboBox->currentIndex(),
                            NodeAddr,
                            &appversion,
                            &appType,
                            500);
        if(ret == CBL_ERR_SUCCESS){
            if(appType != 0){//当前固件不为Bootloader
                ret = CBL_ExcuteApp(ui->deviceIndexComboBox->currentIndex(),
                                    ui->channelIndexComboBox->currentIndex(),
                                    NodeAddr,
                                    0x00);
                if(ret != CBL_ERR_SUCCESS){
#ifdef LANGUE_EN
                    QMessageBox::warning(this,"Warning","Execute firmware faild!");
#else
                    QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("执行固件程序失败！"));
#endif
                    CBL_CloseDevice(ui->deviceIndexComboBox->currentIndex());
                    return;
                }
                Sleep(500);
            }
        }else{
#ifdef LANGUE_EN
            QMessageBox::warning(this,"Warning","Check CAN node faild!");
#else
            QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("节点检测失败！"));
#endif
            CBL_CloseDevice(ui->deviceIndexComboBox->currentIndex());
            return;
        }
    }
    QFile firmwareFile(ui->firmwareLineEdit->text());
    if (firmwareFile.open(QFile::ReadOnly)){
        if(!ui->allNodeCheckBox->isChecked()){
            ret = CBL_NodeCheck(ui->deviceIndexComboBox->currentIndex(),
                                ui->channelIndexComboBox->currentIndex(),
                                NodeAddr,
                                &appversion,
                                &appType,
                                50);
            if(ret == CBL_ERR_SUCCESS){
                if(appType != 0){//当前固件不为Bootloader
#ifdef LANGUE_EN
                    QMessageBox::warning(this,"Warning","Current firmware is not bootloader!");
#else
                    QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("当前固件不为Bootloader固件！"));
#endif
                    CBL_CloseDevice(ui->deviceIndexComboBox->currentIndex());
                    return;
                }
            }else{
#ifdef LANGUE_EN
                QMessageBox::warning(this,"Warning","Check CAN node faild!");
#else
                QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("节点检测失败！"));
#endif
                CBL_CloseDevice(ui->deviceIndexComboBox->currentIndex());
                return;
            }
        }
        uint32_t startAddr = ui->startAddrLineEdit->text().toInt(NULL,16);
        ret = CBL_EraseFlash(ui->deviceIndexComboBox->currentIndex(),
                             ui->channelIndexComboBox->currentIndex(),
                             NodeAddr,
                             startAddr,
                             startAddr+firmwareFile.size(),
                             20000);
        qDebug()<<"firmwareFile.size() = "<<firmwareFile.size();
        if(ret != CBL_ERR_SUCCESS){
            qDebug()<<"CBL_EraseFlash = "<<ret;
#ifdef LANGUE_EN
            QMessageBox::warning(this,"Warning","Erase flash faild!");
#else
            QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("擦出Flash失败！"));
#endif
            CBL_CloseDevice(ui->deviceIndexComboBox->currentIndex());
            return;
        }
        if(ui->allNodeCheckBox->isChecked()){
            Sleep(1000);
        }
        int read_data_num;
#ifdef LANGUE_EN
        QProgressDialog writeDataProcess("Upgrade firmware...","Cancel",0,firmwareFile.size(),this);
        writeDataProcess.setWindowTitle("Upgrade firmware");
#else
        QProgressDialog writeDataProcess(QStringLiteral("正在更新固件..."),QStringLiteral("取消"),0,firmwareFile.size(),this);
        writeDataProcess.setWindowTitle(QStringLiteral("更新固件"));
#endif
        writeDataProcess.setModal(true);
        writeDataProcess.show();
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        int i=0;
        for(i=0;i<firmwareFile.size();i+=1024){
            read_data_num = firmwareFile.read((char*)FirmwareData,1024);
            uint16_t crc_data = crc16_ccitt(FirmwareData,read_data_num);
            FirmwareData[read_data_num] = crc_data>>8;
            FirmwareData[read_data_num+1] = crc_data;
            read_data_num += 2;
            qDebug()<<"read_data_num = "<<read_data_num;
            ret = CBL_WriteBlock(ui->deviceIndexComboBox->currentIndex(),
                                 ui->channelIndexComboBox->currentIndex(),
                                 NodeAddr,
                                 startAddr+i,
                                 FirmwareData,
                                 read_data_num,
                                 10000);
            qDebug()<<"ret = "<<ret;
            if(ret != CBL_ERR_SUCCESS){
#ifdef LANGUE_EN
                QMessageBox::warning(this,"Warning","Write flash faild!");
#else
                QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("写Flash数据失败！"));
#endif
                CBL_CloseDevice(ui->deviceIndexComboBox->currentIndex());
                return;
            }
            writeDataProcess.setValue(i);
            QCoreApplication::processEvents(QEventLoop::AllEvents);
            if(writeDataProcess.wasCanceled()){
                CBL_CloseDevice(ui->deviceIndexComboBox->currentIndex());
                return;
            }
            if(ui->allNodeCheckBox->isChecked()){
                Sleep(10);
            }
        }
        writeDataProcess.setValue(firmwareFile.size());
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        if(writeDataProcess.wasCanceled()){
            CBL_CloseDevice(ui->deviceIndexComboBox->currentIndex());
            return;
        }
        CBL_CloseDevice(ui->deviceIndexComboBox->currentIndex());
    }else{
        CBL_CloseDevice(ui->deviceIndexComboBox->currentIndex());
#ifdef LANGUE_EN
        QMessageBox::warning(this,"Warning","Open firmware file faild!");
#else
        QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("打开固件文件失败！"));
#endif
    }
    qDebug()<<time.elapsed()/1000.0<<"s";
}

void MainWindow::on_openFirmwareFileAction_triggered()
{
    on_openFirmwareFilePushButton_clicked();
}

void MainWindow::on_scanNodeAction_triggered()
{
//#define MAX_NODE_NUM    0x400
    #define MAX_NODE_NUM    0x80
    int ret;
    bool ConfFlag = DeviceConfig();
    if(!ConfFlag){
        CBL_CloseDevice(ui->deviceIndexComboBox->currentIndex());
        return;
    }
    ui->nodeListTableWidget->verticalHeader()->hide();
    ui->nodeListTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->nodeListTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->nodeListTableWidget->setRowCount(0);
#ifdef LANGUE_EN
    QProgressDialog scanNodeProcess("Scanning CAN node...","Cancel",0,MAX_NODE_NUM,this);
    scanNodeProcess.setWindowTitle("Scanning CAN node");
#else
    QProgressDialog scanNodeProcess(QStringLiteral("正在扫描节点..."),QStringLiteral("取消"),0,MAX_NODE_NUM,this);
    scanNodeProcess.setWindowTitle(QStringLiteral("扫描节点"));
#endif
    scanNodeProcess.setModal(true);
    scanNodeProcess.show();
    QCoreApplication::processEvents(QEventLoop::AllEvents);
    for(int i=1;i<MAX_NODE_NUM;i++){
        uint32_t appversion,appType;
        ret = CBL_NodeCheck(ui->deviceIndexComboBox->currentIndex(),
                            ui->channelIndexComboBox->currentIndex(),
                            i,
                            &appversion,
                            &appType,
                            10);

        if(ret == CBL_ERR_SUCCESS){

            ui->nodeListTableWidget->setRowCount(ui->nodeListTableWidget->rowCount()+1);
            ui->nodeListTableWidget->setRowHeight(ui->nodeListTableWidget->rowCount()-1,20);
            QString str;
            str.sprintf("0x%X",i);
            QTableWidgetItem *item = new QTableWidgetItem(str);
            ui->nodeListTableWidget->setItem(ui->nodeListTableWidget->rowCount()-1,0,item);
            if(appType == 0){
                str = "Boot";
            }else{
                str = "App";
            }
            item = new QTableWidgetItem(str);
            ui->nodeListTableWidget->setItem(ui->nodeListTableWidget->rowCount()-1,1,item);
            str.sprintf("v%d.%d",(((appversion>>24)&0xFF)*10)+(appversion>>16)&0xFF,(((appversion>>8)&0xFF)*10)+appversion&0xFF);
            item = new QTableWidgetItem(str);
            ui->nodeListTableWidget->setItem(ui->nodeListTableWidget->rowCount()-1,2,item);
        }
        else
        {
            //qDebug()<<"CBL_NodeCheck ret = "<<ret;

        }
        scanNodeProcess.setValue(i);
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        if(scanNodeProcess.wasCanceled()){
            CBL_CloseDevice(ui->deviceIndexComboBox->currentIndex());
            return;
        }
    }
    CBL_CloseDevice(ui->deviceIndexComboBox->currentIndex());
}

void MainWindow::on_enterUpdatePushButton_clicked()
{
    int ret;
    if(ui->allNodeCheckBox->isChecked()){
        if(ui->nodeListTableWidget->rowCount()<=0){
#ifdef LANGUE_EN
            QMessageBox::warning(this,"Warning","No CAN node!");
#else
            QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("无任何节点！"));
#endif
            return;
        }
    }else{
        if(ui->nodeListTableWidget->currentIndex().row()<0){
#ifdef LANGUE_EN
            QMessageBox::warning(this,"Warning","Please select a CAN node!");
#else
            QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("请选择节点！"));
#endif
            return;
        }
    }
    bool ConfFlag = DeviceConfig();
    if(!ConfFlag){
        CBL_CloseDevice(ui->deviceIndexComboBox->currentIndex());
        return;
    }
    uint16_t NodeAddr;
    if(ui->allNodeCheckBox->isChecked()){
        NodeAddr = 0x00;
    }else{
        NodeAddr = ui->nodeListTableWidget->item(ui->nodeListTableWidget->currentIndex().row(),0)->text().toInt(NULL,16);
    }
    ret = CBL_EnterUpdateMode(ui->deviceIndexComboBox->currentIndex(),
                        ui->channelIndexComboBox->currentIndex(),
                        NodeAddr);
    if(ret != CBL_ERR_SUCCESS){
#ifdef LANGUE_EN
        QMessageBox::warning(this,"Warning","enter update mode failed");
#else
        QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("进入升级模式失败！"));
#endif
    }
    CBL_CloseDevice(ui->deviceIndexComboBox->currentIndex());
}

void MainWindow::on_executeFirmwarePushButton_clicked()
{
    int ret;
    if(ui->allNodeCheckBox->isChecked()){
        if(ui->nodeListTableWidget->rowCount()<=0){
#ifdef LANGUE_EN
            QMessageBox::warning(this,"Warning","No CAN node!");
#else
            QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("无任何节点！"));
#endif
            return;
        }
    }else{
        if(ui->nodeListTableWidget->currentIndex().row()<0){
#ifdef LANGUE_EN
            QMessageBox::warning(this,"Warning","Please select a CAN node!");
#else
            QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("请选择节点！"));
#endif
            return;
        }
    }
    bool ConfFlag = DeviceConfig();
    if(!ConfFlag){
        CBL_CloseDevice(ui->deviceIndexComboBox->currentIndex());
        return;
    }
    uint16_t NodeAddr;
    if(ui->allNodeCheckBox->isChecked()){
        NodeAddr = 0x00;
    }else{
        NodeAddr = ui->nodeListTableWidget->item(ui->nodeListTableWidget->currentIndex().row(),0)->text().toInt(NULL,16);
    }
    ret = CBL_ExcuteApp(ui->deviceIndexComboBox->currentIndex(),
                        ui->channelIndexComboBox->currentIndex(),
                        NodeAddr,
                        ui->startAddrLineEdit->text().toInt(NULL,16));
    if(ret != CBL_ERR_SUCCESS){
#ifdef LANGUE_EN
        QMessageBox::warning(this,"Warning","Execute firmware faild!");
#else
        QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("执行固件文件失败！"));
#endif
    }
    CBL_CloseDevice(ui->deviceIndexComboBox->currentIndex());
}

void MainWindow::on_setbaudRatePushButton_clicked()
{
    int ret;
    VCI_INIT_CONFIG_EX CAN_InitConfig;
    if(ui->allNodeCheckBox->isChecked()){
        if(ui->nodeListTableWidget->rowCount()<=0){
#ifdef LANGUE_EN
            QMessageBox::warning(this,"Warning","No CAN node!");
#else
            QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("无任何节点！"));
#endif
            return;
        }
    }else{
        if(ui->nodeListTableWidget->currentIndex().row()<0){
#ifdef LANGUE_EN
            QMessageBox::warning(this,"Warning","Please select a CAN node!");
#else
            QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("请选择节点！"));
#endif
            return;
        }
    }
    bool ConfFlag = DeviceConfig();
    if(!ConfFlag){
        CBL_CloseDevice(ui->deviceIndexComboBox->currentIndex());
        return;
    }
    QString str = ui->newBaudRateComboBox->currentText();
    str.resize(str.length()-4);
    int baud = str.toInt(NULL,10)*1000;
    CAN_InitConfig.CAN_BRP = CAN_BaudRateInitTab[CAN_GetBaudRateNum(baud)].PreScale;
    CAN_InitConfig.CAN_SJW = CAN_BaudRateInitTab[CAN_GetBaudRateNum(baud)].SJW;
    CAN_InitConfig.CAN_BS1 = CAN_BaudRateInitTab[CAN_GetBaudRateNum(baud)].BS1;
    CAN_InitConfig.CAN_BS2 = CAN_BaudRateInitTab[CAN_GetBaudRateNum(baud)].BS2;
    uint16_t NodeAddr;
    if(ui->allNodeCheckBox->isChecked()){
        NodeAddr = 0x00;
    }else{
        NodeAddr = ui->nodeListTableWidget->item(ui->nodeListTableWidget->currentIndex().row(),0)->text().toInt(NULL,16);
    }
    ret = CBL_SetBaudRate(ui->deviceIndexComboBox->currentIndex(),
                          ui->channelIndexComboBox->currentIndex(),
                          NodeAddr,
                          &CAN_InitConfig,
                          baud,
                          500);
    if(ret != CBL_ERR_SUCCESS){
        CBL_CloseDevice(ui->deviceIndexComboBox->currentIndex());
#ifdef LANGUE_EN
        QMessageBox::warning(this,"Warning","Set baud rate faild!");
#else
        QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("设置波特率失败！"));
#endif
        return;
    }
    ui->baudRateComboBox->setCurrentIndex(ui->newBaudRateComboBox->currentIndex());
    CBL_CloseDevice(ui->deviceIndexComboBox->currentIndex());
}

void MainWindow::on_contactUsAction_triggered()
{
    QString AboutStr;
#ifdef LANGUE_EN
    AboutStr.append("Official website<span style=\"font-size:12px;\">&nbsp;</span>:&nbsp;<a href=\"http://www.viewtool.com\">www.viewtool.com</a><br>");
    AboutStr.append("Official BBS<span style=\"font-size:12px;\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>:&nbsp;<a href=\"http://www.viewtool.com/bbs\">www.viewtool.com/bbs</a><br>");
    AboutStr.append("Official Taobao<span style=\"font-size: 9px;\">&nbsp;</span>:&nbsp;<a href=\"http://viewtool.taobao.com/\">viewtool.taobao.com</a><br>");
    AboutStr.append("Technical Suuport QQ&nbsp;:&nbsp;1713047813<br>");
    AboutStr.append("Product Consultant QQ&nbsp;:&nbsp;191431236");
    QMessageBox::about(this,"Contact us",AboutStr);
#else
    AboutStr.append(QStringLiteral("官方网站")+"<span style=\"font-size:12px;\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>：<a href=http://www.viewtool.com\">www.viewtool.com</a><br>");
    AboutStr.append(QStringLiteral("官方论坛")+"<span style=\"font-size:12px;\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>：<a href=http://www.viewtool.com/bbs\">www.viewtool.com/bbs</a><br>");
    AboutStr.append(QStringLiteral("官方淘宝店")+"<span style=\"font-size:9px;\">&nbsp;&nbsp;&nbsp;</span>：<a href=http://viewtool.taobao.com/\">viewtool.taobao.com</a><br>");
    AboutStr.append(QStringLiteral("技术支持")+"QQ：1713047813<br>");
    AboutStr.append(QStringLiteral("产品咨询")+"QQ：191431236");
    QMessageBox::about(this,QStringLiteral("联系我们"),AboutStr);
#endif
}

void MainWindow::on_aboutAction_triggered()
{
    QString AboutStr;
    AboutStr = "Ginkgo USB-CAN Bootloader 1.2.0\n";
    AboutStr.append("ViewTool Virtual Instrument 2009-2013 Copyright\n");
    AboutStr.append("Hardware Suuport: Ginkgo 2 USB-CAN Adapter");
    QMessageBox::about(this,"About Ginkgo USB-CAN Bootloader",AboutStr);
}

void MainWindow::on_exitAction_triggered()
{
    this->close();
}

void MainWindow::on_actionCAN_BaudRateCalculator_triggered()
{
    QProcess *tool=new QProcess();
    tool->start("\"./tools/Ginkgo USB-CAN Baudrate.exe\"");
}

