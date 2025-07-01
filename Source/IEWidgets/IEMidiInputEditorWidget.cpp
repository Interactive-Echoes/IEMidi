// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMidiInputEditorWidget.h"

#include "QBoxLayout.h"
#include "QCheckBox.h"
#include "QLineEdit.h"
#include "QPushButton.h"

#include "IEFileBrowserWidget.h"
#include "IEMidiActionTypeDropdownWidget.h"
#include "IEMidiMessageEditorWidget.h"
#include "IEMidiMessageTypeDropdownWidget.h"

IEMidiInputEditorWidget::IEMidiInputEditorWidget(IEMidiDeviceInputProperty& MidiDeviceInputProperty, QWidget* Parent) :
    QWidget(Parent),
    m_MidiDeviceInputProperty(MidiDeviceInputProperty)
{
    if (QHBoxLayout* const Layout = new QHBoxLayout(this))
    {
        Layout->setContentsMargins(0, 0, 0, 0);
        Layout->setSpacing(10);

        m_MidiMessageTypeDropdownWidget = new IEMidiMessageTypeDropdownWidget(this);
        m_MidiMessageTypeDropdownWidget->SetValue(m_MidiDeviceInputProperty.MidiMessageType);
        m_MidiMessageTypeDropdownWidget->connect(m_MidiMessageTypeDropdownWidget, &IEMidiMessageTypeDropdownWidget::OnMidiMessageTypeChanged,
            this, &IEMidiInputEditorWidget::OnMidiMessageTypeChanged);
        Layout->addWidget(m_MidiMessageTypeDropdownWidget);

        m_MidiToggleCheckboxWidget = new QCheckBox("Toggle", this);
        m_MidiToggleCheckboxWidget->setChecked(m_MidiDeviceInputProperty.bIsMidiToggle);
        m_MidiToggleCheckboxWidget->hide(); // Start hidden
        m_MidiToggleCheckboxWidget->connect(m_MidiToggleCheckboxWidget, &QCheckBox::checkStateChanged, this, &IEMidiInputEditorWidget::OnMidiToggleChanged);
        
        Layout->addWidget(m_MidiToggleCheckboxWidget);
        
        m_MidiActionTypeDropdownWidget = new IEMidiActionTypeDropdownWidget(this);
        m_MidiActionTypeDropdownWidget->SetValue(m_MidiDeviceInputProperty.MidiActionType);
        m_MidiActionTypeDropdownWidget->connect(m_MidiActionTypeDropdownWidget, &IEMidiActionTypeDropdownWidget::OnMidiActionTypeChanged,
            this, &IEMidiInputEditorWidget::OnMidiActionTypeChanged);
        Layout->addWidget(m_MidiActionTypeDropdownWidget);

        m_OpenFileBrowserWidget = new IEFileBrowserWidget(this);
        m_OpenFileBrowserWidget->SetFilePath(m_MidiDeviceInputProperty.OpenFilePath);
        m_OpenFileBrowserWidget->hide(); // Start hidden
        m_OpenFileBrowserWidget->connect(m_OpenFileBrowserWidget, &IEFileBrowserWidget::OnFilePathCommitted, this, &IEMidiInputEditorWidget::OnOpenFilePathCommited);
        Layout->addWidget(m_OpenFileBrowserWidget);
        
        m_ConsoleCommandWidget = new QLineEdit(this);
        m_ConsoleCommandWidget->setPlaceholderText("Command");
        m_ConsoleCommandWidget->setText(QString::fromStdString(m_MidiDeviceInputProperty.ConsoleCommand));
        m_ConsoleCommandWidget->hide(); // Start hidden
        m_ConsoleCommandWidget->connect(m_ConsoleCommandWidget, &QLineEdit::editingFinished, this, &IEMidiInputEditorWidget::OnConsoleCommandTextCommited);
        Layout->addWidget(m_ConsoleCommandWidget);

        m_RecordButtonWidget = new QPushButton(this);
        m_MidiDeviceInputProperty.bIsRecording = false;
        m_RecordButtonWidget->setText("Record");
        m_RecordButtonWidget->setCheckable(true);
        m_RecordButtonWidget->connect(m_RecordButtonWidget, &QPushButton::toggled, this, &IEMidiInputEditorWidget::OnRecordButtonToggled);
        Layout->addWidget(m_RecordButtonWidget);

        m_MidiMessageEditorWidget = new IEMidiMessageEditorWidget(MidiDeviceInputProperty.MidiMessage, this);
        m_MidiMessageEditorWidget->SetValues(m_MidiDeviceInputProperty.MidiMessage);
        m_MidiMessageEditorWidget->HideByteWidget(2);
        m_MidiMessageEditorWidget->connect(m_MidiMessageEditorWidget, &IEMidiMessageEditorWidget::OnMidiMessageCommitted, this, &IEMidiInputEditorWidget::OnMidiMessageCommitted);
        Layout->addWidget(m_MidiMessageEditorWidget);

        if (QPushButton* const DeleteButton = new QPushButton(this))
        {
            DeleteButton->setText("Delete");
            DeleteButton->connect(DeleteButton, &QPushButton::pressed, [this]()
                {
                    emit OnDeleted();
                    this->deleteLater();
                });

            Layout->addStretch(1);
            Layout->addWidget(DeleteButton);
        }
    }

    OnMidiToggleChanged(m_MidiToggleCheckboxWidget->checkState());
    OnMidiMessageTypeChanged(IEMidiMessageType::None, m_MidiMessageTypeDropdownWidget->GetValue());
    OnMidiActionTypeChanged(IEMidiActionType::None, m_MidiActionTypeDropdownWidget->GetValue());
}

bool IEMidiInputEditorWidget::IsRecording() const
{
    bool bIsRecording = false;
    if (m_RecordButtonWidget)
    {
        bIsRecording = m_RecordButtonWidget->isChecked();
    }
    return bIsRecording;
}

void IEMidiInputEditorWidget::OnMidiMessageTypeChanged(IEMidiMessageType OldMidiMessageType, IEMidiMessageType NewMidiMessageType)
{
    m_MidiDeviceInputProperty.MidiMessageType = NewMidiMessageType;

    if (m_MidiToggleCheckboxWidget)
    {
        if (NewMidiMessageType == IEMidiMessageType::NoteOnOff)
        {
            m_MidiToggleCheckboxWidget->show();
        }
        else
        {
            m_MidiToggleCheckboxWidget->hide();
        }
    }
}

void IEMidiInputEditorWidget::OnMidiToggleChanged(Qt::CheckState CheckState)
{
    m_MidiDeviceInputProperty.bIsMidiToggle = CheckState == Qt::CheckState::Checked;
}

void IEMidiInputEditorWidget::OnMidiActionTypeChanged(IEMidiActionType OldMidiActionType, IEMidiActionType NewMidiActionType)
{
    m_MidiDeviceInputProperty.MidiActionType = NewMidiActionType;

    if (m_OpenFileBrowserWidget)
    {
        m_OpenFileBrowserWidget->hide();
    }
    if (m_ConsoleCommandWidget)
    {
        m_ConsoleCommandWidget->hide();
    }

    switch (NewMidiActionType)
    {
        case IEMidiActionType::ConsoleCommand:
        {
            if (m_ConsoleCommandWidget)
            {
                m_ConsoleCommandWidget->show();
            }
            break;
        }
        case IEMidiActionType::OpenFile:
        {
            if (m_OpenFileBrowserWidget)
            {
                m_OpenFileBrowserWidget->show();
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

void IEMidiInputEditorWidget::OnOpenFilePathCommited()
{
    if (m_OpenFileBrowserWidget)
    {
        m_MidiDeviceInputProperty.OpenFilePath = m_OpenFileBrowserWidget->GetFilePath();
    }
}

void IEMidiInputEditorWidget::OnConsoleCommandTextCommited()
{
    if (m_ConsoleCommandWidget)
    {
        m_MidiDeviceInputProperty.ConsoleCommand = m_ConsoleCommandWidget->text().toStdString();
    }
}

void IEMidiInputEditorWidget::OnRecordButtonToggled(bool bToggled)
{
    m_MidiDeviceInputProperty.bIsRecording = bToggled;
}

void IEMidiInputEditorWidget::OnMidiMessageCommitted()
{
    if (m_MidiMessageEditorWidget)
    {
        m_MidiDeviceInputProperty.MidiMessage = m_MidiMessageEditorWidget->GetValues();
    }
}