// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMidiDeviceInfo.h"

#include "qboxlayout.h"
#include "qheaderview.h"
#include "qlabel.h"

IEMidiDeviceInfo::IEMidiDeviceInfo(const IEMidiProcessor& MidiProcessor, QWidget* Parent) :
    QFrame(Parent),
    m_MidiProcessor(MidiProcessor)
{
    if (m_MidiProcessor.HasActiveMidiDeviceProfile())
    {
        QLabel* const MidiDeviceInfoLabel = new QLabel("Midi Device Info", this);
        MidiDeviceInfoLabel->setAlignment(Qt::AlignLeft);
        MidiDeviceInfoLabel->setStyleSheet(R"(
            QLabel 
            {
                font-size: 16px; 
                font-weight: bold;
                background: transparent;
                border: none;
            }
        )");

        QTableWidget* const MidiDeviceInfoTableWidget = new QTableWidget(5, 2, this);
        MidiDeviceInfoTableWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        MidiDeviceInfoTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        MidiDeviceInfoTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
        MidiDeviceInfoTableWidget->setFocusPolicy(Qt::NoFocus);
        MidiDeviceInfoTableWidget->setMouseTracking(false);
        MidiDeviceInfoTableWidget->setAutoFillBackground(false);
        MidiDeviceInfoTableWidget->setShowGrid(false);
        MidiDeviceInfoTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        MidiDeviceInfoTableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        MidiDeviceInfoTableWidget->setContentsMargins(0, 0, 0, 0);

        if (QHeaderView* const HHeader = MidiDeviceInfoTableWidget->horizontalHeader())
        {
            HHeader->setVisible(false);
            HHeader->setSectionResizeMode(QHeaderView::Stretch);
        }

        if (QHeaderView* const VHeader = MidiDeviceInfoTableWidget->verticalHeader())
        {
            VHeader->setVisible(false);
            VHeader->setSectionResizeMode(QHeaderView::Stretch);
        }

        const IEMidiDeviceProfile& ActiveMidiDeviceProfile = m_MidiProcessor.GetActiveMidiDeviceProfile();

        MidiDeviceInfoTableWidget->setItem(0, 0, CreateCenteredTableWidgetItem("Name:", true));
        MidiDeviceInfoTableWidget->setItem(0, 1, CreateCenteredTableWidgetItem(
            ActiveMidiDeviceProfile.NameID.c_str()));

        MidiDeviceInfoTableWidget->setItem(1, 0, CreateCenteredTableWidgetItem("Input Port:", true));
        MidiDeviceInfoTableWidget->setItem(1, 1, CreateCenteredTableWidgetItem(
            std::to_string(ActiveMidiDeviceProfile.InputPortNumber).c_str()));

        MidiDeviceInfoTableWidget->setItem(2, 0, CreateCenteredTableWidgetItem("Output Port:", true));
        MidiDeviceInfoTableWidget->setItem(2, 1, CreateCenteredTableWidgetItem(
            std::to_string(ActiveMidiDeviceProfile.OutputPortNumber).c_str()));

        MidiDeviceInfoTableWidget->setItem(3, 0, CreateCenteredTableWidgetItem("Current API:", true));
        MidiDeviceInfoTableWidget->setItem(3, 1, CreateCenteredTableWidgetItem(m_MidiProcessor.GetAPIName().c_str()));

        MidiDeviceInfoTableWidget->setItem(4, 0, CreateCenteredTableWidgetItem("IEMidi Version:", true));
        MidiDeviceInfoTableWidget->setItem(4, 1, CreateCenteredTableWidgetItem("1"));

        QVBoxLayout* const Layout = new QVBoxLayout(this);
        Layout->setContentsMargins(20, 30, 20, 20);
        Layout->addWidget(MidiDeviceInfoLabel);
        Layout->addSpacing(30);
        Layout->addWidget(MidiDeviceInfoTableWidget, 1);
    }
}

QTableWidgetItem* IEMidiDeviceInfo::CreateCenteredTableWidgetItem(const QString& Text, bool bBold) const
{
    QTableWidgetItem* const TableWidgetItem = new QTableWidgetItem(Text);
    TableWidgetItem->setTextAlignment(Qt::AlignLeft);
    QFont Font = TableWidgetItem->font();
    bBold ? Font.setBold(true) : Font.setWeight(QFont::Weight::Normal);
    TableWidgetItem->setFont(Font);
    TableWidgetItem->setBackground(Qt::NoBrush);
    TableWidgetItem->setFlags(TableWidgetItem->flags() & ~Qt::ItemIsSelectable & ~Qt::ItemIsEditable);
    return TableWidgetItem;
}