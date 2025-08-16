// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMidiLogger.h"

#include "qboxlayout.h"
#include "qheaderview.h"
#include "qlabel.h"
#include "qtimer.h"

IEMidiLogger::IEMidiLogger(IESPSCQueue<std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>>& IncomingMidiMessages, QWidget* Parent) :
    QFrame(Parent),
    m_MidiLogMessagesBuffer(IncomingMidiMessages)
{
    QLabel* const MidiLoggerLabel = new QLabel("Midi Logger", this);
    MidiLoggerLabel->setAlignment(Qt::AlignCenter);
    MidiLoggerLabel->setStyleSheet(R"(
        QLabel 
        {
            font-size: 16px; 
            font-weight: bold;
            background: transparent;
            border: none;
        }
    )");

    m_MidiLoggerTableWidget = new QTableWidget(IncomingMidiMessages.GetCapacity(), 3, this);
    m_MidiLoggerTableWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_MidiLoggerTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_MidiLoggerTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    m_MidiLoggerTableWidget->setFocusPolicy(Qt::NoFocus);
    m_MidiLoggerTableWidget->setMouseTracking(false);
    m_MidiLoggerTableWidget->setAutoFillBackground(false);
    m_MidiLoggerTableWidget->setShowGrid(false);
    m_MidiLoggerTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_MidiLoggerTableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_MidiLoggerTableWidget->setStyleSheet(R"(
        QTableWidget 
        {
            background: transparent;
            border: none;
        }
    )");

    if (QHeaderView* const HHeader = m_MidiLoggerTableWidget->horizontalHeader())
    {
        HHeader->setVisible(false);
        HHeader->setSectionResizeMode(QHeaderView::Stretch);
    }

    if (QHeaderView* const VHeader = m_MidiLoggerTableWidget->verticalHeader())
    {
        VHeader->setVisible(false);
        VHeader->setSectionResizeMode(QHeaderView::Stretch);
    }

    m_MidiLoggerTableWidget->setItem(0, 0, CreateCenteredTableWidgetItem("Status", true));
    m_MidiLoggerTableWidget->setItem(0, 1, CreateCenteredTableWidgetItem("Data 1", true));
    m_MidiLoggerTableWidget->setItem(0, 2, CreateCenteredTableWidgetItem("Data 2", true));

    QTimer* const UpdateTimer = new QTimer(this);
    connect(UpdateTimer, &QTimer::timeout, this, &IEMidiLogger::FlushMidiMessagesToTable);
    UpdateTimer->start(25);
    
    QVBoxLayout* const Layout = new QVBoxLayout(this);
    Layout->setContentsMargins(15, 15, 15, 15);
    Layout->addWidget(MidiLoggerLabel);
    Layout->addSpacing(10);
    Layout->addWidget(m_MidiLoggerTableWidget, 1);
}

void IEMidiLogger::FlushMidiMessagesToTable() const
{
    int Row = 1;
    while (!m_MidiLogMessagesBuffer.IsEmpty())
    {
        std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT> MidiMessage;
        if (m_MidiLogMessagesBuffer.Pop(MidiMessage))
        {
            m_MidiLoggerTableWidget->insertRow(Row);
            m_MidiLoggerTableWidget->setItem(Row, 0, CreateCenteredTableWidgetItem(std::to_string(MidiMessage[0]).c_str(), false));
            m_MidiLoggerTableWidget->setItem(Row, 1, CreateCenteredTableWidgetItem(std::to_string(MidiMessage[1]).c_str(), false));
            m_MidiLoggerTableWidget->setItem(Row, 2, CreateCenteredTableWidgetItem(std::to_string(MidiMessage[2]).c_str(), false));
            Row++;

            const int RowCount = m_MidiLoggerTableWidget->rowCount();
            if (RowCount > m_MidiLogMessagesBuffer.GetCapacity())
            {
                m_MidiLoggerTableWidget->removeRow(RowCount - 1);
            }
        }
    }
}

QTableWidgetItem* IEMidiLogger::CreateCenteredTableWidgetItem(const QString& Text, bool bBold) const
{
    QTableWidgetItem* const TableWidgetItem = new QTableWidgetItem(Text);
    TableWidgetItem->setTextAlignment(Qt::AlignCenter);
    QFont Font = TableWidgetItem->font();
    Font.setBold(bBold);
    TableWidgetItem->setFont(Font);
    TableWidgetItem->setBackground(Qt::NoBrush);
    TableWidgetItem->setFlags(TableWidgetItem->flags() & ~Qt::ItemIsSelectable & ~Qt::ItemIsEditable);
    return TableWidgetItem;
};
