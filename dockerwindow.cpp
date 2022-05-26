#include "dockerwindow.h"
#include "ui_dockerwindow.h"
#include <QTimer>
#include <QSizePolicy>
#include <QGraphicsItem>
#include <QTransform>
#include <QColorDialog>
#include <QFileDialog>
#include <QStandardPaths>


DockerWindow::DockerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isDisplayEnabled( false )
{
    ui->setupUi(this);

    connect( ui->actionNew, SIGNAL( triggered() ), this, SLOT( onActionNewTriggered() ) );
    connect( ui->actionOpen, SIGNAL( triggered() ), this, SLOT( onActionOpenTriggered() ) );
    connect( ui->actionSave, SIGNAL( triggered() ), this, SLOT( onActionSaveTriggered() ) );
    connect( ui->actionSave_as, SIGNAL( triggered() ), this, SLOT( onActionSaveAsTriggered() ) );
    connect( ui->actionClose, SIGNAL( triggered() ), this, SLOT( onActionCloseTriggered() ) );
    connect( ui->actionExit, SIGNAL( triggered() ), this, SLOT( onActionExitTriggered() ) );

    connect( &doc.getPaintTool(), SIGNAL( penChanged() ), this, SLOT( onPenColorChange() ));
    connect( &doc.getPaintTool(), SIGNAL( brushChanged() ), this, SLOT( onBrushColorChange() ));

    connect( &doc, SIGNAL( itemsListChanged() ), this, SLOT( on_Scene_Contents_Changed() ) );
}

DockerWindow::~DockerWindow()
{
    delete ui;
}


void DockerWindow::onActionNewTriggered()
{
    doc.init();
    enableDisplay();
    doc.newScene( { 0, 0, canvas->width() - 10, canvas->height() - 10 }, QColorConstants::White );
}

void DockerWindow::onActionSaveTriggered()
{
    doc.write( "hello1.svg" );
}

void DockerWindow::onActionSaveAsTriggered()
{
    QFileDialog dlg{
                     this,
                     "Save to file",
                     QStandardPaths::displayName( QStandardPaths::PicturesLocation ),
                     "Scalable Vector Graphics (*.svg)"
                   };
    dlg.setDefaultSuffix( "svg" );

    switch ( dlg.exec() ) {
    case dlg.Accepted: {
        doc.write( dlg.selectedFiles().first() );
        break;
    }
    case dlg.Rejected: {
        break;
    }
    default:
        break;
    }
}

void DockerWindow::onActionOpenTriggered()
{
    doc.init();
    enableDisplay();

    QFileDialog dlg{
                     this,
                     "Open file",
                     QStandardPaths::displayName( QStandardPaths::PicturesLocation ),
                     "Scalable Vector Graphics (*.svg)"
                   };
    dlg.setDefaultSuffix( "svg" );

    switch ( dlg.exec() ) {
    case dlg.Accepted: {
        doc.read( dlg.selectedFiles().first() );
        break;
    }
    case dlg.Rejected: {
        break;
    }
    default:
        break;
    }
}

void DockerWindow::onActionCloseTriggered()
{
    doc.closeScene();
    disableDisplay();
}

void DockerWindow::onActionExitTriggered()
{
    qApp->exit();
}

void DockerWindow::resizeToContents()
{
    resize( minimumSizeHint() );
}

void DockerWindow::onPenColorChange()
{
    QPalette pal{};
    pal.setColor( QPalette::Button, doc.getPenColor());
    pal.setColor( QPalette::Base, doc.getPenColor());

    ui->PenColorButton->setAutoFillBackground( true );
    ui->PenColorButton->setPalette( pal );
}

void DockerWindow::onBrushColorChange()
{
    QPalette pal{};
    pal.setColor( QPalette::Button, doc.getBrushColor() );

    ui->BrushColorButton->setAutoFillBackground( true );
    ui->BrushColorButton->setPalette( pal );
}

void DockerWindow::onCanvasMouseReleased( Qt::KeyboardModifiers, QPointF )
{
    switch ( action ) {
    case Add: {
        doc.finishAdding();
        break;
    }
    case Scale: {
        break;
    }
    case Rotate: {
        break;
    }
    case Move: {
        if ( ui->RectButton->isFlat() ) {
            action = Rotate;
        } else {
            action = Scale;
        }
        break;
    }
    default:
        break;
    }
}

void DockerWindow::onCanvasMousePressed( Qt::KeyboardModifiers mods, QPointF point )
{
    switch ( action ) {
    case Add: {
        doc.addItem( point.x(), point.y() );
        break;
    }
    case Scale: {
        doc.setSelectedItem( ui->shapesList->currentItem()->text() );

        if ( doc.getSelectedItem()->boundingRect().contains( point ) ) {
            action = Move;

            ui->RectButton->setFlat( true );
            ui->CircleButton->setFlat( false );

            doc.saveSelectedTranslation();
        } else {
            doc.saveSelectedScale();
        }
        break;
    }
    case Rotate: {
        doc.setSelectedItem( ui->shapesList->currentItem()->text() );

        if ( doc.getSelectedItem()->boundingRect().contains( point ) ) {
            action = Move;

            ui->RectButton->setFlat( false );
            ui->CircleButton->setFlat( true );

            doc.saveSelectedTranslation();
        } else {
            doc.saveSelectedRotation();
        }
        break;
    }
    case Move: {
        break;
    }
    default:
        break;
    }
}

void DockerWindow::onCanvasMouseMoved( Qt::KeyboardModifiers mods, QPointF point )
{
    switch ( action ) {
    case Add: {
        if ( !doc.getLast() || !canvas->getLastPressed() ) return;

        doc.redrawLast( *canvas->getLastPressed(), point );
        break;
    }
    case Scale: {
        auto scaleFrom = *canvas->getLastPressed() - doc.getSelectedItem()->boundingRect().center();
        auto scaleTo = point - doc.getSelectedItem()->boundingRect().center();

        auto scale = sqrt( scaleTo.x() * scaleTo.x() + scaleTo.y() * scaleTo.y() ) /
                     sqrt( scaleFrom.x() * scaleFrom.x() + scaleFrom.y() * scaleFrom.y() );

        doc.scaleSelectedItem( scale );
        break;
    }
    case Rotate: {
        auto [ x1, y1 ] = point - doc.getSelectedItem()->boundingRect().center();
        auto [ x2, y2 ] = *canvas->getLastPressed() - doc.getSelectedItem()->boundingRect().center();
        float angle1 = atan( y1 / x1 ) * ( 180 / M_PI );
        float angle2 = atan( y2 / x2 ) * ( 180 / M_PI );
        if ( x1 < 0 ) angle1 += 180;
        if ( x2 < 0 ) angle2 += 180;

        doc.rotateSelectedItem( angle1 - angle2 );
        break;
    }
    case Move: {
        doc.translateSelectedItem( point - *canvas->getLastPressed() );
    }
    default:
        break;
    }
}


void DockerWindow::enableDisplay()
{
    if ( isDisplayEnabled ) return;

    canvas = new Canvas( &doc.getScene(), this );

    canvas->setSizePolicy( { QSizePolicy::Expanding, QSizePolicy::Expanding } );
    canvas->setMinimumSize( { 647, 400 } );
    connect( canvas, SIGNAL( mouseReleased( Qt::KeyboardModifiers, QPointF ) ),
             this, SLOT( onCanvasMouseReleased( Qt::KeyboardModifiers, QPointF ) ) );
    connect( canvas, SIGNAL( mousePressed( Qt::KeyboardModifiers, QPointF ) ),
             this, SLOT( onCanvasMousePressed( Qt::KeyboardModifiers, QPointF ) ) );
    connect( canvas, SIGNAL( mouseMoved( Qt::KeyboardModifiers, QPointF ) ),
             this, SLOT( onCanvasMouseMoved( Qt::KeyboardModifiers, QPointF ) ) );
    connect( canvas, SIGNAL( cancelEditing() ), &doc, SLOT( cancelEditing() ) );

    ui->horizontalLayout->insertWidget( 0, canvas );
    isDisplayEnabled = true;

    ui->RectButton->setFlat( false );
    ui->CircleButton->setFlat( true );
    ui->LineButton->setFlat( true );
    ui->PathButton->setFlat( true );

    ui->widthDropDown->addItem( "1px", { 1 } );
    ui->widthDropDown->addItem( "2px", { 2 } );
    ui->widthDropDown->addItem( "4px", { 5 } );
    ui->widthDropDown->addItem( "10px", { 10 } );
    ui->widthDropDown->addItem( "15px", { 15 } );

    QTimer::singleShot( 0, this, SLOT( resizeToContents() ) ); // queue resize to the end of event loop
}

void DockerWindow::disableDisplay()
{
    if ( !isDisplayEnabled ) return;

    ui->RectButton->setFlat( false );
    ui->CircleButton->setFlat( false );
    ui->LineButton->setFlat( false );
    ui->PathButton->setFlat( false );

    ui->centralwidget->layout()->removeWidget( canvas );
    delete canvas;

    ui->widthDropDown->clear();
    ui->shapesList->clear();

    isDisplayEnabled = false;
}

void DockerWindow::setButtonModeAdd()
{
    action = Add;

    ui->RectButton->setText( "R" );
    ui->CircleButton->setText( "C" );
    ui->LineButton->setText( "L" );
    ui->PathButton->setText( "P" );

    ui->RectButton->setFlat( false );
    ui->CircleButton->setFlat( true );
    ui->LineButton->setFlat( true );
    ui->PathButton->setFlat( true );

    doc.getPaintTool().setShape( PaintTool::Rect );

    connect( ui->RectButton, SIGNAL( clicked() ), this, SLOT( on_RectButton_clicked() ) );
    connect( ui->CircleButton, SIGNAL( clicked() ), this, SLOT( on_CircleButton_clicked() ) );
    connect( ui->LineButton, SIGNAL( clicked() ), this, SLOT( on_LineButton_clicked() ) );
    connect( ui->PathButton, SIGNAL( clicked() ), this, SLOT( on_PathButton_clicked() ) );

    disconnect( ui->RectButton, SIGNAL( clicked() ), this, SLOT( on_RotateButton_clicked() ) );
    disconnect( ui->CircleButton, SIGNAL( clicked() ), this, SLOT( on_ScaleButton_clicked() ) );
    disconnect( ui->LineButton, SIGNAL( clicked() ), this, SLOT( on_DeleteButton_clicked() ) );
    disconnect( ui->PathButton, SIGNAL( clicked() ), this, SLOT( on_CancelButton_clicked() ) );
}

void DockerWindow::setButtonModeChange()
{
    action = Rotate;

    ui->RectButton->setText( "R" );
    ui->CircleButton->setText( "S" );
    ui->LineButton->setText( "D" );
    ui->PathButton->setText( "C" );

    ui->RectButton->setFlat( false );
    ui->CircleButton->setFlat( true );
    ui->LineButton->setFlat( false );
    ui->PathButton->setFlat( false );

    disconnect( ui->RectButton, SIGNAL( clicked() ), this, SLOT( on_RectButton_clicked() ) );
    disconnect( ui->CircleButton, SIGNAL( clicked() ), this, SLOT( on_CircleButton_clicked() ) );
    disconnect( ui->LineButton, SIGNAL( clicked() ), this, SLOT( on_LineButton_clicked() ) );
    disconnect( ui->PathButton, SIGNAL( clicked() ), this, SLOT( on_PathButton_clicked() ) );

    connect( ui->RectButton, SIGNAL( clicked() ), this, SLOT( on_RotateButton_clicked() ) );
    connect( ui->CircleButton, SIGNAL( clicked() ), this, SLOT( on_ScaleButton_clicked() ) );
    connect( ui->LineButton, SIGNAL( clicked() ), this, SLOT( on_DeleteButton_clicked() ) );
    connect( ui->PathButton, SIGNAL( clicked() ), this, SLOT( on_CancelButton_clicked() ) );
}

void DockerWindow::on_RectButton_clicked()
{
    if ( !isDisplayEnabled ) return;

    ui->RectButton->setFlat( false );
    ui->CircleButton->setFlat( true );
    ui->LineButton->setFlat( true );
    ui->PathButton->setFlat( true );

    doc.getPaintTool().setShape( PaintTool::Rect );
}

void DockerWindow::on_CircleButton_clicked()
{
    if ( !isDisplayEnabled ) return;

    ui->RectButton->setFlat( true );
    ui->CircleButton->setFlat( false );
    ui->LineButton->setFlat( true );
    ui->PathButton->setFlat( true );

    doc.getPaintTool().setShape( PaintTool::Circle );
}


void DockerWindow::on_LineButton_clicked()
{
    if ( !isDisplayEnabled ) return;

    ui->RectButton->setFlat( true );
    ui->CircleButton->setFlat( true );
    ui->LineButton->setFlat( false );
    ui->PathButton->setFlat( true );

    doc.getPaintTool().setShape( PaintTool::Line );
}


void DockerWindow::on_PathButton_clicked()
{
    if ( !isDisplayEnabled ) return;

    ui->RectButton->setFlat( true );
    ui->CircleButton->setFlat( true );
    ui->LineButton->setFlat( true );
    ui->PathButton->setFlat( false );

    doc.getPaintTool().setShape( PaintTool::Path );
}

void DockerWindow::on_RotateButton_clicked()
{
    action = Rotate;

    ui->RectButton->setFlat( false );
    ui->CircleButton->setFlat( true );
}

void DockerWindow::on_ScaleButton_clicked()
{
    action = Scale;

    ui->RectButton->setFlat( true );
    ui->CircleButton->setFlat( false );
}

void DockerWindow::on_DeleteButton_clicked()
{
    doc.deleteItem( ui->shapesList->currentItem()->text() );
    setButtonModeAdd();
}

void DockerWindow::on_CancelButton_clicked()
{
    ui->shapesList->clearSelection();
    setButtonModeAdd();
}


void DockerWindow::on_PenColorButton_released()
{
    if ( !isDisplayEnabled ) return;

    QColorDialog dlg{ doc.getPenColor(), this };

    switch ( dlg.exec() ) {
    case dlg.Accepted: {
        doc.getPaintTool().setPen( { dlg.currentColor() } );
        break;
    }
    case dlg.Rejected: {
        break;
    }
    default:
        break;
    }
}


void DockerWindow::on_BrushColorButton_released()
{
    if ( !isDisplayEnabled ) return;

    QColorDialog dlg{ doc.getBrushColor(), this };

    switch ( dlg.exec() ) {
    case dlg.Accepted: {
        doc.getPaintTool().setBrush( { dlg.currentColor() } );
        break;
    }
    case dlg.Rejected: {
        break;
    }
    default:
        break;
    }
}


void DockerWindow::on_widthDropDown_currentIndexChanged( int )
{
    if ( !isDisplayEnabled ) return;

    doc.setPenWidth( ui->widthDropDown->currentData().toInt() );
}

void DockerWindow::on_Scene_Contents_Changed()
{
    ui->shapesList->clear();

    for ( auto item : doc.getNamedItemsList() ) {
        QListWidgetItem listItem{ item.second };
        ui->shapesList->addItem( item.second );
    }
}


void DockerWindow::on_shapesList_currentItemChanged( QListWidgetItem *current, QListWidgetItem *previous )
{

}


void DockerWindow::on_shapesList_itemSelectionChanged()
{
    setButtonModeChange();
}

