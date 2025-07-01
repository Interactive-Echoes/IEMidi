// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include "QWidget.h"

#include "IEMidiTypes.h"

class QComboBox;

class IEMidiMessageTypeDropdownWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IEMidiMessageTypeDropdownWidget(QWidget* Parent = nullptr);

public:
    void SetValue(IEMidiMessageType MidiMessageType);
    IEMidiMessageType GetValue() const;

Q_SIGNALS:
    void OnMidiMessageTypeChanged(IEMidiMessageType OldMidiMessageType, IEMidiMessageType NewMidiMessageType);

private Q_SLOTS:
    void OnComboBoxIndexChanged(int NewIndex);

private:
    QComboBox* m_ComboBoxWidget;

private:
    IEMidiMessageType m_CachedMidiMessageType = IEMidiMessageType::None;
};