// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include "qcombobox.h"

#include "IEMidiTypes.h"

class IEMidiActionTypeDropdown : public QComboBox
{
    Q_OBJECT

public:
    explicit IEMidiActionTypeDropdown(QWidget* Parent = nullptr);

public:
    void SetValue(IEMidiActionType MidiActionType);
    IEMidiActionType GetValue() const;

protected:
    void paintEvent(QPaintEvent* PaintEvent) override;

Q_SIGNALS:
    void OnMidiActionTypeChanged(IEMidiActionType OldMidiActionType, IEMidiActionType NewMidiActionType);

private Q_SLOTS:
    void OnComboBoxIndexChanged(int NewIndex);

private:
    IEMidiActionType m_CachedMidiActionType = IEMidiActionType::None;
};