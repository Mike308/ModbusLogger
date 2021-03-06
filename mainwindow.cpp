#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    timer = new QTimer();
    statusLabel = new QLabel();





    QList<QSerialPortInfo> comPorts = QSerialPortInfo::availablePorts();
    QList<qint32> baudListInfo = QSerialPortInfo::standardBaudRates();


    foreach (QSerialPortInfo port, comPorts){

        ui->portList->addItem(port.portName());

    }

    foreach (qint32 baudList, baudListInfo) {

        ui->baudList->addItem(QString::number(baudList));

    }

    statusLabel->setText("Waiting..");
    ui->statusBar->addWidget(statusLabel);


    ui->disconnectButton->setEnabled(false);









}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_connectButton_clicked(){

    modbusMaster = new Modbus();


    if(modbusMaster->connectToSlave(ui->portList->currentText(),ui->baudList->currentText().toInt())){

        connect(timer,SIGNAL(timeout()),this,SLOT(onTimeoutSlot()));
        connect(modbusMaster,SIGNAL(onReadReady(QModbusDataUnit)),this,SLOT(onReadReadySlot(QModbusDataUnit)));
        connect(modbusMaster,SIGNAL(onReadError(QString)),this,SLOT(onReadError(QString)));
        timer->start(5000);
        ui->connectButton->setEnabled(false);
        ui->disconnectButton->setEnabled(true);


    }else{

        statusLabel->setText("Error");
    }












}

void MainWindow::onReadReadySlot(QModbusDataUnit reg){

    //timer->stop();

    int t1 = reg.value(0);
    int t2 = reg.value(1);

    ui->t1Label->setText(QString::number(t1));
    ui->t2Label->setText(QString::number(t2));
    ui->t1Gauge->setValue(t1);
    ui->t2Gauge->setValue(t2);

    // timer->start();




}

void MainWindow::on_pushButton_clicked(){

    modbusMaster->executeWriteRequest(10,10,Modbus::HoldingRegisters,ui->rDial->value());
    modbusMaster->executeWriteRequest(10,11,Modbus::HoldingRegisters,ui->gDial->value());
    modbusMaster->executeWriteRequest(10,12,Modbus::HoldingRegisters,ui->bDial->value());



}

void MainWindow::onTimeoutSlot(){

    static int i = 0;
    i++;
    modbusMaster->executeReadRequest(10,100,Modbus::InputRegisters);

      qDebug () << "Debug counter: "<<i;
}

void MainWindow::onReadError(QString msg){

    statusLabel->setText(msg);


}

void MainWindow::on_led1_stateChanged(int arg1)
{
    if (ui->led1->isChecked()){

        modbusMaster->executeWriteRequest(10,50,Modbus::Coils,1);

    }else{

        modbusMaster->executeWriteRequest(10,50,Modbus::Coils,0);
    }
}

void MainWindow::on_led2_stateChanged(int arg1)
{
    if (ui->led2->isChecked()){

        modbusMaster->executeWriteRequest(10,51,Modbus::Coils,1);
    }else{

        modbusMaster->executeWriteRequest(10,51,Modbus::Coils,0);
    }
}

void MainWindow::on_rDial_actionTriggered(int action)
{
    int val = ui->rDial->value();
    ui->rLabel->setText(QString::number(val));

}

void MainWindow::on_gDial_actionTriggered(int action)
{
    int val = ui->rDial->value();
    ui->gLabel->setText(QString::number(val));
}

void MainWindow::on_MainWindow_destroyed()
{
    delete modbusMaster;
}

void MainWindow::on_disconnectButton_clicked()
{
    timer->stop();
    disconnect(modbusMaster,SIGNAL(onReadReady(QModbusDataUnit)),this,0);
    disconnect(modbusMaster,SIGNAL(onReadError(QString)),this,0);


    modbusMaster->disconnectSlave();
    ui->connectButton->setEnabled(true);
    ui->disconnectButton->setEnabled(false);
}
