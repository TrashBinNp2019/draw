#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include <QDomDocument>
#include <QFile>
#include <QGraphicsScene>

#include "painttool.h"

typedef std::pair< QGraphicsItem *, QString > NamedGraphicsItem;


class DocumentManager : public QObject
{
    Q_OBJECT
public:
    explicit DocumentManager(QObject *parent = nullptr);

    QGraphicsScene &getScene();
    PaintTool &getPaintTool();

    void setSelectedItem( const QString & );
    void saveSelectedTranslation();
    void saveSelectedRotation();
    void saveSelectedScale();
    QGraphicsItem *getSelectedItem() const;

    void addItem( qreal, qreal );
    void finishAdding();

    void deleteItem( const QString & );
    void scaleSelectedItem( float );
    void rotateSelectedItem( float );
    void translateSelectedItem( const QPointF & );

    const QGraphicsItem *getLast() const;
    void redrawLast(const QPointF &pivot, const QPointF &to );

    void init();

    QColor getPenColor() const;
    QColor getBrushColor() const;

    void setPenColor( QColor );
    void setPenWidth( int );
    void setBrushColor( QColor );

    void newScene( const QRect &, const QColor & );
    void closeScene( );
    void read( const QString & );
    void write( const QString & );


    const std::list<NamedGraphicsItem> &getNamedItemsList() const;

public slots:
    void cancelEditing();

signals:
    void itemsListChanged();

private:
    PaintTool tool;
    QGraphicsScene scene;

    qreal savedRotation;
    qreal savedScale;
    QPointF savedTranslation;

    QGraphicsItem *selected;

    std::list< NamedGraphicsItem > namedItems;

    void addNamedItem( const QGraphicsItem *, const QString & );
    void parseSVG( const QDomDocument & );
    void parseStrokeAndFill(const QDomElement &, QAbstractGraphicsShapeItem * ) const;
    void parseStrokeAndFill(const QDomElement &, QGraphicsLineItem * ) const;
};

#endif // DOCUMENTMANAGER_H
