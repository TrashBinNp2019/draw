#include "canvas.h"

Canvas::Canvas( QGraphicsScene *scene, QWidget *parent )
    : QGraphicsView( scene, parent )
{
}

const QPointF *Canvas::getLastPressed() const
{
    return lastPressed;
}

void Canvas::mousePressEvent(QMouseEvent *ev)
{
    if ( ev->button() == Qt::LeftButton ) {
        isBeingEdited = true;
        emit mousePressed( ev->modifiers(), ev->position() );
        lastPressed = new QPointF( ev->pos() );
    } else
    if ( ev->button() == Qt::RightButton && isBeingEdited ) {
        emit cancelEditing();
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent *ev)
{
    if ( ev->button() == Qt::LeftButton ) {
        isBeingEdited = false;
        emit mouseReleased( ev->modifiers(), ev->position() );
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *ev)
{
    if ( isBeingEdited ) {
        emit mouseMoved( ev->modifiers(), ev->position() );
    }
}
