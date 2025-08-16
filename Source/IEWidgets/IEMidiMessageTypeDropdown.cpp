// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMidiMessageTypeDropdown.h"

#include "qboxlayout.h"
#include "qcombobox.h"

#include "IELog.h"

IEMidiMessageTypeDropdown::IEMidiMessageTypeDropdown(QWidget* Parent) :
    QWidget(Parent)
{
    m_ComboBoxWidget = new QComboBox(this);
    m_ComboBoxWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_ComboBoxWidget->addItem("-Message Type-");
    m_ComboBoxWidget->addItem("NoteOnOff");
    m_ComboBoxWidget->addItem("ControlChange");
    m_ComboBoxWidget->connect(m_ComboBoxWidget, &QComboBox::currentIndexChanged, this, &IEMidiMessageTypeDropdown::OnComboBoxIndexChanged);

    QHBoxLayout* const Layout = new QHBoxLayout(this);
    Layout->setContentsMargins(0, 0, 0, 0);
    Layout->addWidget(m_ComboBoxWidget);
}

void IEMidiMessageTypeDropdown::SetValue(IEMidiMessageType MidiMessageType)
{
    if (m_ComboBoxWidget)
    {
        m_ComboBoxWidget->setCurrentIndex(static_cast<int>(MidiMessageType));
        m_CachedMidiMessageType = MidiMessageType;
    }
}

IEMidiMessageType IEMidiMessageTypeDropdown::GetValue() const
{
    return m_CachedMidiMessageType;
}

void IEMidiMessageTypeDropdown::OnComboBoxIndexChanged(int NewIndex)
{
    const IEMidiMessageType NewMidiMessageType = static_cast<IEMidiMessageType>(NewIndex);
    emit OnMidiMessageTypeChanged(m_CachedMidiMessageType, NewMidiMessageType);
    m_CachedMidiMessageType = NewMidiMessageType;
}