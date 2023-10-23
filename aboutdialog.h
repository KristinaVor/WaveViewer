#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

private:
    QString aboutText;
};

#endif // ABOUTDIALOG_H
