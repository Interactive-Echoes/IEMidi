// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEDropdownItemDelegate.h"

#include "qpainter.h"
#include "qpainterpath.h"

IEDropdownItemDelegate::IEDropdownItemDelegate(QObject* Parent) :
    QStyledItemDelegate(Parent)
{
}

void IEDropdownItemDelegate::paint(QPainter* Painter, const QStyleOptionViewItem& Option, const QModelIndex& Index) const
{
    QStyleOptionViewItem IEOption = Option;
    IEOption.displayAlignment = Qt::AlignCenter;

    Painter->setRenderHint(QPainter::Antialiasing, true);

    QRect Rect = IEOption.rect.adjusted(1, 1, -1, -1);
    QPainterPath PainterPath;
    PainterPath.addRoundedRect(Rect, 6, 6);
    Painter->setPen(Qt::NoPen);
    if (IEOption.state & QStyle::State_Selected)
    {
        Painter->fillPath(PainterPath, IEOption.palette.highlight());
    }   
    else
    {
        Painter->fillPath(PainterPath, IEOption.palette.base());
    }
    Painter->drawPath(PainterPath);

    Painter->setPen(QPen(IEOption.palette.color(QPalette::Text)));
    IEOption.state &= ~QStyle::State_Selected;
    QStyledItemDelegate::paint(Painter, IEOption, Index);
}