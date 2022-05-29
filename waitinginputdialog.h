#ifndef WAITINGINPUTDIALOG_H
#define WAITINGINPUTDIALOG_H

#include <QDialog>


QT_BEGIN_NAMESPACE
namespace Ui { class WaitingInputDialog; }
QT_END_NAMESPACE

class WaitingInputDialog : public QDialog
{
    Q_OBJECT

public:
    WaitingInputDialog( QWidget * = nullptr );
    QString getSelectedOption();

public slots:
    void setOptions( const QStringList & );

private:
    Ui::WaitingInputDialog *ui;
};

#endif // WAITINGINPUTDIALOG_H
