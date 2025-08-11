// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMidiOutputEditorWidget.h"

#include "qboxlayout.h"
#include "qpushbutton.h"

#include "IEMidiMessageEditorWidget.h"

IEMidiOutputEditorWidget::IEMidiOutputEditorWidget(IEMidiDeviceOutputProperty& MidiDeviceOutputProperty, QWidget* Parent) :
    QWidget(Parent),
    m_MidiDeviceOutputProperty(MidiDeviceOutputProperty)
{
    m_SendButtonWidget = new QPushButton(this);
    m_SendButtonWidget->setText("Send");
    m_SendButtonWidget->connect(m_SendButtonWidget, &QPushButton::pressed, this, &IEMidiOutputEditorWidget::OnSendButtonPressed);
    
    m_MidiMessageEditorWidget = new IEMidiMessageEditorWidget(m_MidiDeviceOutputProperty.MidiMessage, this);
    m_MidiMessageEditorWidget->SetValues(m_MidiDeviceOutputProperty.MidiMessage);
    m_MidiMessageEditorWidget->connect(m_MidiMessageEditorWidget, &IEMidiMessageEditorWidget::OnMidiMessageCommitted, this, &IEMidiOutputEditorWidget::OnMidiMessageCommitted);
    
    QPushButton* const DeleteButton = new QPushButton("Delete", this);
    DeleteButton->connect(DeleteButton, &QPushButton::pressed, this, &IEMidiOutputEditorWidget::OnDeleteButtonPressed);
    DeleteButton->setStyleSheet(R"(
        QPushButton
        {
            background-color: rgb(45, 20, 20);
        }

        QPushButton:hover 
        {
            background-color: rgb(60, 35, 35);
        }
    )");

    QHBoxLayout* const Layout = new QHBoxLayout(this);
    Layout->setContentsMargins(0, 0, 0, 0);
    Layout->setSpacing(10);
    Layout->addWidget(m_SendButtonWidget);
    Layout->addWidget(m_MidiMessageEditorWidget);
    Layout->addStretch(1);
    Layout->addWidget(DeleteButton);

}

void IEMidiOutputEditorWidget::OnMidiMessageCommitted() const
{
    if (m_MidiMessageEditorWidget)
    {
        m_MidiDeviceOutputProperty.MidiMessage = m_MidiMessageEditorWidget->GetValues();
    }
}

void IEMidiOutputEditorWidget::OnSendButtonPressed()const
{
    emit OnSendMidiButtonPressed(m_MidiDeviceOutputProperty.MidiMessage);
}

void IEMidiOutputEditorWidget::OnDeleteButtonPressed()
{
    m_MidiDeviceOutputProperty.Delete();
    deleteLater();
}