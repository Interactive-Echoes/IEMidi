// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMidiOutputEditorWidget.h"

#include "QBoxLayout.h"
#include "QPushButton.h"

#include "IEMidiMessageEditorWidget.h"

IEMidiOutputEditorWidget::IEMidiOutputEditorWidget(IEMidiDeviceOutputProperty& MidiDeviceOutputProperty, QWidget* Parent) :
    QWidget(Parent),
    m_MidiDeviceOutputProperty(MidiDeviceOutputProperty)
{
    QHBoxLayout* const Layout = new QHBoxLayout(this);
    Layout->setContentsMargins(0, 0, 0, 0);
    Layout->setSpacing(10);

    m_SendButtonWidget = new QPushButton(this);
    m_SendButtonWidget->setText("Send");
    Layout->addWidget(m_SendButtonWidget);

    m_MidiMessageEditorWidget = new IEMidiMessageEditorWidget(m_MidiDeviceOutputProperty.MidiMessage, this);
    m_MidiMessageEditorWidget->SetValues(m_MidiDeviceOutputProperty.MidiMessage);
    m_MidiMessageEditorWidget->connect(m_MidiMessageEditorWidget, &IEMidiMessageEditorWidget::OnMidiMessageCommitted, this, &IEMidiOutputEditorWidget::OnMidiMessageCommitted);
    Layout->addWidget(m_MidiMessageEditorWidget);

    if (QPushButton* const DeleteButton = new QPushButton(this))
    {
        DeleteButton->setText("Delete");
        DeleteButton->connect(DeleteButton, &QPushButton::pressed, [this]()
            {
                emit OnDeleted();
                deleteLater();
            });

        Layout->addStretch(1);
        Layout->addWidget(DeleteButton);
    }
}

void IEMidiOutputEditorWidget::OnMidiMessageCommitted()
{
    if (m_MidiMessageEditorWidget)
    {
        m_MidiDeviceOutputProperty.MidiMessage = m_MidiMessageEditorWidget->GetValues();
    }
}

void IEMidiOutputEditorWidget::OnSendButtonPressed()
{
    emit OnMidiMessageSendRequested();
}