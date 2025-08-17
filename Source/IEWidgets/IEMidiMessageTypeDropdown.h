// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include "qcombobox.h"
#include "qpixmap.h"

#include "IEMidiTypes.h"

class IEMidiMessageTypeDropdown : public QComboBox
{
    Q_OBJECT

public:
    explicit IEMidiMessageTypeDropdown(QWidget* Parent = nullptr);

public:
    void SetValue(IEMidiMessageType MidiMessageType);
    IEMidiMessageType GetValue() const;

protected:
    void paintEvent(QPaintEvent* PaintEvent) override;

Q_SIGNALS:
    void OnMidiMessageTypeChanged(IEMidiMessageType OldMidiMessageType, IEMidiMessageType NewMidiMessageType);

private Q_SLOTS:
    void OnComboBoxIndexChanged(int NewIndex);

private:
    QPixmap m_Arrow;
    IEMidiMessageType m_CachedMidiMessageType = IEMidiMessageType::None;
};