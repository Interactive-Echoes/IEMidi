// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include "QWidget.h"

class QComboBox;

class IEActionTypeDropdownWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IEActionTypeDropdownWidget(QWidget* Parent = nullptr);

private:
    QComboBox* m_ComboBox;
};