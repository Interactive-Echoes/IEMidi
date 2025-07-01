// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMidiMessageEditorWidget.h"

#include <algorithm>

#include "QBoxLayout.h"
#include "QSpinBox.h"

#include "IELog.h"

IEMidiMessageEditorWidget::IEMidiMessageEditorWidget(const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>& MidiMessage, QWidget* Parent) : QWidget(Parent)
{
    QHBoxLayout* const Layout = new QHBoxLayout(this);
    Layout->setContentsMargins(0, 0, 0, 0);
    Layout->setSpacing(0);

    for (int i = 0; i < MIDI_MESSAGE_BYTE_COUNT; i++)
    {
        m_SpinBoxes[i] = new QSpinBox(this);
        m_SpinBoxes[i]->setRange(0, 255);
        m_SpinBoxes[i]->setValue(MidiMessage[i]);
        Layout->addWidget(m_SpinBoxes[i]);
    }
}

IEMidiMessageEditorWidget::IEMidiMessageEditorWidget(QWidget* Parent) : IEMidiMessageEditorWidget({0, 0, 0}, Parent)
{}

std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT> IEMidiMessageEditorWidget::GetValues() const
{
    return  {   static_cast<uint8_t>(m_SpinBoxes[0]->value()),
                static_cast<uint8_t>(m_SpinBoxes[1]->value()),
                static_cast<uint8_t>(m_SpinBoxes[2]->value()) };
}

void IEMidiMessageEditorWidget::SetValues(const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>& MidiMessage)
{
    for (int i = 0; i < m_SpinBoxes.size(); i++)
    {
        m_SpinBoxes[i]->setValue(MidiMessage[i]);
    }
}
