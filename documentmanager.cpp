#include "documentmanager.h"

#include <QDomDocument>
#include <QSvgGenerator>
#include <QStyle>


DocumentManager::DocumentManager(QObject *parent)
    : QObject{parent}
    , tool()
    , scene()
{
}

QGraphicsScene &DocumentManager::getScene()
{
    return scene;
}

void DocumentManager::addItem( qreal x, qreal y )
{
    scene.addItem( tool.makeItem( x, y ) );
}

void DocumentManager::deleteItem( const QString &name )
{
    for ( auto i = namedItems.begin(); i != namedItems.end(); i++ ) {
        if ( i->second == name ) {
            auto rect = i->first->boundingRect();
            delete i->first;

            scene.update( rect );
            namedItems.erase( i );
            emit itemsListChanged();
            break;
        }
    }
}

void DocumentManager::scaleSelectedItem( float scale )
{
    selected->setScale( savedScale * scale );
}

void DocumentManager::rotateSelectedItem( float angle )
{
    selected->setRotation( savedRotation + angle );
}

void DocumentManager::translateSelectedItem( const QPointF &vector )
{
    auto [ x, y ] = vector - savedTranslation;
    savedTranslation = vector;

    selected->moveBy( x, y );
}

void DocumentManager::finishAdding()
{
    tool.getLast()->setTransformOriginPoint( tool.getLast()->boundingRect().center() );

    switch ( tool.getCurrentShape() ) {
    case PaintTool::Rect: {
        addNamedItem( tool.getLast(), "Rectangle" );
        break;
    }
    case PaintTool::Circle: {
        addNamedItem( tool.getLast(), "Circle" );
        break;
    }
    case PaintTool::Line: {
        addNamedItem( tool.getLast(), "Line" );
        break;
    }
    case PaintTool::Path: {
        addNamedItem( tool.getLast(), "Path" );
        auto ptr = dynamic_cast< QGraphicsPathItem *>( tool.getLast() );
        auto path1 = ptr->path();
        QPainterPath path2{};

        path2.moveTo(  path1.elementAt( 0 ) );
        int i;
        for ( i = 1; path1.elementCount() - i > 6; i += 6 ) {
            path2.cubicTo( path1.elementAt( i ), path1.elementAt( i + 2 ), path1.elementAt( i + 4 ) );
        }

        switch ( path1.elementCount() - i ) {
        case 6: {
            path2.cubicTo( path1.elementAt( i + 1 ), path1.elementAt( i + 3 ), path1.elementAt( i + 5 ) );
            break;
        }
        case 5: {
            path2.quadTo( path1.elementAt( i + 1 ), path1.elementAt( i + 2 ) );
            path2.quadTo( path1.elementAt( i + 3 ), path1.elementAt( i + 4 ) );
            break;
        }
        case 4: {
            path2.cubicTo( path1.elementAt( i + 1 ), path1.elementAt( i + 2 ), path1.elementAt( i + 3 ) );
            break;
        }
        case 3: {
            path2.quadTo( path1.elementAt( i + 1 ), path1.elementAt( i + 2 ) );
            break;
        }
        default:
            break;
        }

        ptr->setPath( path2 );

        break;
    }
    default: {
        break;
    }
    }

//    tool.getLast()->setFlag( QGraphicsItem::ItemIsMovable, true );
}

const QGraphicsItem *DocumentManager::getLast() const
{
    return tool.getLast();
}

void DocumentManager::redrawLast( const QPointF &pivot, const QPointF &to )
{
    auto [ x1, y1 ] = pivot;
    auto [ x2, y2 ] = to;

    if ( x1 > x2 ) {
        auto buf = x1;
        x1 = x2;
        x2 = buf;
    }
    if ( y1 > y2 ) {
        auto buf = y1;
        y1 = y2;
        y2 = buf;
    }

    switch ( tool.getCurrentShape() ) {
    case PaintTool::Rect: {
        auto ptr = dynamic_cast< QGraphicsRectItem *>( tool.getLast() );
        ptr->setRect( { static_cast<qreal>(x1), static_cast<qreal>(y1), x2 - x1, y2 - y1 } );
        break;
    }
    case PaintTool::Circle: {
        auto ptr = dynamic_cast< QGraphicsEllipseItem *>( tool.getLast() );
        ptr->setRect( { static_cast<qreal>(x1), static_cast<qreal>(y1), x2 - x1, y2 - y1 } );
        break;
    }
    case PaintTool::Line: {
        auto ptr = dynamic_cast< QGraphicsLineItem *>( tool.getLast() );
        ptr->setLine( { pivot, to } );
        break;
    }
    case PaintTool::Path: {
        auto ptr = dynamic_cast< QGraphicsPathItem *>( tool.getLast() );
        auto path = ptr->path();
        path.lineTo( to );
        ptr->setPath( path );
        break;
    }
    default: {
        break;
    }
    }

    scene.update(  { static_cast<qreal>(x1) - 5, static_cast<qreal>(y1) - 5, x2 - x1 + 10, y2 - y1 + 10 } );
}

void DocumentManager::init()
{
    tool.setShape( PaintTool::Rect );
    tool.setBrush( { Qt::transparent } );
    tool.setPen( { Qt::black } );
}

QColor DocumentManager::getPenColor() const
{
    return tool.getPen().color();
}

QColor DocumentManager::getBrushColor() const
{
    return tool.getBrush().color();
}

void DocumentManager::setPenColor( QColor col )
{
    auto pen = tool.getPen();
    pen.setColor( col );
    tool.setPen( pen );
}

void DocumentManager::setPenWidth( int width )
{
    auto pen = tool.getPen();
    pen.setWidth( width );
    tool.setPen( pen );
}

void DocumentManager::setBrushColor( QColor col )
{
    auto brush = tool.getBrush();
    brush.setColor( col );
    tool.setBrush( brush );
}

void DocumentManager::newScene( const QRect &rect, const QColor &color )
{
    closeScene();

    scene.setSceneRect( rect );
    scene.setBackgroundBrush( { color } );
}

void DocumentManager::closeScene()
{
    scene.clear();
    tool.resetLast();
    namedItems.clear();
}

void DocumentManager::read( const QString &name )
{
    QDomDocument doc;
    QFile file( name );

    if ( !file.open(QIODevice::ReadOnly) || !doc.setContent( &file ) ) return;

    getSceneFromSvg( doc );
}

void DocumentManager::write( const QString &name )
{
    QSvgGenerator gen;
    gen.setFileName( name );
    gen.setSize( scene.sceneRect().size().toSize() );
    gen.setViewBox( scene.sceneRect() );
    gen.setTitle( "Hello svg :)" );

    QPainter painter;
    painter.begin( &gen );
    painter.setClipRect( scene.sceneRect() );

    scene.render( &painter );

    painter.end();
}

void DocumentManager::cancelEditing()
{
    scene.removeItem( tool.getLast() );
    tool.resetLast();
}

QGraphicsItem *DocumentManager::getSelectedItem() const
{
    return selected;
}

void DocumentManager::addNamedItem( QGraphicsItem *item, const QString &name )
{
    auto final_name = name;

    while ( true ) {
        bool changed = false;

        for (auto const &element: namedItems) {
          if (element.second == final_name) {
            // check if index was already appended
            bool test;
                auto ind = final_name.split(" ").last().toInt(&test);
            if (test) {
              final_name = final_name.mid(0, final_name.lastIndexOf(" ")) + ' ' +
                           QString::number(ind + 1);
            } else {
              final_name += " 1";
            }
            changed = true;
            break;
          }
        }

        if ( !changed ) {
            break;
        }
    }

    namedItems.push_back( std::make_pair( item, final_name ) );
    emit itemsListChanged();
}

void DocumentManager::getSceneFromSvg( const QDomDocument &doc )
{
    auto header = doc.firstChildElement( "svg" );
    auto rect = header.attribute( "viewBox" ).split( " " );

    if ( header.isNull() ) return;

    closeScene();
    scene.setSceneRect( {
                            rect.at( 0 ).toDouble(),
                            rect.at( 1 ).toDouble(),
                            rect.at( 2 ).toDouble(),
                            rect.at( 3 ).toDouble()
                        } );

    QDomNodeList list = doc.elementsByTagName( "g" );
    for ( int i = 0; i < list.count(); i++ ) {
        auto element = list.item( i ).firstChildElement( "rect" );
        if ( !element.isNull() ) {
            auto rect = new QGraphicsRectItem();
            rect->setRect(
                            element.attribute("x").toInt(),
                            element.attribute("y").toInt(),
                            element.attribute("width").toInt(),
                            element.attribute("height").toInt()
                         );

            auto wrap = list.item( i ).toElement();
            parseStrokeAndFill( wrap, rect );

            scene.addItem( rect );
            addNamedItem( rect, "Rectangle" );
            continue;
        }

        element = list.item( i ).firstChildElement( "ellipse" );
        if ( !element.isNull() ) {
            auto ellipse = new QGraphicsEllipseItem	(
                            element.attribute("cx").toDouble() - element.attribute("rx").toDouble(),
                            element.attribute("cy").toDouble() - element.attribute("ry").toDouble(),
                            element.attribute("rx").toDouble() * 2,
                            element.attribute("ry").toDouble() * 2
                                                    );

            auto wrap = list.item( i ).toElement();
            parseStrokeAndFill( wrap, ellipse );

            scene.addItem( ellipse );
            addNamedItem( ellipse, "Ellipse" );
            continue;
        }

        element = list.item( i ).firstChildElement( "polyline" );
        if ( !element.isNull() ) {
            auto line = new QGraphicsLineItem();

            auto points = element.attribute( "points" ).split( " " );
            QPoint from	{
                            points.at( 0 ).split( "," ).at( 0 ).toInt(),
                            points.at( 0 ).split( "," ).at( 1 ).toInt(),
                        };
            QPoint to	{
                            points.at( 1 ).split( "," ).at( 0 ).toInt(),
                            points.at( 1 ).split( "," ).at( 1 ).toInt(),
                        };
            line->setLine( { from, to } );

            auto wrap = list.item( i ).toElement();
            parseStrokeAndFill( wrap, line );

            scene.addItem( line );
            addNamedItem( line, "Line" );
            continue;
        }

        element = list.item( i ).firstChildElement( "path" );
        if ( !element.isNull() ) {
            auto pathItem = new QGraphicsPathItem();
            QPainterPath path;
            auto points = element.attribute( "d" ).split( " " );

            for ( int j = 0; j < points.count() - 2; j++ ) {
                if ( points[ j ][ 0 ] == 'M' ) {
                    auto pointStr = points[ j ].split( "," );
                    pointStr[ 0 ].remove( 0, 1 );

                    path.moveTo( {
                                     pointStr[ 0 ].toDouble(),
                                     pointStr[ 1 ].toDouble()
                                 } );
                }
                if ( points[ j ][ 0 ] == 'C' ) {
                    auto ctrlPointStr1 = points[ j ].split( "," );
                    ctrlPointStr1[ 0 ].remove( 0, 1 );
                    auto ctrlPointStr2 = points[ ++j ].split( "," );
                    auto destPointStr = points[ ++j ].split( "," );

                    QPointF ctrlPoint1{
                                        ctrlPointStr1[ 0 ].toDouble(),
                                        ctrlPointStr1[ 1 ].toDouble()
                                      };
                    QPointF ctrlPoint2{
                                        ctrlPointStr2[ 0 ].toDouble(),
                                        ctrlPointStr2[ 1 ].toDouble()
                                      };
                    QPointF dstPoint  {
                                        destPointStr[ 0 ].toDouble(),
                                        destPointStr[ 1 ].toDouble()
                                      };


                    path.cubicTo( ctrlPoint1, ctrlPoint2, dstPoint );
                }
            }
            pathItem->setPath( path );

            auto wrap = list.item( i ).toElement();
            parseStrokeAndFill( wrap, pathItem );

            scene.addItem( pathItem );
            addNamedItem( pathItem, "Path" );
            continue;
        }
    }
}

const std::list<NamedGraphicsItem> &DocumentManager::getNamedItemsList() const
{
    return namedItems;
}

void DocumentManager::parseStrokeAndFill( const QDomElement &wrap, QAbstractGraphicsShapeItem *item ) const
{
            if ( wrap.attribute( "fill" ) != "none" ) {
                QColor color{ wrap.attribute( "fill", "#ffffff" ) };
                color.setAlphaF( wrap.attribute( "fill-opacity" ).toFloat() );

                QBrush brush{ color };
                item->setBrush( brush );

            } else {
                item->setBrush( { Qt::transparent } );
            }

            if ( wrap.attribute( "stroke" ) != "none" ) {
                QColor color{ wrap.attribute( "stroke", "#ffffff" ) };
                color.setAlphaF( wrap.attribute( "stroke-opacity", "1" ).toFloat() );

                QPen pen{ color };
                pen.setWidth( wrap.attribute( "stroke-width", "1" ).toInt() );
                item->setPen( pen );
            } else {
                item->setPen( { Qt::transparent } );
            }
}

void DocumentManager::parseStrokeAndFill( const QDomElement &wrap, QGraphicsLineItem *item ) const
{
            if ( wrap.attribute( "stroke" ) != "none" ) {
                QColor color{ wrap.attribute( "stroke", "#ffffff" ) };
                color.setAlphaF( wrap.attribute( "stroke-opacity", "1" ).toFloat() );

                QPen pen{ color };
                pen.setWidth( wrap.attribute( "stroke-width", "1" ).toInt() );
                item->setPen( pen );
            } else {
                item->setPen( { Qt::transparent } );
            }
}

PaintTool &DocumentManager::getPaintTool()
{
    return tool;
}

void DocumentManager::setSelectedItem( const QString &name )
{
    for ( auto i = namedItems.begin(); i != namedItems.end(); i++ ) {
        if ( i->second == name ) {
            selected = i->first;
        }
    }
}

void DocumentManager::saveSelectedTranslation()
{
    savedTranslation = { 0, 0 };
}

void DocumentManager::saveSelectedRotation()
{
    savedRotation = selected->rotation();
}

void DocumentManager::saveSelectedScale()
{
    savedScale = selected->scale();
}
