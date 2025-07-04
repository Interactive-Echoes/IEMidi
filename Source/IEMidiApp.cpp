// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMidiApp.h"

#include <string>

#include "QBoxLayout.h"
#include "QFontDatabase.h"
#include "QFrame.h"
#include "QHeaderView.h"
#include "QLabel.h"
#include "QMainWindow.h"
#include "QMenu.h"
#include "QObject.h"
#include "QPushButton.h"
#include "QSizePolicy.h"
#include "QSystemTrayIcon.h"
#include "QTableWidget.h"

#include "IELog.h"

#include "IEWidgets/IEMidiInputEditorWidget.h"
#include "IEWidgets/IEMidiLoggerTableWidget.h"
#include "IEWidgets/IEMidiOutputEditorWidget.h"

IEMidiApp::IEMidiApp(int& Argc, char** Argv) :
    QApplication(Argc, Argv),
    m_MainWindow(std::make_unique<QMainWindow>()),
    m_MidiProcessor(std::make_unique<IEMidiProcessor>()),
    m_MidiProfileManager(std::make_unique<IEMidiProfileManager>())
{
    m_OnMidiCallbackID = m_MidiProcessor->AddOnMidiCallback(this, &IEMidiApp::OnMidiCallback);

    m_MainWindow->setWindowTitle("IEMidi");
    m_MainWindow->resize(960, 720);
    m_MainWindow->setAutoFillBackground(true);
    const std::string& IconPath = std::format("{0}/IE-Brand-Kit/IE-Logo-NoBg.png", IEResources_Folder_Path); // compile time definition
    m_MainWindow->setWindowIcon(QIcon(IconPath.c_str()));

    m_TrayIcon = new QSystemTrayIcon(m_MainWindow.get());
    m_TrayIcon->setIcon(QIcon(IconPath.c_str()));
    if (QMenu* const TrayMenu = new QMenu(m_MainWindow.get()))
    {
        QAction* const ShowAction = new QAction("Show", TrayMenu);
        m_MainWindow->connect(ShowAction, &QAction::triggered, m_MainWindow.get(), &QMainWindow::show);
        TrayMenu->addAction(ShowAction);

        QAction* const QuitAction = new QAction("Quit", TrayMenu);
        connect(QuitAction, &QAction::triggered, this, &QCoreApplication::quit);
        TrayMenu->addAction(QuitAction);

        m_TrayIcon->setContextMenu(TrayMenu);
        m_TrayIcon->show();
    }

    const std::string& MainFontPath = std::format("{0}/Fonts/Fonts/Montserrat/Montserrat-VariableFont_wght.ttf", IEResources_Folder_Path); // compile time definition
    int MainFontId = QFontDatabase::addApplicationFont(MainFontPath.c_str());
    if (MainFontId != -1)
    {
        QString MainFontFamily = QFontDatabase::applicationFontFamilies(MainFontId).at(0);
        QFont MainFont(MainFontFamily, 13);
        MainFont.setWeight(QFont::Weight::Medium);
        setFont(MainFont);
    }

    //m_MainWindow->setStyleSheet("background-color: rgb(30, 30, 30);");
    // setStyleSheet(R"(
    //     QPushButton{ font - family: 'Montserrat'; font - size: 12pt; font - weight: DemiBold }
        
    //     )");

    setStyleSheet(R"(

        QMainWindow > QWidget {
            background-color: rgb(10, 10, 10);
        }

        QWidget {
            background-color: rgb(20, 20, 20);
            color: #d4d4d4;
            font-family: 'Segoe UI', sans-serif;
            font-size: 12pt;
            border-radius: 10px;
        }

        QPushButton {
            background-color:rgb(45, 45, 48);
            border: 1px solid #3c3c3c;
            padding: 4px 8px;
            border-radius: 4px;
        }
    
        QPushButton:hover {
            background-color: #3e3e42;
        }
    
        QLineEdit, QTextEdit {
            background-color: #252526;
            border: 1px solid #3c3c3c;
            padding: 4px;
        }

        QComboBox {
            background-color: #252526;
            border-radius: 5px;
            padding: 4px 28px 4px 8px;
        }
       
    )");
   
    DrawMidiDeviceSelectionWindow();
}

IEMidiApp::~IEMidiApp()
{
    if (m_MidiProcessor)
    {
        m_MidiProcessor->RemoveOnMidiCallback(m_OnMidiCallbackID);
    }
}

IEAppState IEMidiApp::GetAppState() const
{
    return m_AppState;
}

void IEMidiApp::SetAppState(IEAppState AppState)
{
    if (m_AppState != AppState)
    {
        m_AppState = AppState;
        OnAppStateChanged();
    }
}

void IEMidiApp::OnAppStateChanged()
{
    ResetMainWindowCentralWidget();
}

void IEMidiApp::DrawMidiDeviceSelectionWindow()
{
    if (QWidget* const CentralWidget = new QWidget(m_MainWindow.get()))
    {
        m_MainWindow->setCentralWidget(CentralWidget);

        if (QHBoxLayout* const CentralLayout = new QHBoxLayout(CentralWidget))
        {
            CentralWidget->setLayout(CentralLayout);
 
            if (QFrame* const MidiDeviceSelectionWidget = new QFrame(CentralWidget))
            {
                CentralLayout->addWidget(MidiDeviceSelectionWidget);
                MidiDeviceSelectionWidget->setFrameStyle(QFrame::Panel);
                MidiDeviceSelectionWidget->setMaximumSize(300, 600);

                if (QVBoxLayout* const MidiDeviceSelectionLayout = new QVBoxLayout(MidiDeviceSelectionWidget))
                {
                    MidiDeviceSelectionWidget->setLayout(MidiDeviceSelectionLayout);

                    MidiDeviceSelectionLayout->setSpacing(10);
                    MidiDeviceSelectionLayout->setAlignment(Qt::Alignment::enum_type::AlignCenter);

                    if (QLabel* const SelectMIDIDeviceLabel = new QLabel("Select MIDI Device", MidiDeviceSelectionWidget))
                    {
                        SelectMIDIDeviceLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
                        SelectMIDIDeviceLabel->setAlignment(Qt::AlignCenter);

                        MidiDeviceSelectionLayout->addWidget(SelectMIDIDeviceLabel);
                    }

                    if (m_MidiProcessor)
                    {
                        const std::vector<std::string> AvailableMidiDevices = m_MidiProcessor->GetAvailableMidiDevices();
                        if (!AvailableMidiDevices.empty())
                        {
                            for (const std::string& MidiDeviceName : AvailableMidiDevices)
                            {
                                if (QWidget* const MidiDeviceEntryWidget = new QWidget(MidiDeviceSelectionWidget))
                                {
                                    MidiDeviceSelectionLayout->addWidget(MidiDeviceEntryWidget);

                                    if (QHBoxLayout* const MidiDeviceEntryLayout = new QHBoxLayout(MidiDeviceEntryWidget))
                                    {
                                        MidiDeviceEntryWidget->setLayout(MidiDeviceEntryLayout);

                                        MidiDeviceEntryLayout->setSpacing(10);
                                        MidiDeviceEntryLayout->setAlignment(Qt::Alignment::enum_type::AlignCenter);

                                        if (QLabel* const MidiDeviceEntryLabel = new QLabel(MidiDeviceName.c_str(), MidiDeviceEntryWidget))
                                        {
                                            MidiDeviceEntryLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
                                            MidiDeviceEntryLayout->addWidget(MidiDeviceEntryLabel);
                                        }

                                        if (QPushButton* const MidiDeviceEditButton = new QPushButton(MidiDeviceEntryWidget))
                                        {
                                            MidiDeviceEditButton->setText("Edit");
                                            MidiDeviceEntryLayout->addWidget(MidiDeviceEditButton);
                                            MidiDeviceEditButton->connect(MidiDeviceEditButton, &QPushButton::pressed, [this, MidiDeviceName]()
                                                {
                                                    ActivateMidiDeviceProfile(MidiDeviceName);
                                                    DrawActiveMidiDeviceEditorWindow();
                                                });
                                        }

                                        if (QPushButton* const MidiDeviceActivateButton = new QPushButton(MidiDeviceEntryWidget))
                                        {
                                            MidiDeviceActivateButton->setText("Activate");
                                            MidiDeviceEntryLayout->addWidget(MidiDeviceActivateButton);
                                            MidiDeviceActivateButton->connect(MidiDeviceActivateButton, &QPushButton::pressed, [this, MidiDeviceName]()
                                                {
                                                    ActivateMidiDeviceProfile(MidiDeviceName);
                                                    RunInBackground();
                                                });
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            if (QLabel* const SelectMIDIDeviceLabel = new QLabel("No Midi Device Available", MidiDeviceSelectionWidget))
                            {
                                SelectMIDIDeviceLabel->setAlignment(Qt::AlignCenter);
                                SelectMIDIDeviceLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: rgb(255, 0, 0);");

                                MidiDeviceSelectionLayout->addWidget(SelectMIDIDeviceLabel);
                            }
                        }
                    }
                }
            }
        }
    }
    m_MainWindow->show();
}

void IEMidiApp::DrawActiveMidiDeviceEditorWindow()
{
    if (QWidget* const CentralWidget = new QWidget(m_MainWindow.get()))
    {
        m_MainWindow->setCentralWidget(CentralWidget);

        if (QHBoxLayout* const CentralLayout = new QHBoxLayout(CentralWidget))
        {
            CentralWidget->setLayout(CentralLayout);

            DrawActiveMidiDeviceSideBar(CentralWidget);
            DrawActiveMidiDeviceEditorFrameWidget(CentralWidget);
        }
    }
    m_MainWindow->show();
}

void IEMidiApp::DrawActiveMidiDeviceSideBar(QWidget* Parent)
{
    if (QFrame* const SideBarFrameWidget = new QFrame(Parent))
    {
        if (QBoxLayout* const ParentLayout = qobject_cast<QBoxLayout*>(Parent->layout()))
        {
            ParentLayout->addWidget(SideBarFrameWidget, 2);

            if (QVBoxLayout* const SideBarLayout = new QVBoxLayout(SideBarFrameWidget))
            {
                SideBarFrameWidget->setLayout(SideBarLayout);

                if (QFrame* const MidiDeviceInfoFrameWidget = new QFrame(SideBarFrameWidget))
                {
                    SideBarLayout->addWidget(MidiDeviceInfoFrameWidget);

                    if (QVBoxLayout* const MidiDeviceInfoLayout = new QVBoxLayout(MidiDeviceInfoFrameWidget))
                    {
                        MidiDeviceInfoFrameWidget->setLayout(MidiDeviceInfoLayout);

                        if (QTableWidget* const MidiDeviceInfoTableWidget = new QTableWidget(6, 2, MidiDeviceInfoFrameWidget))
                        {
                            MidiDeviceInfoLayout->addWidget(MidiDeviceInfoTableWidget);

                            MidiDeviceInfoTableWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
                            MidiDeviceInfoTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
                            MidiDeviceInfoTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
                            MidiDeviceInfoTableWidget->setFocusPolicy(Qt::NoFocus);
                            MidiDeviceInfoTableWidget->setMouseTracking(false);
                            MidiDeviceInfoTableWidget->setAutoFillBackground(false);
                            MidiDeviceInfoTableWidget->setShowGrid(false);
                            MidiDeviceInfoTableWidget->setStyleSheet(R"(
                                    QTableWidget 
                                    {
                                        background: transparent;
                                        border: none;
                                    }
                                )");

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

                            const auto CreateCenteredTableWidgetItem = [](const QString& Text, bool bBold = false)
                                {
                                    QTableWidgetItem* const TableWidgetItem = new QTableWidgetItem(Text);
                                    TableWidgetItem->setTextAlignment(Qt::AlignCenter);
                                    QFont Font = TableWidgetItem->font();
                                    Font.setPointSize(10);
                                    Font.setBold(bBold);
                                    TableWidgetItem->setFont(Font);
                                    TableWidgetItem->setBackground(Qt::NoBrush);
                                    return TableWidgetItem;
                                };

                            if (m_MidiProcessor)
                            {
                                if (m_MidiProcessor->HasActiveMidiDeviceProfile())
                                {
                                    const IEMidiDeviceProfile& ActiveMidiDeviceProfile = m_MidiProcessor->GetActiveMidiDeviceProfile();

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
                                    MidiDeviceInfoTableWidget->setItem(3, 1, CreateCenteredTableWidgetItem(m_MidiProcessor->GetAPIName().c_str()));

                                    MidiDeviceInfoTableWidget->setItem(4, 0, CreateCenteredTableWidgetItem("IEMidi Version:", true));
                                    MidiDeviceInfoTableWidget->setItem(4, 1, CreateCenteredTableWidgetItem("1"));

                                    MidiDeviceInfoTableWidget->setItem(5, 0, CreateCenteredTableWidgetItem("Save File:", true));
                                    MidiDeviceInfoTableWidget->setItem(5, 1, CreateCenteredTableWidgetItem(m_MidiProfileManager ?
                                        m_MidiProfileManager->GetIEMidiProfilesFilePath().string().c_str() : "Invalid Path"));
                                }
                            }
                        }
                    }
                }

                if (QFrame* const MidiLoggerFrameWidget = new QFrame(SideBarFrameWidget))
                {
                    SideBarLayout->addWidget(MidiLoggerFrameWidget);

                    if (QVBoxLayout* const MidiLoggerLayout = new QVBoxLayout(MidiLoggerFrameWidget))
                    {
                        MidiLoggerFrameWidget->setLayout(MidiLoggerLayout);

                        if (IEMidiLoggerTableWidget* const MidiLoggerTableWidget = new IEMidiLoggerTableWidget(m_MidiProcessor->GetMidiLogMessagesBuffer(),
                            MidiLoggerFrameWidget))
                        {
                            m_MidiLoggerTableWidget = MidiLoggerTableWidget;
                            MidiLoggerLayout->addWidget(MidiLoggerTableWidget);
                        }
                    }
                }
            }
        }
    }
}

void IEMidiApp::DrawActiveMidiDeviceEditorFrameWidget(QWidget* Parent)
{
    if (QFrame* const SelectedMidiDeviceEditorFrameWidget = new QFrame(Parent))
    {
        if (QBoxLayout* const ParentLayout = qobject_cast<QBoxLayout*>(Parent->layout()))
        {
            ParentLayout->addWidget(SelectedMidiDeviceEditorFrameWidget, 5);

            if (QVBoxLayout* const SelectedMidiDeviceEditorLayout = new QVBoxLayout(SelectedMidiDeviceEditorFrameWidget))
            {
                SelectedMidiDeviceEditorFrameWidget->setLayout(SelectedMidiDeviceEditorLayout);

                SelectedMidiDeviceEditorLayout->setContentsMargins(60, 0, 60, 0);

                if (m_MidiProcessor)
                {
                    const std::string ActiveMidiDeviceName = m_MidiProcessor->GetActiveMidiDeviceProfile().NameID;
                    if (QLabel* const SelectedMidiDeviceLabel = new QLabel(ActiveMidiDeviceName.c_str(), SelectedMidiDeviceEditorFrameWidget))
                    {
                        SelectedMidiDeviceEditorLayout->addWidget(SelectedMidiDeviceLabel, 1);

                        SelectedMidiDeviceLabel->setStyleSheet("font-size: 22px; font-weight: bold;");
                    }

                    DrawActiveMidiDeviceInputEditorFrameWidget(SelectedMidiDeviceEditorFrameWidget);
                    DrawActiveMidiDeviceOutputEditorFrameWidget(SelectedMidiDeviceEditorFrameWidget);

                    if (QWidget* const Footer = new QWidget(SelectedMidiDeviceEditorFrameWidget))
                    {
                        SelectedMidiDeviceEditorLayout->addWidget(Footer, 1);

                        if (QHBoxLayout* const FooterLayout = new QHBoxLayout(Footer))
                        {
                            FooterLayout->addStretch(1);
                            if (QPushButton* const SaveProfileButton = new QPushButton("Save", Footer))
                            {
                                FooterLayout->addWidget(SaveProfileButton);

                                SaveProfileButton->connect(SaveProfileButton, &QPushButton::pressed, this, &IEMidiApp::SaveActiveMidiDeviceProfile);
                            }

                            if (QPushButton* const ActivateProfileButton = new QPushButton("Activate", Footer))
                            {
                                FooterLayout->addWidget(ActivateProfileButton);

                                ActivateProfileButton->connect(ActivateProfileButton, &QPushButton::pressed, [this, ActiveMidiDeviceName]()
                                {
                                    ActivateMidiDeviceProfile(ActiveMidiDeviceName);
                                    RunInBackground();
                                });
                            }
                        }
                    }
                }
            }
        }
    }
}

void IEMidiApp::DrawActiveMidiDeviceInputEditorFrameWidget(QWidget* Parent)
{
    if (QFrame* const MidiInputEditorFrameWidget = new QFrame(Parent))
    {
        if (QBoxLayout* const ParentLayout = qobject_cast<QBoxLayout*>(Parent->layout()))
        {
            ParentLayout->addWidget(MidiInputEditorFrameWidget, 2);

            if (QVBoxLayout* const MidiInputEditorFrameLayout = new QVBoxLayout(MidiInputEditorFrameWidget))
            {
                MidiInputEditorFrameLayout->setContentsMargins(0, 0, 0, 0);
                MidiInputEditorFrameLayout->setSpacing(10);

                // Input For Loop
                if (m_MidiProcessor && m_MidiProcessor->HasActiveMidiDeviceProfile())
                {
                    IEMidiDeviceInputProperty* MidiDeviceInputProperty = m_MidiProcessor->GetActiveMidiDeviceProfile().InputPropertiesHead.get();
                    while (MidiDeviceInputProperty)
                    {
                        if (IEMidiInputEditorWidget* const MidiInputEditorWidget = new IEMidiInputEditorWidget(*MidiDeviceInputProperty, MidiInputEditorFrameWidget))
                        {
                            MidiInputEditorFrameLayout->addWidget(MidiInputEditorWidget);
                            MidiInputEditorWidget->connect(MidiInputEditorWidget, &IEMidiInputEditorWidget::OnRecording, [this, MidiInputEditorWidget]()
                            {
                                m_MidiListeningWidgets.Push(MidiInputEditorWidget);
                            });
                        }
                        MidiDeviceInputProperty = MidiDeviceInputProperty->Next();
                    }
                }
                // End For loop

                if (QPushButton* const AddInputPropertyButton = new QPushButton("Add Property", MidiInputEditorFrameWidget))
                {
                    MidiInputEditorFrameLayout->addWidget(AddInputPropertyButton);

                    AddInputPropertyButton->setMaximumWidth(120);
                    AddInputPropertyButton->connect(AddInputPropertyButton, &QPushButton::pressed, [=, this]()
                        {
                            if (m_MidiProcessor && m_MidiProcessor->HasActiveMidiDeviceProfile())
                            {
                                IEMidiDeviceProfile& ActiveMidiDeviceProfile = m_MidiProcessor->GetActiveMidiDeviceProfile();
                                IEMidiDeviceInputProperty& NewMidiDeviceInputProperty = ActiveMidiDeviceProfile.MakeInputProperty();

                                if (IEMidiInputEditorWidget* const MidiInputEditorWidget = new IEMidiInputEditorWidget(
                                    NewMidiDeviceInputProperty, MidiInputEditorFrameWidget))
                                {
                                    MidiInputEditorFrameLayout->insertWidget(MidiInputEditorFrameLayout->count() - 2, MidiInputEditorWidget);

                                    MidiInputEditorWidget->connect(MidiInputEditorWidget, &IEMidiInputEditorWidget::OnRecording, [this, MidiInputEditorWidget]()
                                    {
                                        m_MidiListeningWidgets.Push(MidiInputEditorWidget);
                                    });
                                }
                            }
                        });
                }
                MidiInputEditorFrameLayout->addStretch(1);
            }
        }
    }
}

void IEMidiApp::DrawActiveMidiDeviceOutputEditorFrameWidget(QWidget* Parent) const
{
    if (QFrame* const MidiOutputEditorFrameWidget = new QFrame(Parent))
    {
        if (QBoxLayout* const ParentLayout = qobject_cast<QBoxLayout*>(Parent->layout()))
        {
            ParentLayout->addWidget(MidiOutputEditorFrameWidget, 2);

            if (QVBoxLayout* const MidiOutputEditorFrameLayout = new QVBoxLayout(MidiOutputEditorFrameWidget))
            {
                MidiOutputEditorFrameLayout->setContentsMargins(0, 0, 0, 0);
                MidiOutputEditorFrameLayout->setSpacing(10);

                // Output for loop
                if (m_MidiProcessor && m_MidiProcessor->HasActiveMidiDeviceProfile())
                {
                    IEMidiDeviceOutputProperty* MidiDeviceOutputProperty = m_MidiProcessor->GetActiveMidiDeviceProfile().OutputPropertiesHead.get();
                    while (MidiDeviceOutputProperty)
                    {
                        if (IEMidiOutputEditorWidget* const MidiOutputEditorWidget = new IEMidiOutputEditorWidget(*MidiDeviceOutputProperty, MidiOutputEditorFrameWidget))
                        {
                            MidiOutputEditorFrameLayout->addWidget(MidiOutputEditorWidget);

                            MidiOutputEditorWidget->connect(MidiOutputEditorWidget, &IEMidiOutputEditorWidget::OnSendMidiButtonPressed,
                                [this](const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>& MidiMessage)
                                {
                                    if (m_MidiProcessor && m_MidiProcessor->HasActiveMidiDeviceProfile())
                                    {
                                        m_MidiProcessor->SendMidiOutputMessage(MidiMessage);
                                    }
                                });
                        }
                        MidiDeviceOutputProperty = MidiDeviceOutputProperty->Next();
                    }
                }
                // End For loop

                if (QPushButton* const AddOutputPropertyButton = new QPushButton("Add Property", MidiOutputEditorFrameWidget))
                {
                    MidiOutputEditorFrameLayout->addWidget(AddOutputPropertyButton);
                    AddOutputPropertyButton->setMaximumWidth(120);
                    
                    AddOutputPropertyButton->connect(AddOutputPropertyButton, &QPushButton::pressed,
                        [this, MidiOutputEditorFrameWidget, MidiOutputEditorFrameLayout]()
                        {
                            if (m_MidiProcessor && m_MidiProcessor->HasActiveMidiDeviceProfile())
                            {
                                IEMidiDeviceOutputProperty& NewMidiDeviceOutputProperty = m_MidiProcessor->GetActiveMidiDeviceProfile().MakeOutputProperty();

                                if (IEMidiOutputEditorWidget* const MidiOutputEditorWidget = new IEMidiOutputEditorWidget(
                                    NewMidiDeviceOutputProperty, MidiOutputEditorFrameWidget))
                                {
                                    MidiOutputEditorFrameLayout->insertWidget(MidiOutputEditorFrameLayout->count() - 2, MidiOutputEditorWidget);
                                }
                            }
                        });
                }

                MidiOutputEditorFrameLayout->addStretch(1);
            }
        }
    }
}

void IEMidiApp::ResetMainWindowCentralWidget() const
{
    if (m_MainWindow)
    {
        if (QWidget* const CentralWidget = m_MainWindow->centralWidget())
        {
            CentralWidget->hide();
            CentralWidget->deleteLater();
        }
    }
}

void IEMidiApp::RunInBackground() const
{
    if (m_TrayIcon)
    {
        m_TrayIcon->showMessage(
            "IEMidi",                  
            "App is running in the background",
            QSystemTrayIcon::Information,
            5000
        );
    }
    m_MainWindow->hide();
}

void IEMidiApp::ActivateMidiDeviceProfile(const std::string& MidiDeviceName) const
{
    if (m_MidiProcessor && m_MidiProfileManager && m_MidiProcessor->ActivateMidiDeviceProfile(MidiDeviceName))
    {
        IEMidiDeviceProfile& ActiveMidiDeviceProfile = m_MidiProcessor->GetActiveMidiDeviceProfile();
        m_MidiProfileManager->LoadProfile(ActiveMidiDeviceProfile);
        IEMidiDeviceOutputProperty* MidiDeviceOutputProperty = m_MidiProcessor->GetActiveMidiDeviceProfile().OutputPropertiesHead.get();
        while (MidiDeviceOutputProperty)
        {
            m_MidiProcessor->SendMidiOutputMessage(MidiDeviceOutputProperty->MidiMessage);
            MidiDeviceOutputProperty = MidiDeviceOutputProperty->Next();
        }
    }
}

void IEMidiApp::SaveActiveMidiDeviceProfile() const
{
    if (m_MidiProcessor && m_MidiProfileManager)
    {
        if (m_MidiProcessor->HasActiveMidiDeviceProfile())
        {
            m_MidiProfileManager->SaveProfile(m_MidiProcessor->GetActiveMidiDeviceProfile());
        }
    }
}

void IEMidiApp::OnMidiCallback(double Timestamp, const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT> MidiMessage)
{
    QMetaObject::invokeMethod(this, [this]()
        {
            while (!m_MidiListeningWidgets.IsEmpty())
            {
                std::optional<QPointer<QWidget>> MidiDependentWidget = m_MidiListeningWidgets.Pop();
                if (MidiDependentWidget.has_value())
                {
                    if (const QPointer<QWidget> MidiDependentWidgetPtr = MidiDependentWidget.value())
                    {
                        MidiDependentWidgetPtr->repaint();
                    }
                }
            }

            if (m_MidiLoggerTableWidget)
            {
                m_MidiLoggerTableWidget->repaint();
            }
        }, Qt::QueuedConnection);
}
