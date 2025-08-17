// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include "qimage.h"
#include "qpushbutton.h"

class IEDeletePropertyButton : public QPushButton
{
    Q_OBJECT

public:
    explicit IEDeletePropertyButton(QWidget* Parent = nullptr);

protected:
    void paintEvent(QPaintEvent* PaintEvent) override;
};