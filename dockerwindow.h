#ifndef DOCKERWINDOW_H
#define DOCKERWINDOW_H

#include "canvas.h"
#include "documentmanager.h"

#include <QLabel>
#include <QListWidgetItem>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class DockerWindow : public QMainWindow
{
    Q_OBJECT

public:
    DockerWindow(QWidget *parent = nullptr);
    ~DockerWindow();

private slots:
    void onActionNewTriggered();
    void onActionSaveTriggered();
    void onActionSaveAsTriggered();
    void onActionOpenTriggered();
    void onActionCloseTriggered();
    void onActionExitTriggered();

    void resizeToContents();

    void onPenColorChange();
    void onBrushColorChange();

    void onCanvasMouseReleased( Qt::KeyboardModifiers, QPointF );
    void onCanvasMousePressed( Qt::KeyboardModifiers, QPointF );
    void onCanvasMouseMoved( Qt::KeyboardModifiers, QPointF );

    void on_RectButton_clicked();
    void on_CircleButton_clicked();
    void on_LineButton_clicked();
    void on_PathButton_clicked();

    void on_RotateButton_clicked();
    void on_ScaleButton_clicked();
    void on_DeleteButton_clicked();
    void on_CancelButton_clicked();

    void on_PenColorButton_released();
    void on_BrushColorButton_released();

    void on_widthDropDown_currentIndexChanged(int index);

    void on_Scene_Contents_Changed();

    void on_shapesList_currentItemChanged( QListWidgetItem *, QListWidgetItem * );

    void on_shapesList_itemSelectionChanged();

private:
    enum Action { Add, Scale, Rotate, Move } action;

    Ui::MainWindow *ui;

    void enableDisplay();
    void disableDisplay();

    void setButtonModeAdd();
    void setButtonModeChange();

    bool isDisplayEnabled;

    Canvas *canvas;
    DocumentManager doc;
};
#endif // DOCKERWINDOW_H
