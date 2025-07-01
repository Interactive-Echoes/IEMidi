// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMidiOutputEditorWidget.h"

#include "QBoxLayout.h"
#include "QPushButton.h"

#include "IEMidiMessageEditorWidget.h"

IEMidiOutputEditorWidget::IEMidiOutputEditorWidget(const IEMidiDeviceOutputProperty& MidiDeviceOutputProperty, QWidget* Parent) : QWidget(Parent) 
{
    QHBoxLayout* const Layout = new QHBoxLayout(this);
    Layout->setContentsMargins(0, 0, 0, 0);
    Layout->setSpacing(10);

    m_SendButton = new QPushButton(this);
    m_SendButton->setText("Send");
    Layout->addWidget(m_SendButton);

    m_MidiMessageEditor = new IEMidiMessageEditorWidget(MidiDeviceOutputProperty.MidiMessage, this);
    Layout->addWidget(m_MidiMessageEditor);

    if (QPushButton* const DeleteButton = new QPushButton(this))
    {
        DeleteButton->setText("Delete");
        DeleteButton->connect(DeleteButton, &QPushButton::pressed, [this]()
            {
                emit OnDelete();
                this->deleteLater();
            });

        Layout->addStretch(1);
        Layout->addWidget(DeleteButton);
    }
}