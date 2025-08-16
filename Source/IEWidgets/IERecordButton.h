// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include "qpushbutton.h"

class IERecordButton : public QPushButton
{
    Q_OBJECT

public:
    explicit IERecordButton(QWidget* Parent = nullptr);

protected:
    void paintEvent(QPaintEvent* PaintEvent) override;
};