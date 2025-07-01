// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include <array>
#include <vector>

#include "QWidget.h"

#include "IEMidiTypes.h"

class IEMidiMessageEditorWidget;
class QPushButton;

class IEMidiOutputEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IEMidiOutputEditorWidget(const IEMidiDeviceOutputProperty& MidiDeviceOutputProperty, QWidget* Parent = nullptr);

signals:
    void OnDelete();

private:
    IEMidiMessageEditorWidget* m_MidiMessageEditor;
    QPushButton* m_SendButton;
};