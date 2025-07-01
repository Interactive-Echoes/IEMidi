// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include <array>
#include <vector>

#include "QWidget.h"

#include "IEMidiTypes.h"

class IEActionTypeDropdownWidget;
class IEMidiMessageEditorWidget;
class IEMidiTypeDropdownWidget;
class QCheckBox;
class QPushButton;

class IEMidiInputEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IEMidiInputEditorWidget(const IEMidiDeviceInputProperty& MidiDeviceInputProperty, QWidget* Parent = nullptr);

signals:
    void OnDelete();

private:
    IEActionTypeDropdownWidget* m_ActionTypeDropdown;
    IEMidiMessageEditorWidget* m_MidiMessageEditor;
    IEMidiTypeDropdownWidget* m_MidiTypeDropdown;
    QCheckBox* m_MidiToggleCheckbox;
    QPushButton* m_RecordButton;
};