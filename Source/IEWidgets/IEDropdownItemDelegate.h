// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "qstyleditemdelegate.h"

class IEDropdownItemDelegate : public QStyledItemDelegate
{
public:
    explicit IEDropdownItemDelegate(QObject* Parent = nullptr);
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};