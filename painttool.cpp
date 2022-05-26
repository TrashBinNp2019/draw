#include "painttool.h"

#include <QGraphicsRectItem>


PaintTool::Shapes PaintTool::getCurrentShape() const
{
    return currentShape;
}

void PaintTool::setShape( Shapes shape )
{
    currentShape = shape;
}

PaintTool::PaintTool( QObject *parent )
    : QObject( parent )
    , pen()
    , brush()
{
}

QGraphicsItem *PaintTool::makeItem( int x, int y )
{
    switch ( currentShape ) {
    case Rect: {
        auto item = new QGraphicsRectItem( x, y, 0, 0 );
        item->setPen( pen );
        item->setBrush( brush );
        last = item;

        return last;
        break;
    }
    case Circle: {
        auto item = new QGraphicsEllipseItem( x, y, 0, 0 );
        item->setPen( pen );
        item->setBrush( brush );
        last = item;

        return last;
        break;
    }
    case Line: {
        auto item = new QGraphicsLineItem( x, y, x, y );
        item->setPen( pen );
        last = item;

        return last;
        break;
    }
    case Path: {
        auto item = new QGraphicsPathItem();
        QPainterPath path;
        path.moveTo( x, y );
        item->setPath( path );
        item->setPen( pen );
        last = item;

        return last;
        break;
    }
    default: {
        break;
    }
    }
}

QPen &PaintTool::getPenRef()
{
    return pen;
}

const QPen &PaintTool::getPen() const
{
    return pen;
}

void PaintTool::setPen(const QPen &newPen)
{
    if (pen == newPen)
        return;
    pen = newPen;
    emit penChanged();
}

QBrush &PaintTool::getBrushRef()
{
    return brush;
}

const QBrush &PaintTool::getBrush() const
{
    return brush;
}

void PaintTool::setBrush(const QBrush &newBrush)
{
    if (brush == newBrush)
        return;
    brush = newBrush;
    emit brushChanged();
}

QGraphicsItem *PaintTool::getLast() const
{
    return last;
}

void PaintTool::resetLast()
{
    last = 0;
}
