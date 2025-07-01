// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMidiInputEditorWidget.h"

#include "QBoxLayout.h"
#include "QCheckBox.h"
#include "QPushButton.h"

#include "IEActionTypeDropdownWidget.h"
#include "IEMidiMessageEditorWidget.h"
#include "IEMidiTypeDropdownWidget.h"

IEMidiInputEditorWidget::IEMidiInputEditorWidget(const IEMidiDeviceInputProperty& MidiDeviceInputProperty, QWidget* Parent) : QWidget(Parent) 
{
    QHBoxLayout* const Layout = new QHBoxLayout(this);
    Layout->setContentsMargins(0, 0, 0, 0);
    Layout->setSpacing(10);

    m_MidiTypeDropdown = new IEMidiTypeDropdownWidget(this);
    Layout->addWidget(m_MidiTypeDropdown);

    m_ActionTypeDropdown = new IEActionTypeDropdownWidget(this);
    Layout->addWidget(m_ActionTypeDropdown);

    m_MidiToggleCheckbox = new QCheckBox("Toggle", this);
    Layout->addWidget(m_MidiToggleCheckbox);

    m_RecordButton = new QPushButton(this);
    m_RecordButton->setText("Record");
    Layout->addWidget(m_RecordButton);

    m_MidiMessageEditor = new IEMidiMessageEditorWidget(MidiDeviceInputProperty.MidiMessage, this);
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