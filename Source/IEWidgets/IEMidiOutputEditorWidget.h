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
    explicit IEMidiOutputEditorWidget(IEMidiDeviceOutputProperty& MidiDeviceOutputProperty, QWidget* Parent = nullptr);

Q_SIGNALS:
    void OnMidiMessageSendRequested();
    void OnDeleted();

private Q_SLOTS:
    void OnSendButtonPressed();
    void OnMidiMessageCommitted();

private:
    IEMidiDeviceOutputProperty& m_MidiDeviceOutputProperty;

private:
    IEMidiMessageEditorWidget* m_MidiMessageEditorWidget;
    QPushButton* m_SendButtonWidget;
};