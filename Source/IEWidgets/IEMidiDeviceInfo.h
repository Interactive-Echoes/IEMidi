// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include "qframe.h"
#include "qtablewidget.h"
#include "qwidget.h"

#include "IEMidiProcessor.h"

class IEMidiDeviceInfo : public QFrame
{
    Q_OBJECT

public:
    explicit IEMidiDeviceInfo(const IEMidiProcessor& MidiProcessor, QWidget* Parent = nullptr);

private:
    QTableWidgetItem* CreateCenteredTableWidgetItem(const QString& Text, bool bBold = false) const;

private:
    const IEMidiProcessor& m_MidiProcessor;
};