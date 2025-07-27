// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMidiActionTypeDropdownWidget.h"

#include "qboxlayout.h"
#include "qcombobox.h"

IEMidiActionTypeDropdownWidget::IEMidiActionTypeDropdownWidget(QWidget* Parent) :
    QWidget(Parent)
{
    m_ComboBoxWidget = new QComboBox(this);
    m_ComboBoxWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_ComboBoxWidget->addItem("-Action Type-");
    m_ComboBoxWidget->addItem("Volume");
    m_ComboBoxWidget->addItem("Mute");
    m_ComboBoxWidget->addItem("ConsoleCommand");
    m_ComboBoxWidget->addItem("OpenFile");
    m_ComboBoxWidget->connect(m_ComboBoxWidget, &QComboBox::currentIndexChanged, this, &IEMidiActionTypeDropdownWidget::OnComboBoxIndexChanged);

    QHBoxLayout* const Layout = new QHBoxLayout(this);
    Layout->setContentsMargins(0, 0, 0, 0);
    Layout->addWidget(m_ComboBoxWidget);
}

void IEMidiActionTypeDropdownWidget::SetValue(IEMidiActionType MidiActionType)
{
    if (m_ComboBoxWidget)
    {
        m_ComboBoxWidget->setCurrentIndex(static_cast<int>(MidiActionType));
        m_CachedMidiActionType = MidiActionType;
    }
}

IEMidiActionType IEMidiActionTypeDropdownWidget::GetValue() const
{
    return m_CachedMidiActionType;
}

void IEMidiActionTypeDropdownWidget::OnComboBoxIndexChanged(int NewIndex)
{
    const IEMidiActionType NewIEMidiActionType = static_cast<IEMidiActionType>(NewIndex);
    emit OnMidiActionTypeChanged(m_CachedMidiActionType, NewIEMidiActionType);
    m_CachedMidiActionType = NewIEMidiActionType;
}