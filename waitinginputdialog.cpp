#include "waitinginputdialog.h"
#include "ui_waitinginputdialog.h"

#include <QPushButton>


WaitingInputDialog::WaitingInputDialog( QWidget *parent )
    : QDialog( parent )
    , ui( new Ui::WaitingInputDialog )
{
    ui->setupUi( this );

    ui->optionsComboBox->clear();
    ui->optionsComboBox->addItem( "..." );

    ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( false );

    ui->label->setText( "Loading..." );
}

QString WaitingInputDialog::getSelectedOption()
{
    return ui->optionsComboBox->currentText();
}

void WaitingInputDialog::setOptions( const QStringList &options )
{
    if ( options.length() > 0 ) {
        ui->label->setText( "Choose file:" );

        ui->optionsComboBox->clear();
        ui->optionsComboBox->addItems( options );
        ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( true );
    } else {
        ui->label->setText( "No files found" );
    }
}
