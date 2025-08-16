// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IERecordButton.h"

#include "qpainter.h"

IERecordButton::IERecordButton(QWidget* Parent) :
    QPushButton(Parent)
{
    setCheckable(true);
    setChecked(false);
    setFixedSize(rect().height() - 6, rect().height() - 6);
    setFlat(true);
}

void IERecordButton::paintEvent(QPaintEvent* PaintEvent)
{
    QPainter Painter(this);
    Painter.setRenderHint(QPainter::Antialiasing);

    Painter.setBrush(Qt::NoBrush);
    Painter.setPen(QPen(QColor(120, 120, 120), 2));
    Painter.drawEllipse(rect().adjusted(2, 2, -2, -2));

    const QColor FillColor = isChecked() ? QColor(255, 30, 30) : underMouse() ? QColor(200, 30, 30) : QColor(100, 30, 30);
    Painter.setBrush(FillColor);
    Painter.setPen(Qt::NoPen);
    Painter.drawEllipse(rect().adjusted(7, 7, -7, -7));
}