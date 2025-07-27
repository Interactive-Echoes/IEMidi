// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include "qtablewidget.h"
#include "qwidget.h"

#include "IEMidiProcessor.h"

class IEMidiDeviceInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IEMidiDeviceInfoWidget(const IEMidiProcessor& MidiProcessor, QWidget* Parent = nullptr);

private:
    QTableWidgetItem* CreateCenteredTableWidgetItem(const QString& Text, bool bBold = false) const;

private:
    const IEMidiProcessor& m_MidiProcessor;
};