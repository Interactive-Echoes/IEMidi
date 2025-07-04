// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMidiInputEditorWidget.h"

#include "IELog.h"
#include "QBoxLayout.h"
#include "QCheckBox.h"
#include "QCoreEvent.h"
#include "QLineEdit.h"
#include "QMetaObject.h"
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

        if (QWidget* const SubWidget = new QWidget(this))
        {
            Layout->addWidget(SubWidget, 2);
            if (QHBoxLayout* const SubLayout = new QHBoxLayout(SubWidget))
            {
                SubWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
                SubLayout->setContentsMargins(0, 0, 0, 0);
                SubLayout->setSpacing(10);

                m_MidiMessageTypeDropdownWidget = new IEMidiMessageTypeDropdownWidget(SubWidget);
                m_MidiMessageTypeDropdownWidget->SetValue(m_MidiDeviceInputProperty.MidiMessageType);
                m_MidiMessageTypeDropdownWidget->connect(m_MidiMessageTypeDropdownWidget, &IEMidiMessageTypeDropdownWidget::OnMidiMessageTypeChanged,
                    this, &IEMidiInputEditorWidget::OnMidiMessageTypeChanged);
                SubLayout->addWidget(m_MidiMessageTypeDropdownWidget);

                m_MidiToggleCheckboxWidget = new QCheckBox("Toggle", SubWidget);
                m_MidiToggleCheckboxWidget->setChecked(m_MidiDeviceInputProperty.bIsMidiToggle);
                m_MidiToggleCheckboxWidget->hide(); // Start hidden
                m_MidiToggleCheckboxWidget->connect(m_MidiToggleCheckboxWidget, &QCheckBox::checkStateChanged, this, &IEMidiInputEditorWidget::OnMidiToggleChanged);
                SubLayout->addWidget(m_MidiToggleCheckboxWidget);

                m_MidiActionTypeDropdownWidget = new IEMidiActionTypeDropdownWidget(SubWidget);
                m_MidiActionTypeDropdownWidget->SetValue(m_MidiDeviceInputProperty.MidiActionType);
                m_MidiActionTypeDropdownWidget->connect(m_MidiActionTypeDropdownWidget, &IEMidiActionTypeDropdownWidget::OnMidiActionTypeChanged,
                    this, &IEMidiInputEditorWidget::OnMidiActionTypeChanged);
                SubLayout->addWidget(m_MidiActionTypeDropdownWidget);

                m_OpenFileBrowserWidget = new IEFileBrowserWidget(SubWidget);
                m_OpenFileBrowserWidget->SetFilePath(m_MidiDeviceInputProperty.OpenFilePath);
                m_OpenFileBrowserWidget->hide(); // Start hidden
                m_OpenFileBrowserWidget->connect(m_OpenFileBrowserWidget, &IEFileBrowserWidget::OnFilePathCommitted, this, &IEMidiInputEditorWidget::OnOpenFilePathCommited);
                SubLayout->addWidget(m_OpenFileBrowserWidget);

                m_ConsoleCommandWidget = new QLineEdit(SubWidget);
                m_ConsoleCommandWidget->setPlaceholderText("Command");
                m_ConsoleCommandWidget->setText(QString::fromStdString(m_MidiDeviceInputProperty.ConsoleCommand));
                m_ConsoleCommandWidget->hide(); // Start hidden
                m_ConsoleCommandWidget->connect(m_ConsoleCommandWidget, &QLineEdit::editingFinished, this, &IEMidiInputEditorWidget::OnConsoleCommandTextCommited);
                SubLayout->addWidget(m_ConsoleCommandWidget);
            }
        }

        Layout->addStretch(1);

        if (QWidget* const SubWidget = new QWidget(this))
        {
            Layout->addWidget(SubWidget, 2);
            if (QHBoxLayout* const SubLayout = new QHBoxLayout(SubWidget))
            {
                SubWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
                SubLayout->setContentsMargins(0, 0, 0, 0);
                SubLayout->setSpacing(10);

                m_RecordButtonWidget = new QPushButton(SubWidget);
                m_MidiDeviceInputProperty.bIsRecording = false;
                m_RecordButtonWidget->setText("Record");
                m_RecordButtonWidget->setCheckable(true);
                m_RecordButtonWidget->setStyleSheet(R"(
                    QPushButton:checked {
                        background-color: rgb(75, 3, 3);
                    }
                )");
                    m_RecordButtonWidget->connect(m_RecordButtonWidget, &QPushButton::toggled, this, &IEMidiInputEditorWidget::OnRecordButtonToggled);
                SubLayout->addWidget(m_RecordButtonWidget, 1);

                m_MidiMessageEditorWidget = new IEMidiMessageEditorWidget(m_MidiDeviceInputProperty.MidiMessage, SubWidget);
                m_MidiMessageEditorWidget->SetValues(m_MidiDeviceInputProperty.MidiMessage);
                m_MidiMessageEditorWidget->HideByteWidget(2);
                m_MidiMessageEditorWidget->connect(m_MidiMessageEditorWidget, &IEMidiMessageEditorWidget::OnMidiMessageCommitted, this, &IEMidiInputEditorWidget::OnMidiMessageCommitted);
                SubLayout->addWidget(m_MidiMessageEditorWidget, 1);

                if (QPushButton* const DeleteButton = new QPushButton(SubWidget))
                {
                    DeleteButton->setText("Delete");
                    DeleteButton->connect(DeleteButton, &QPushButton::pressed, this, &IEMidiInputEditorWidget::OnDeleteButtonPressed);

                    SubLayout->addStretch(2);
                    SubLayout->addSpacing(200);
                    SubLayout->addWidget(DeleteButton);
                }
            }
        }

        OnMidiToggleChanged(m_MidiToggleCheckboxWidget->checkState());
        OnMidiMessageTypeChanged(IEMidiMessageType::None, m_MidiMessageTypeDropdownWidget->GetValue());
        OnMidiActionTypeChanged(IEMidiActionType::None, m_MidiActionTypeDropdownWidget->GetValue());
    }
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

void IEMidiInputEditorWidget::OnMidiMessageTypeChanged(IEMidiMessageType OldMidiMessageType, IEMidiMessageType NewMidiMessageType) const
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

void IEMidiInputEditorWidget::paintEvent(QPaintEvent* event)
{
    if (event)
    {
        if (m_RecordButtonWidget)
        {
            m_RecordButtonWidget->setChecked(m_MidiDeviceInputProperty.bIsRecording);
        }

        if (m_MidiMessageEditorWidget)
        {
            m_MidiMessageEditorWidget->SetValues(m_MidiDeviceInputProperty.MidiMessage);
        }
    }

    QWidget::paintEvent(event);
}

void IEMidiInputEditorWidget::OnMidiToggleChanged(Qt::CheckState CheckState) const
{
    m_MidiDeviceInputProperty.bIsMidiToggle = CheckState == Qt::CheckState::Checked;
}

void IEMidiInputEditorWidget::OnMidiActionTypeChanged(IEMidiActionType OldMidiActionType, IEMidiActionType NewMidiActionType) const
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

void IEMidiInputEditorWidget::OnOpenFilePathCommited() const
{
    if (m_OpenFileBrowserWidget)
    {
        m_MidiDeviceInputProperty.OpenFilePath = m_OpenFileBrowserWidget->GetFilePath();
    }
}

void IEMidiInputEditorWidget::OnConsoleCommandTextCommited() const
{
    if (m_ConsoleCommandWidget)
    {
        m_MidiDeviceInputProperty.ConsoleCommand = m_ConsoleCommandWidget->text().toStdString();
    }
}

void IEMidiInputEditorWidget::OnRecordButtonToggled(bool bToggled) const
{
    m_MidiDeviceInputProperty.bIsRecording = bToggled;
    emit OnRecording();
}

void IEMidiInputEditorWidget::OnMidiMessageCommitted() const
{
    if (m_MidiMessageEditorWidget)
    {
        m_MidiDeviceInputProperty.MidiMessage = m_MidiMessageEditorWidget->GetValues();
    }
}

void IEMidiInputEditorWidget::OnDeleteButtonPressed()
{
    m_MidiDeviceInputProperty.Delete();
    deleteLater();
}
