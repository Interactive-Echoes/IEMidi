// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include <array>
#include <vector>

#include "QWidget.h"

#include "IEMidiTypes.h"

class IEFileBrowserWidget;
class IEMidiActionTypeDropdownWidget;
class IEMidiMessageEditorWidget;
class IEMidiMessageTypeDropdownWidget;
class QCheckBox;
class QLineEdit;
class QPushButton;

class IEMidiInputEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IEMidiInputEditorWidget(IEMidiDeviceInputProperty& MidiDeviceInputProperty, QWidget* Parent = nullptr);

public:
    bool IsRecording() const;

Q_SIGNALS:
    void OnRecording() const;

protected:
    void paintEvent(QPaintEvent* event) override;

private Q_SLOTS:
    void OnMidiMessageTypeChanged(IEMidiMessageType OldMidiMessageType, IEMidiMessageType NewMidiMessageType) const;
    void OnMidiToggleChanged(Qt::CheckState CheckState) const;
    void OnMidiActionTypeChanged(IEMidiActionType OldMidiActionType, IEMidiActionType NewMidiActionType) const;
    void OnOpenFilePathCommited() const;
    void OnConsoleCommandTextCommited() const;
    void OnRecordButtonToggled(bool bToggled) const;
    void OnMidiMessageCommitted() const;
    void OnDeleteButtonPressed();

private:
    IEMidiDeviceInputProperty& m_MidiDeviceInputProperty;

private:
    IEFileBrowserWidget* m_OpenFileBrowserWidget;
    IEMidiActionTypeDropdownWidget* m_MidiActionTypeDropdownWidget;
    IEMidiMessageEditorWidget* m_MidiMessageEditorWidget;
    IEMidiMessageTypeDropdownWidget* m_MidiMessageTypeDropdownWidget;
    QCheckBox* m_MidiToggleCheckboxWidget;
    QLineEdit* m_ConsoleCommandWidget;
    QPushButton* m_RecordButtonWidget;
};