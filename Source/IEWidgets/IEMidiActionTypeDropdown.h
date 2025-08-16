// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include "qwidget.h"

#include "IEMidiTypes.h"

class QComboBox;

class IEMidiActionTypeDropdown : public QWidget
{
    Q_OBJECT

public:
    explicit IEMidiActionTypeDropdown(QWidget* Parent = nullptr);

public:
    void SetValue(IEMidiActionType MidiActionType);
    IEMidiActionType GetValue() const;

Q_SIGNALS:
    void OnMidiActionTypeChanged(IEMidiActionType OldMidiActionType, IEMidiActionType NewMidiActionType);

private Q_SLOTS:
    void OnComboBoxIndexChanged(int NewIndex);

private:
    QComboBox* m_ComboBoxWidget;

private:
    IEMidiActionType m_CachedMidiActionType = IEMidiActionType::None;
};