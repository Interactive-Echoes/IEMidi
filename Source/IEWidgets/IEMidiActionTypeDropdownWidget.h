// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include "QWidget.h"

#include "IEMidiTypes.h"

class QComboBox;

class IEMidiActionTypeDropdownWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IEMidiActionTypeDropdownWidget(QWidget* Parent = nullptr);

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