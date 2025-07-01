// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include "QWidget.h"

class QComboBox;

class IEMidiTypeDropdownWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IEMidiTypeDropdownWidget(QWidget* Parent = nullptr);

private:
    QComboBox* m_ComboBox;
};