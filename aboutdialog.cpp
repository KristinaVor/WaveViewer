#include "aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("About");
    setMinimumSize(300, 200);

    aboutText = "Версия ПО: 1.0\n"
                "Автор: Ваше имя\n"
                "Описание: Произвольная информация о вашем ПО.";

    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel(aboutText, this);
    layout->addWidget(label);
    setLayout(layout);
}

AboutDialog::~AboutDialog()
{
    // Деструктор окна "О программе" здесь
}
