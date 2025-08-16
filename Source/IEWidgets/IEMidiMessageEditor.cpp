// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMidiMessageEditor.h"

#include <algorithm>

#include "qboxlayout.h"
#include "qspinbox.h"

#include "IELog.h"

IEMidiMessageEditor::IEMidiMessageEditor(const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>& MidiMessage, QWidget* Parent) :
    QWidget(Parent)
{
    QString SpinBoxStyle = R"(
        QSpinBox {
            border-radius: 6px;
            padding: 4px 8px;
            background: rgb(33, 33, 33);
            selection-background-color: #0078d7;
            font-size: 14px;
        })";

    QHBoxLayout* const Layout = new QHBoxLayout(this);
    Layout->setContentsMargins(0, 0, 0, 0);
    Layout->setSpacing(5);
    
    for (int i = 0; i < MIDI_MESSAGE_BYTE_COUNT; i++)
    {
        m_SpinBoxWidgets[i] = new QSpinBox(this);
        if (i == 0)
        {
            m_SpinBoxWidgets[i]->setRange(0, 255);
        }
        else
        {
            m_SpinBoxWidgets[i]->setRange(0, 127);
        }
        m_SpinBoxWidgets[i]->setValue(MidiMessage[i]);
        m_SpinBoxWidgets[i]->connect(m_SpinBoxWidgets[i], &QSpinBox::editingFinished, this, &IEMidiMessageEditor::OnMidiByteCommitted);
        m_SpinBoxWidgets[i]->setStyleSheet(SpinBoxStyle);
        m_SpinBoxWidgets[i]->setButtonSymbols(QAbstractSpinBox::NoButtons);
        Layout->addWidget(m_SpinBoxWidgets[i]);
    }
}

std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT> IEMidiMessageEditor::GetValues() const
{
    return  {   static_cast<uint8_t>(m_SpinBoxWidgets[0]->value()),
                static_cast<uint8_t>(m_SpinBoxWidgets[1]->value()),
                static_cast<uint8_t>(m_SpinBoxWidgets[2]->value()) };
}

void IEMidiMessageEditor::SetValues(const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>& MidiMessage) const
{
    for (int i = 0; i < m_SpinBoxWidgets.size(); i++)
    {
        m_SpinBoxWidgets[i]->setValue(MidiMessage[i]);
    }
}

void IEMidiMessageEditor::ShowByteWidget(size_t Index) const
{
    IEAssert(Index >= 0 && Index < MIDI_MESSAGE_BYTE_COUNT);
    if (m_SpinBoxWidgets[Index])
    {
        m_SpinBoxWidgets[Index]->show();
    }
}

void IEMidiMessageEditor::HideByteWidget(size_t Index) const
{
    IEAssert(Index >= 0 && Index < MIDI_MESSAGE_BYTE_COUNT);
    if (m_SpinBoxWidgets[Index])
    {
        m_SpinBoxWidgets[Index]->hide();
    }
}

void IEMidiMessageEditor::OnMidiByteCommitted() const
{
    emit OnMidiMessageCommitted();
}