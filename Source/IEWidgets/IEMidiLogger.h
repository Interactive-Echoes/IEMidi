// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include <array>

#include "qframe.h"
#include "qtablewidget.h"
#include "qwidget.h"

#include "IEConcurrency.h"
#include "IEMidiTypes.h"

class IEMidiLogger : public QFrame
{
    Q_OBJECT

public:
    explicit IEMidiLogger(IESPSCQueue<std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>>& IncomingMidiMessages, QWidget* Parent = nullptr);

private:
    QTableWidgetItem* CreateCenteredTableWidgetItem(const QString& Text, bool bBold = false) const;
    void FlushMidiMessagesToTable() const;

private:
    IESPSCQueue<std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>>& m_MidiLogMessagesBuffer;

private:
    QTableWidget* m_MidiLoggerTableWidget;
};