// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEActionTypeDropdownWidget.h"

#include "QBoxLayout.h"
#include "QComboBox.h"

IEActionTypeDropdownWidget::IEActionTypeDropdownWidget(QWidget* Parent) : QWidget(Parent)
{
    QVBoxLayout* const Layout = new QVBoxLayout(this);

    m_ComboBox = new QComboBox(this);
    m_ComboBox->addItem("Option 1");
    m_ComboBox->addItem("Option 2");
    m_ComboBox->addItem("Option 3");
    Layout->setContentsMargins(0, 0, 0, 0);
    Layout->addWidget(m_ComboBox);
}