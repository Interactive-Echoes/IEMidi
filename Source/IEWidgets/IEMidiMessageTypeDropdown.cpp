// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMidiMessageTypeDropdown.h"

#include "qboxlayout.h"
#include "qpainter.h"

#include "IELog.h"

#include "IEDropdownItemDelegate.h"

IEMidiMessageTypeDropdown::IEMidiMessageTypeDropdown(QWidget* Parent) :
    QComboBox(Parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedWidth(160);
    connect(this, &QComboBox::currentIndexChanged, this, &IEMidiMessageTypeDropdown::OnComboBoxIndexChanged);
    setItemDelegate(new IEDropdownItemDelegate(this));

    addItem("-Message Type-");
    addItem("NoteOnOff");
    addItem("ControlChange");
    
    QHBoxLayout* const Layout = new QHBoxLayout(this);
}

void IEMidiMessageTypeDropdown::SetValue(IEMidiMessageType MidiMessageType)
{
    setCurrentIndex(static_cast<int>(MidiMessageType));
}

IEMidiMessageType IEMidiMessageTypeDropdown::GetValue() const
{
    return m_CachedMidiMessageType;
}

void IEMidiMessageTypeDropdown::paintEvent(QPaintEvent* PaintEvent)
{
    static const std::string ArrowIconPath = std::string(IEResources_Folder_Path) + "/Icons/Down-Arrow.png";
    static const QPixmap ArrowPixmap(ArrowIconPath.c_str());

    QPainter Painter(this);

    QStyleOptionComboBox StyleOption;
    initStyleOption(&StyleOption);

    style()->drawPrimitive(QStyle::PE_PanelButtonCommand, &StyleOption, &Painter, this);
    style()->drawControl(QStyle::CE_ComboBoxLabel, &StyleOption, &Painter, this);
    
    if (!ArrowPixmap.isNull())
    {
        static const int ArrowSize = 12;
        QRect ArrowRect(width() - ArrowSize - 6, (height() - ArrowSize) / 2, ArrowSize, ArrowSize);
        Painter.fillRect(ArrowRect.adjusted(-2, -2, 2, 2), palette().button());
        Painter.drawPixmap(ArrowRect, ArrowPixmap);
        style()->drawItemPixmap(&Painter, ArrowRect, 0, ArrowPixmap);
    }
}

void IEMidiMessageTypeDropdown::OnComboBoxIndexChanged(int NewIndex)
{
    const IEMidiMessageType NewMidiMessageType = static_cast<IEMidiMessageType>(NewIndex);
    emit OnMidiMessageTypeChanged(m_CachedMidiMessageType, NewMidiMessageType);
    m_CachedMidiMessageType = NewMidiMessageType;
}