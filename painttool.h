#ifndef PAINTTOOL_H
#define PAINTTOOL_H

#include <QPainter>
#include <QGraphicsItem>


class PaintTool : public QObject
{
    Q_OBJECT

public:
    enum Shapes { Rect, Circle, Line, Path };

    PaintTool( QObject * = nullptr );

    QGraphicsItem *makeItem( int x, int y );

    QPen &getPenRef();
    const QPen &getPen() const;
    void setPen(const QPen &newPen);

    QBrush &getBrushRef();
    const QBrush &getBrush() const;
    void setBrush(const QBrush &newBrush);

    QGraphicsItem *getLast() const;
    void resetLast();

    Shapes getCurrentShape() const;
    void setShape( Shapes );

signals:
    void penChanged();
    void brushChanged();

private:
    QPen pen;
    QBrush brush;

    Shapes lastShape, currentShape;

    QGraphicsItem *last;

    Q_PROPERTY(QPen pen READ getPen WRITE setPen NOTIFY penChanged)
    Q_PROPERTY(QBrush brush READ getBrush WRITE setBrush NOTIFY brushChanged)
};

#endif // PAINTTOOL_H
