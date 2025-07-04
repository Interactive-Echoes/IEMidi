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

    QString SpinBoxStyle = R"(
        QSpinBox {
            border: 1px solid #ccc;
            border-radius: 6px;
            padding: 4px 8px;
            background: rgb(10, 10, 10);
            selection-background-color: #0078d7;
            font-size: 14px;
        }
        
        QSpinBox::up-button, QSpinBox::down-button {
            width: 16px;
            border: none;
            background: transparent;
            subcontrol-origin: border;
        }
        
        QSpinBox::up-button {
            subcontrol-position: right top;
            image: url(:/icons/up-arrow.svg); /* Replace with your icon */
            margin: 1px;
        }
        
        QSpinBox::down-button {
            subcontrol-position: right bottom;
            image: url(:/icons/down-arrow.svg); /* Replace with your icon */
            margin: 1px;
        }
        
        QSpinBox::up-arrow, QSpinBox::down-arrow {
            width: 10px;
            height: 10px;
        }
        )";

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
        m_SpinBoxWidgets[i]->connect(m_SpinBoxWidgets[i], &QSpinBox::editingFinished, this, &IEMidiMessageEditorWidget::OnMidiByteCommitted);
        m_SpinBoxWidgets[i]->setStyleSheet(SpinBoxStyle);
        Layout->addWidget(m_SpinBoxWidgets[i]);
    }
}

std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT> IEMidiMessageEditorWidget::GetValues() const
{
    return  {   static_cast<uint8_t>(m_SpinBoxWidgets[0]->value()),
                static_cast<uint8_t>(m_SpinBoxWidgets[1]->value()),
                static_cast<uint8_t>(m_SpinBoxWidgets[2]->value()) };
}

void IEMidiMessageEditorWidget::SetValues(const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>& MidiMessage) const
{
    for (int i = 0; i < m_SpinBoxWidgets.size(); i++)
    {
        m_SpinBoxWidgets[i]->setValue(MidiMessage[i]);
    }
}

void IEMidiMessageEditorWidget::ShowByteWidget(size_t Index) const
{
    IEAssert(Index >= 0 && Index < MIDI_MESSAGE_BYTE_COUNT);
    if (m_SpinBoxWidgets[Index])
    {
        m_SpinBoxWidgets[Index]->show();
    }
}

void IEMidiMessageEditorWidget::HideByteWidget(size_t Index) const
{
    IEAssert(Index >= 0 && Index < MIDI_MESSAGE_BYTE_COUNT);
    if (m_SpinBoxWidgets[Index])
    {
        m_SpinBoxWidgets[Index]->hide();
    }
}

void IEMidiMessageEditorWidget::OnMidiByteCommitted() const
{
    emit OnMidiMessageCommitted();
}