/*
 * Copyright 2013 KanMemo Project.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "clipcursor.h"
#include <QPainter>
#include <QCursor>

ClipCursor::ClipCursor(QQuickPaintedItem *parent) :
    QQuickPaintedItem(parent)
  , m_move(true)
{
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptHoverEvents(true);
}

const QRect &ClipCursor::cursor() const
{
    return m_cursor;
}

void ClipCursor::setCursor(const QRect &rect)
{
    if(m_cursor == rect)
        return;

    m_cursor = rect;

    //範囲外
    qreal width = m_cursor.width();
    qreal height = m_cursor.height();
    if(availableRange().isValid()){
        if(m_cursor.left() < availableRange().left()){
            m_cursor.setX(availableRange().x());
            m_cursor.setWidth(width);
        }
        if(m_cursor.top() < availableRange().top()){
            m_cursor.setY(availableRange().y());
            m_cursor.setHeight(height);
        }

        if(m_cursor.right() > availableRange().right()){
            m_cursor.setX(availableRange().right() - m_cursor.width() + 1);
            m_cursor.setWidth(width);
        }
        if(m_cursor.bottom() > availableRange().bottom()){
            m_cursor.setY(availableRange().bottom() - m_cursor.height() + 1);
            m_cursor.setHeight(height);
        }
    }else{
        if(m_cursor.left() < boundingRect().left()){
            m_cursor.setX(boundingRect().x());
            m_cursor.setWidth(width);
        }
        if(m_cursor.top() < boundingRect().top()){
            m_cursor.setY(boundingRect().y());
            m_cursor.setHeight(height);
        }

        if(m_cursor.right() > boundingRect().right()){
            m_cursor.setX(boundingRect().right() - m_cursor.width() + 1);
            m_cursor.setWidth(width);
        }
        if(m_cursor.bottom() > boundingRect().bottom()){
            m_cursor.setY(boundingRect().bottom() - m_cursor.height() + 1);
            m_cursor.setHeight(height);
        }
    }

    emit cursorChanged();
    update();
}

const QRect &ClipCursor::availableRange() const
{
    return m_availableRange;
}

void ClipCursor::setAvailableRange(const QRect &rect)
{
    if(m_availableRange == rect)
        return;

    m_availableRange = rect;
    emit availableRangeChanged();
}

void ClipCursor::paint(QPainter *painter)
{
    if(!cursor().isValid() || !availableRange().isValid())
        return;

    qreal width = boundingRect().width();
    qreal height = boundingRect().height();

    painter->setRenderHint(QPainter::Antialiasing, antialiasing());

    //余白
    qreal c_left = cursor().left();
    qreal c_right = cursor().right();
    qreal c_top = cursor().top();
    qreal c_bottom = cursor().bottom();
    qreal c_width = cursor().width();
    painter->setPen(QPen(QColor(0, 0, 0, 0), 0));
    painter->setBrush(QBrush(QColor(0, 0, 0, 125)));
    painter->drawRect(QRect(0, 0, c_left, height));                             //左
    painter->drawRect(QRect(c_left, 0, c_width, c_top));                        //上
    painter->drawRect(QRect(c_right+1, 0, width - c_right-2, height));          //右
    painter->drawRect(QRect(c_left, c_bottom+1, c_width, height - c_bottom-2)); //下

    //カーソル
    painter->setPen(QPen(QColor(0, 200, 200, 255), 1));
    painter->setBrush(QBrush(QColor(0, 0, 0, 0)));
    painter->drawRect(cursor());
}

void ClipCursor::mousePressEvent(QMouseEvent *event)
{
    m_prevPoint.setX(event->x());
    m_prevPoint.setY(event->y());
}

void ClipCursor::mouseReleaseEvent(QMouseEvent *event)
{
    if(m_move){
        setCursor(QRect(cursor().x() + (event->x() - m_prevPoint.x())
                        , cursor().y() + (event->y() - m_prevPoint.y())
                        , cursor().width()
                        , cursor().height()));
    }
}

void ClipCursor::mouseMoveEvent(QMouseEvent *event)
{
    if(m_move){
        setCursor(QRect(cursor().x() + (event->x() - m_prevPoint.x())
                        , cursor().y() + (event->y() - m_prevPoint.y())
                        , cursor().width()
                        , cursor().height()));
    }
    m_prevPoint.setX(event->x());
    m_prevPoint.setY(event->y());
}

void ClipCursor::hoverEnterEvent(QHoverEvent *event)
{
    QQuickPaintedItem::setCursor(Qt::OpenHandCursor);
}

void ClipCursor::hoverLeaveEvent(QHoverEvent *event)
{
    QQuickPaintedItem::setCursor(Qt::ArrowCursor);
}

void ClipCursor::hoverMoveEvent(QHoverEvent *event)
{
    qreal x = event->pos().x();
    qreal y = event->pos().y();

    qreal c_top = cursor().top();
    qreal c_bottom = cursor().bottom();
    qreal c_left = cursor().left();
    qreal c_right = cursor().right();

//    qDebug() << "hover x,y=" << x << "," << y;

    if((x > (c_left - 10) && x < (c_left + 10)) && (y > c_top && y < c_bottom)){
        //左
        QQuickPaintedItem::setCursor(Qt::SizeHorCursor);
        m_move = false;
    }else if((x > (c_right - 10) && x < (c_right + 10)) && (y > c_top && y < c_bottom)){
        //右
        QQuickPaintedItem::setCursor(Qt::SizeHorCursor);
        m_move = false;
    }else if((x > c_left && x < c_right) && (y > (c_top - 10) && y < (c_top + 10))){
        //上
        QQuickPaintedItem::setCursor(Qt::SizeVerCursor);
        m_move = false;
    }else if((x > c_left && x < c_right) && (y > (c_bottom - 10) && y < (c_bottom + 10))){
        //下
        QQuickPaintedItem::setCursor(Qt::SizeVerCursor);
        m_move = false;
    }else if((x > c_left && x < c_right) && (y > c_top && y < c_bottom)){
        //範囲内
        QQuickPaintedItem::setCursor(Qt::OpenHandCursor);
        m_move = true;
    }else{
        //範囲外
        QQuickPaintedItem::setCursor(Qt::ArrowCursor);
        m_move = false;
    }
}

