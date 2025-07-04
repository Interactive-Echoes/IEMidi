// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "qwidget.h"

#include "IEMidiTypes.h"

class QSpinBox;

class IEMidiMessageEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IEMidiMessageEditorWidget(const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>& MidiMessage, QWidget* Parent = nullptr);

public:
    std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT> GetValues() const;
    void SetValues(const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>& MidiMessage) const;
    void ShowByteWidget(size_t Index) const;
    void HideByteWidget(size_t Index) const;
    
Q_SIGNALS:
    void OnMidiMessageCommitted() const;

private Q_SLOTS:
    void OnMidiByteCommitted() const;

private:
    std::array<QSpinBox*, MIDI_MESSAGE_BYTE_COUNT> m_SpinBoxWidgets;
};