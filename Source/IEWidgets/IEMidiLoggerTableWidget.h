// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include <array>

#include "QTableWidget.h"
#include "QWidget.h"

#include "IEConcurrency.h"
#include "IEMidiTypes.h"

class IEMidiLoggerTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IEMidiLoggerTableWidget(IESPSCQueue<std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>>& IncomingMidiMessages, QWidget* Parent = nullptr);

private:
    QTableWidgetItem* CreateCenteredTableWidgetItem(const QString& Text, bool bBold = false) const;
    void FlushMidiMessagesToTable();

private:
    IESPSCQueue<std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>>& m_MidiLogMessagesBuffer;

private:
    QTableWidget* m_MidiLoggerTableWidget;
    int m_CurrentRow = 1;
};