// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMidiActionTypeDropdown.h"

#include "qboxlayout.h"
#include "qpainter.h"
#include "qstyleditemdelegate.h"
#include "qabstractitemview.h"

#include "IEDropdownItemDelegate.h"

IEMidiActionTypeDropdown::IEMidiActionTypeDropdown(QWidget* Parent) :
    QComboBox(Parent)
{
    connect(this, &QComboBox::currentIndexChanged, this, &IEMidiActionTypeDropdown::OnComboBoxIndexChanged);
    setItemDelegate(new IEDropdownItemDelegate(this));

    addItem("-Action Type-");
    addItem("Volume");
    addItem("Mute");
    addItem("ConsoleCommand");
    addItem("OpenFile");
}

void IEMidiActionTypeDropdown::SetValue(IEMidiActionType MidiActionType)
{
    setCurrentIndex(static_cast<int>(MidiActionType));
    m_CachedMidiActionType = MidiActionType;
}

IEMidiActionType IEMidiActionTypeDropdown::GetValue() const
{
    return m_CachedMidiActionType;
}

void IEMidiActionTypeDropdown::paintEvent(QPaintEvent* PaintEvent)
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

void IEMidiActionTypeDropdown::OnComboBoxIndexChanged(int NewIndex)
{
    const IEMidiActionType NewIEMidiActionType = static_cast<IEMidiActionType>(NewIndex);
    emit OnMidiActionTypeChanged(m_CachedMidiActionType, NewIEMidiActionType);
    m_CachedMidiActionType = NewIEMidiActionType;
}