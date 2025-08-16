// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEDeletePropertyButton.h"

#include "qpainter.h"

IEDeletePropertyButton::IEDeletePropertyButton(QWidget* Parent) :
    QPushButton(Parent)
{
    setCheckable(true);
    setChecked(false);
    setFixedSize(rect().height() - 10, rect().height() - 10);
    setFlat(true);
}

void IEDeletePropertyButton::paintEvent(QPaintEvent* PaintEvent)
{
    static const std::string TrashIconPath = std::string(IEResources_Folder_Path) + "/Icons/Trash-Red.png";
    static QImage TrashIconImage = QImage(TrashIconPath.c_str());

    QPainter TrashIconPainter(&TrashIconImage);
    TrashIconPainter.setRenderHint(QPainter::Antialiasing);
    TrashIconPainter.drawImage(TrashIconImage.rect(), TrashIconImage);
    TrashIconPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);

    const QColor FillColor = underMouse() ? QColor(200, 30, 30) : QColor(100, 30, 30);
    TrashIconPainter.fillRect(TrashIconImage.rect(), FillColor);
    
    QPainter Painter(this);
    Painter.setRenderHint(QPainter::Antialiasing);
    Painter.setBrush(Qt::NoBrush);
    Painter.drawImage(rect(), TrashIconImage);
}