#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "mainwindow.h"


AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AboutDialog) {
    ui->setupUi(this);
    ui->textLbl->setText( QStringLiteral(
        R"HTML(<html><head/><body><p align="center"><span style=" font-size:14pt; font-weight:700;">%1 %2</span></p>)HTML"
        R"HTML(<p align="center"><span style=" font-size:11pt;">by Dr. Richard D. Kaminsky</span></p>)HTML"
        R"HTML(<p align="center"><span style=" font-size:11pt;">%3</span></p>)HTML"
        R"HTML(<p align="center"><span style=" font-size:11pt;">%4</span></p>)HTML"
        R"HTML(<p align="center"><span style=" font-size:10pt;"><br/>Developed using %5 &nbsp; https://www.qt.io/</span></p>)HTML"
        "</body></html>" ).arg(APP_TITLE, APP_VERSION, APP_BUILD_DATE, APP_COPYRIGHT, APP_QT_VERSION)
    );
}


AboutDialog::~AboutDialog() {
    delete ui;
}
