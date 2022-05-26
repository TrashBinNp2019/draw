#ifndef CANVAS_H
#define CANVAS_H

#include <QGraphicsView>
#include <QMouseEvent>


class Canvas : public QGraphicsView
{
    Q_OBJECT

public:
    Canvas( QGraphicsScene *, QWidget * = nullptr );
    const QPointF *getLastPressed() const;

private slots:
    void mousePressEvent( QMouseEvent *ev ) override;
    void mouseReleaseEvent( QMouseEvent *ev ) override;
    void mouseMoveEvent( QMouseEvent *ev ) override;

signals:
    void mousePressed( Qt::KeyboardModifiers, QPointF );
    void mouseReleased( Qt::KeyboardModifiers, QPointF );
    void mouseMoved( Qt::KeyboardModifiers, QPointF );
    void cancelEditing();

private:
    bool isBeingEdited;
    QPointF *lastPressed;

};

#endif // CANVAS_H
