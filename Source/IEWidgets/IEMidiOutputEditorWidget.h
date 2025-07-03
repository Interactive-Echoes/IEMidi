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
    void OnSendMidiButtonPressed(const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>& MidiMessage) const;

private Q_SLOTS:
    void OnSendButtonPressed() const;
    void OnMidiMessageCommitted() const;
    void OnDeleteButtonPressed();

private:
    IEMidiDeviceOutputProperty& m_MidiDeviceOutputProperty;

private:
    IEMidiMessageEditorWidget* m_MidiMessageEditorWidget;
    QPushButton* m_SendButtonWidget;
};