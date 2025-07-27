// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMidiInputEditorWidget.h"

#include "IELog.h"
#include "qboxlayout.h"
#include "qcheckbox.h"
#include "qcoreevent.h"
#include "qlineedit.h"
#include "qmetaobject.h"
#include "qpushbutton.h"

#include "IEFileBrowserWidget.h"
#include "IEMidiActionTypeDropdownWidget.h"
#include "IEMidiMessageEditorWidget.h"
#include "IEMidiMessageTypeDropdownWidget.h"

IEMidiInputEditorWidget::IEMidiInputEditorWidget(IEMidiDeviceInputProperty& MidiDeviceInputProperty, QWidget* Parent) :
    QWidget(Parent),
    m_MidiDeviceInputProperty(MidiDeviceInputProperty)
{
    QWidget* const SubWidget1 = new QWidget(this);

    m_MidiMessageTypeDropdownWidget = new IEMidiMessageTypeDropdownWidget(SubWidget1);
    m_MidiMessageTypeDropdownWidget->SetValue(m_MidiDeviceInputProperty.MidiMessageType);
    m_MidiMessageTypeDropdownWidget->connect(m_MidiMessageTypeDropdownWidget, &IEMidiMessageTypeDropdownWidget::OnMidiMessageTypeChanged,
        this, &IEMidiInputEditorWidget::OnMidiMessageTypeChanged);

    m_MidiToggleCheckboxWidget = new QCheckBox("Toggle", SubWidget1);
    m_MidiToggleCheckboxWidget->setChecked(m_MidiDeviceInputProperty.bIsMidiToggle);
    m_MidiToggleCheckboxWidget->hide(); // Start hidden
    m_MidiToggleCheckboxWidget->connect(m_MidiToggleCheckboxWidget, &QCheckBox::checkStateChanged, this, &IEMidiInputEditorWidget::OnMidiToggleChanged);

    m_MidiActionTypeDropdownWidget = new IEMidiActionTypeDropdownWidget(SubWidget1);
    m_MidiActionTypeDropdownWidget->SetValue(m_MidiDeviceInputProperty.MidiActionType);
    m_MidiActionTypeDropdownWidget->connect(m_MidiActionTypeDropdownWidget, &IEMidiActionTypeDropdownWidget::OnMidiActionTypeChanged,
        this, &IEMidiInputEditorWidget::OnMidiActionTypeChanged);

    m_OpenFileBrowserWidget = new IEFileBrowserWidget(SubWidget1);
    m_OpenFileBrowserWidget->SetFilePath(m_MidiDeviceInputProperty.OpenFilePath);
    m_OpenFileBrowserWidget->hide(); // Start hidden
    m_OpenFileBrowserWidget->connect(m_OpenFileBrowserWidget, &IEFileBrowserWidget::OnFilePathCommitted, this, &IEMidiInputEditorWidget::OnOpenFilePathCommited);

    m_ConsoleCommandWidget = new QLineEdit(SubWidget1);
    m_ConsoleCommandWidget->setPlaceholderText("Command");
    m_ConsoleCommandWidget->setText(QString::fromStdString(m_MidiDeviceInputProperty.ConsoleCommand));
    m_ConsoleCommandWidget->hide(); // Start hidden
    m_ConsoleCommandWidget->connect(m_ConsoleCommandWidget, &QLineEdit::editingFinished, this, &IEMidiInputEditorWidget::OnConsoleCommandTextCommited);

    QHBoxLayout* const SubLayout1 = new QHBoxLayout(SubWidget1);
    SubLayout1->setContentsMargins(0, 0, 0, 0);
    SubLayout1->setSpacing(10);
    SubLayout1->addWidget(m_MidiMessageTypeDropdownWidget);
    SubLayout1->addWidget(m_MidiToggleCheckboxWidget);
    SubLayout1->addWidget(m_MidiActionTypeDropdownWidget);
    SubLayout1->addWidget(m_OpenFileBrowserWidget);
    SubLayout1->addWidget(m_ConsoleCommandWidget);
    SubLayout1->addStretch(1);

    QWidget* const SubWidget2 = new QWidget(this);
    SubWidget2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_MidiDeviceInputProperty.bIsRecording = false;
    m_RecordButtonWidget = new QPushButton("Record", SubWidget2);
    m_RecordButtonWidget->setCheckable(true);
    m_RecordButtonWidget->setChecked(false);
    m_RecordButtonWidget->connect(m_RecordButtonWidget, &QPushButton::toggled, this, &IEMidiInputEditorWidget::OnRecordButtonToggled);
    m_RecordButtonWidget->setStyleSheet(R"(
        QPushButton {
            background-color: rgb(100, 70, 10);
        }

        QPushButton:hover {
            background-color: rgb(60, 2, 2);
        }
        
        QPushButton:checked {
            background-color: rgb(60, 2, 2);
        }
    )");

    m_MidiMessageEditorWidget = new IEMidiMessageEditorWidget(m_MidiDeviceInputProperty.MidiMessage, SubWidget2);
    m_MidiMessageEditorWidget->SetValues(m_MidiDeviceInputProperty.MidiMessage);
    m_MidiMessageEditorWidget->HideByteWidget(2);
    m_MidiMessageEditorWidget->connect(m_MidiMessageEditorWidget, &IEMidiMessageEditorWidget::OnMidiMessageCommitted, this, &IEMidiInputEditorWidget::OnMidiMessageCommitted);

    QHBoxLayout* const SubLayout2 = new QHBoxLayout(SubWidget2);
    SubLayout2->setContentsMargins(0, 0, 0, 0);
    SubLayout2->setSpacing(10);
    SubLayout2->addWidget(m_RecordButtonWidget);
    SubLayout2->addWidget(m_MidiMessageEditorWidget);

    QPushButton* const DeleteButton = new QPushButton("Delete", this);
    DeleteButton->connect(DeleteButton, &QPushButton::pressed, this, &IEMidiInputEditorWidget::OnDeleteButtonPressed);

    QHBoxLayout* const Layout = new QHBoxLayout(this);
    Layout->setContentsMargins(0, 0, 0, 0);
    Layout->addWidget(SubWidget1, 2);
    Layout->addWidget(SubWidget2, 2);
    Layout->addStretch(1);
    Layout->addWidget(DeleteButton);

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
