// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMidiDeviceOutputPropertyEditor.h"

#include "qboxlayout.h"
#include "qpushbutton.h"

#include "IEDeletePropertyButton.h"
#include "IEMidiMessageEditor.h"

IEMidiDeviceOutputPropertyEditor::IEMidiDeviceOutputPropertyEditor(IEMidiDeviceOutputProperty& MidiDeviceOutputProperty, QWidget* Parent) :
    QWidget(Parent),
    m_MidiDeviceOutputProperty(MidiDeviceOutputProperty)
{
    m_SendButtonWidget = new QPushButton(this);
    m_SendButtonWidget->setText("Send");
    m_SendButtonWidget->connect(m_SendButtonWidget, &QPushButton::pressed, this, &IEMidiDeviceOutputPropertyEditor::OnSendButtonPressed);
    
    m_MidiMessageEditorWidget = new IEMidiMessageEditor(m_MidiDeviceOutputProperty.MidiMessage, this);
    m_MidiMessageEditorWidget->SetValues(m_MidiDeviceOutputProperty.MidiMessage);
    m_MidiMessageEditorWidget->connect(m_MidiMessageEditorWidget, &IEMidiMessageEditor::OnMidiMessageCommitted, this, &IEMidiDeviceOutputPropertyEditor::OnMidiMessageCommitted);
    
    IEDeletePropertyButton* const DeleteButton = new IEDeletePropertyButton(this);
    DeleteButton->connect(DeleteButton, &QPushButton::pressed, this, &IEMidiDeviceOutputPropertyEditor::OnDeleteButtonPressed);

    QHBoxLayout* const Layout = new QHBoxLayout(this);
    Layout->setContentsMargins(0, 0, 0, 0);
    Layout->setSpacing(10);
    Layout->addWidget(m_SendButtonWidget);
    Layout->addWidget(m_MidiMessageEditorWidget);
    Layout->addStretch(1);
    Layout->addWidget(DeleteButton);

}

void IEMidiDeviceOutputPropertyEditor::OnMidiMessageCommitted() const
{
    if (m_MidiMessageEditorWidget)
    {
        m_MidiDeviceOutputProperty.MidiMessage = m_MidiMessageEditorWidget->GetValues();
    }
}

void IEMidiDeviceOutputPropertyEditor::OnSendButtonPressed()const
{
    emit OnSendMidiButtonPressed(m_MidiDeviceOutputProperty.MidiMessage);
}

void IEMidiDeviceOutputPropertyEditor::OnDeleteButtonPressed()
{
    m_MidiDeviceOutputProperty.Delete();
    deleteLater();
}