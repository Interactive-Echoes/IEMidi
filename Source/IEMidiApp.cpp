// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMidiApp.h"

#include <string>

#include "qboxlayout.h"
#include "qfontdatabase.h"
#include "qframe.h"
#include "qfile.h"
#include "qheaderview.h"
#include "qlabel.h"
#include "qmainwindow.h"
#include "qmenu.h"
#include "qobject.h"
#include "qpushbutton.h"
#include "qsizepolicy.h"
#include "qstylefactory.h"
#include "qsystemtrayicon.h"
#include "qtablewidget.h"

#include "IELog.h"

#include "IEWidgets/IEMidiDeviceInfo.h"
#include "IEWidgets/IEMidiDeviceInputPropertyEditor.h"
#include "IEWidgets/IEMidiLogger.h"
#include "IEWidgets/IEMidiDeviceOutputPropertyEditor.h"

IEMidiApp::IEMidiApp(int& Argc, char** Argv) :
    QApplication(Argc, Argv),
    m_MidiProcessor(std::make_unique<IEMidiProcessor>()),
    m_MidiProfileManager(std::make_unique<IEMidiProfileManager>())
{
    m_OnMidiCallbackID = m_MidiProcessor->AddOnMidiCallback(this, &IEMidiApp::OnMidiCallback);

    const std::string TestFlag = std::string("test");
    for (int i = 0; i < Argc; i++)
    {
        std::string Arg = Argv[i];

        std::transform(Arg.begin(), Arg.end(), Arg.begin(),
            [](unsigned char C)
            {
                return std::tolower(C);
            });

        if (Arg.find(TestFlag) != std::string::npos)
        {
            m_MidiProcessor->SetTestMode(true);
        }
    }

    const std::string& AppStylePath = std::format("{0}/Stylesheets/MainStylesheet.qss", Resources_Folder_Path);
    QFile AppStyle(AppStylePath.c_str());
    if (AppStyle.open(QFile::ReadOnly))
    {
        const QString StyleSheet = QLatin1String(AppStyle.readAll());
        setStyleSheet(StyleSheet);
    }

    QPalette Palette = palette();
    Palette.setColor(QPalette::Window, Qt::black);
    setPalette(Palette);

    setStyle(QStyleFactory::create("windows"));

    if (const int FontID = QFontDatabase::addApplicationFont(m_IEFontPath.c_str()); FontID != -1)
    {
        const QString FontFamily = QFontDatabase::applicationFontFamilies(FontID).at(0);
        QFont GlobalFont(FontFamily);
        GlobalFont.setPixelSize(14);
        GlobalFont.setWeight(QFont::Weight::Medium);
        GlobalFont.setHintingPreference(QFont::PreferFullHinting);
        GlobalFont.setStyleStrategy(QFont::PreferAntialias);
        GlobalFont.setKerning(true);
        GlobalFont.setFixedPitch(false);
        GlobalFont.setLetterSpacing(QFont::AbsoluteSpacing, 1.0f);
        setFont(GlobalFont);
    }

    SetupMainWindow();
    SetupTrayIcon();

    DrawMidiDeviceSelection();
}

IEMidiApp::~IEMidiApp()
{
    if (m_MidiProcessor)
    {
        m_MidiProcessor->RemoveOnMidiCallback(m_OnMidiCallbackID);
    }
}

void IEMidiApp::SetupMainWindow()
{
    m_MainWindow = new QMainWindow();
    if (m_MainWindow)
    {
        m_MainWindow->setWindowTitle("IEMidi");
        m_MainWindow->resize(1080, 960);
        m_MainWindow->setAutoFillBackground(true);
        m_MainWindow->setWindowIcon(QIcon(m_IEIconPath.c_str()));
    }
}

void IEMidiApp::SetupTrayIcon()
{
    if (m_MainWindow)
    {
        m_SystemTrayIcon = new QSystemTrayIcon(m_MainWindow);
        if (m_SystemTrayIcon)
        {
            m_SystemTrayIcon->setIcon(QIcon(m_IEIconPath.c_str()));
            QMenu* const SystemTrayMenu = new QMenu(m_MainWindow);

            QAction* const ShowAction = new QAction("Show", SystemTrayMenu);
            SystemTrayMenu->addAction(ShowAction);
            m_MainWindow->connect(ShowAction, &QAction::triggered, [this]()
                {
                    m_MainWindow->show();
                    DrawActiveMidiDeviceEditor();
                });

            QAction* const QuitAction = new QAction("Quit", SystemTrayMenu);
            SystemTrayMenu->addAction(QuitAction);
            connect(QuitAction, &QAction::triggered, this, &QCoreApplication::quit);

            m_SystemTrayIcon->setContextMenu(SystemTrayMenu);
            m_SystemTrayIcon->show();
        }
    }
}

void IEMidiApp::DrawMidiDeviceSelection()
{
    if (m_MainWindow)
    {
        QWidget* const CentralWidget = new QWidget(m_MainWindow);
        m_MainWindow->setCentralWidget(CentralWidget);

        QHBoxLayout* const CentralLayout = new QHBoxLayout(CentralWidget);
        CentralWidget->setLayout(CentralLayout);

        QFrame* const MidiDeviceSelectionWidget = new QFrame(CentralWidget);
        CentralLayout->addWidget(MidiDeviceSelectionWidget);
        MidiDeviceSelectionWidget->setFrameStyle(QFrame::Panel);
        MidiDeviceSelectionWidget->setMaximumSize(320, 600);
        MidiDeviceSelectionWidget->setObjectName("MidiDeviceSelectionWidget");
        
        QVBoxLayout* const MidiDeviceSelectionLayout = new QVBoxLayout(MidiDeviceSelectionWidget);
        MidiDeviceSelectionWidget->setLayout(MidiDeviceSelectionLayout);
        MidiDeviceSelectionLayout->setSpacing(15);
        MidiDeviceSelectionLayout->setContentsMargins(20, 0, 20, 0);
        MidiDeviceSelectionLayout->setAlignment(Qt::Alignment::enum_type::AlignCenter);

        QLabel* const SelectMIDIDeviceLabel = new QLabel("Select MIDI Device", MidiDeviceSelectionWidget);
        MidiDeviceSelectionLayout->addWidget(SelectMIDIDeviceLabel);
        SelectMIDIDeviceLabel->setStyleSheet("font-size: 24px; font-weight: bold; background-color:none;");
        SelectMIDIDeviceLabel->setAlignment(Qt::AlignCenter);
        MidiDeviceSelectionLayout->addSpacing(20);

        if (m_MidiProcessor)
        {
            const std::vector<std::string>& AvailableMidiDevices = m_MidiProcessor->GetAvailableMidiDevices();
            if (!AvailableMidiDevices.empty())
            {
                for (const std::string& MidiDeviceName : AvailableMidiDevices)
                {
                    QWidget* const MidiDeviceEntryWidget = new QWidget(MidiDeviceSelectionWidget);
                    MidiDeviceSelectionLayout->addWidget(MidiDeviceEntryWidget);
                    MidiDeviceEntryWidget->setObjectName("MidiDeviceEntryWidget");

                    QHBoxLayout* const MidiDeviceEntryLayout = new QHBoxLayout(MidiDeviceEntryWidget);
                    MidiDeviceEntryWidget->setLayout(MidiDeviceEntryLayout);
                    MidiDeviceEntryLayout->setSpacing(8);
                    MidiDeviceEntryLayout->setAlignment(Qt::Alignment::enum_type::AlignCenter);
                    MidiDeviceEntryLayout->setContentsMargins(0, 0, 0, 0);

                    QLabel* const MidiDeviceEntryLabel = new QLabel(MidiDeviceName.c_str(), MidiDeviceEntryWidget);
                    MidiDeviceEntryLayout->addWidget(MidiDeviceEntryLabel);
                    MidiDeviceEntryLayout->addStretch();
                    MidiDeviceEntryLabel->setStyleSheet("font-size: 18px; font-weight: bold;");

                    QPushButton* const MidiDeviceEditButton = new QPushButton(MidiDeviceEntryWidget);
                    MidiDeviceEntryLayout->addWidget(MidiDeviceEditButton);
                    MidiDeviceEditButton->setText("Edit");
                    MidiDeviceEditButton->connect(MidiDeviceEditButton, &QPushButton::pressed, [this, MidiDeviceName]()
                        {
                            ActivateMidiDeviceProfile(MidiDeviceName);
                            DrawActiveMidiDeviceEditor();
                        });
                
                    QPushButton* const MidiDeviceActivateButton = new QPushButton(MidiDeviceEntryWidget);
                    MidiDeviceEntryLayout->addWidget(MidiDeviceActivateButton);
                    MidiDeviceActivateButton->setText("Activate");
                    MidiDeviceActivateButton->connect(MidiDeviceActivateButton, &QPushButton::pressed, [this, MidiDeviceName]()
                        {
                            ActivateMidiDeviceProfile(MidiDeviceName);
                            RunInBackground();
                        });
                }
            }
            else
            {
                QLabel* const SelectMIDIDeviceLabel = new QLabel("No Midi Device Available", MidiDeviceSelectionWidget);
                MidiDeviceSelectionLayout->addWidget(SelectMIDIDeviceLabel);
                SelectMIDIDeviceLabel->setAlignment(Qt::AlignCenter);
                SelectMIDIDeviceLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: rgba(175, 55, 55, 1);");
            }
        }
        m_MainWindow->show();
    }
}

void IEMidiApp::DrawActiveMidiDeviceEditor()
{
    if (m_MainWindow && m_MidiProcessor && m_MidiProcessor->HasActiveMidiDeviceProfile())
    {
        QWidget* const CentralWidget = new QWidget(m_MainWindow);
        m_MainWindow->setCentralWidget(CentralWidget);
        CentralWidget->setObjectName("CentralWidget");

        QHBoxLayout* const CentralLayout = new QHBoxLayout(CentralWidget);
        CentralWidget->setLayout(CentralLayout);
        CentralLayout->setSpacing(0);
        CentralLayout->setContentsMargins(0, 0, 0, 0);

        DrawActiveMidiDeviceSideBar(CentralWidget);
        DrawActiveMidiDeviceEditor(CentralWidget);

        m_MainWindow->show();
    }
}

void IEMidiApp::DrawActiveMidiDeviceSideBar(QWidget* Parent)
{
    if (QBoxLayout* const ParentLayout = qobject_cast<QBoxLayout*>(Parent->layout()))
    {
        QFrame* const SideBarFrame = new QFrame(Parent);
        ParentLayout->addWidget(SideBarFrame);
        SideBarFrame->setObjectName("SidebarFrame");
        SideBarFrame->setFixedWidth(300);

        QVBoxLayout* const SideBarLayout = new QVBoxLayout(SideBarFrame);
        SideBarFrame->setLayout(SideBarLayout);
        SideBarLayout->setContentsMargins(0, 0, 0, 0);
        SideBarLayout->setSpacing(0);

        if (m_MidiProcessor && m_MidiProcessor->HasActiveMidiDeviceProfile())
        {
            IEMidiDeviceInfo* const MidiDeviceInfo = new IEMidiDeviceInfo(*m_MidiProcessor, SideBarFrame);
            SideBarLayout->addWidget(MidiDeviceInfo, 2);
            MidiDeviceInfo->setObjectName("MidiDeviceInfo");

            m_MidiLogger = new IEMidiLogger(m_MidiProcessor->GetMidiLogMessagesBuffer(), SideBarFrame);
            SideBarLayout->addWidget(m_MidiLogger, 3);
            m_MidiLogger->setObjectName("MidiLogger");
        }
    }
}

void IEMidiApp::DrawActiveMidiDeviceEditor(QWidget* Parent)
{
    if (QBoxLayout* const ParentLayout = qobject_cast<QBoxLayout*>(Parent->layout()))
    {
        QFrame* const SelectedMidiDeviceEditorFrame = new QFrame(Parent);
        ParentLayout->addWidget(SelectedMidiDeviceEditorFrame);
        SelectedMidiDeviceEditorFrame->setObjectName("SelectedMidiDeviceEditorFrame");

        QVBoxLayout* const SelectedMidiDeviceEditorLayout = new QVBoxLayout(SelectedMidiDeviceEditorFrame);
        SelectedMidiDeviceEditorFrame->setLayout(SelectedMidiDeviceEditorLayout);
        SelectedMidiDeviceEditorLayout->setContentsMargins(40, 0, 40, 0);
        SelectedMidiDeviceEditorLayout->setSpacing(0);

        if (m_MidiProcessor && m_MidiProcessor->HasActiveMidiDeviceProfile())
        {
            const std::string ActiveMidiDeviceName = m_MidiProcessor->GetActiveMidiDeviceProfile().NameID;

            QLabel* const SelectedMidiDeviceLabel = new QLabel(ActiveMidiDeviceName.c_str(), SelectedMidiDeviceEditorFrame);
            SelectedMidiDeviceEditorLayout->addWidget(SelectedMidiDeviceLabel, 1);
            SelectedMidiDeviceLabel->setAlignment(Qt::AlignCenter);
            SelectedMidiDeviceLabel->setObjectName("SelectedMidiDeviceLabel");

            DrawActiveMidiDeviceInputEditor(SelectedMidiDeviceEditorFrame);
            DrawActiveMidiDeviceOutputEditor(SelectedMidiDeviceEditorFrame);

            QWidget* const SelectedMidiDeviceEditorFooter = new QWidget(SelectedMidiDeviceEditorFrame);
            SelectedMidiDeviceEditorLayout->addWidget(SelectedMidiDeviceEditorFooter, 1);
            SelectedMidiDeviceEditorFooter->setObjectName("SelectedMidiDeviceEditorFooter");

            QHBoxLayout* const FooterLayout = new QHBoxLayout(SelectedMidiDeviceEditorFooter);
            FooterLayout->addStretch(1);
            FooterLayout->setContentsMargins(0, 0, 0, 0);

            QPushButton* const SaveProfileButton = new QPushButton("Save", SelectedMidiDeviceEditorFooter);
            FooterLayout->addWidget(SaveProfileButton);
            SaveProfileButton->setObjectName("SaveProfileButton");
            SaveProfileButton->connect(SaveProfileButton, &QPushButton::pressed, this, &IEMidiApp::SaveActiveMidiDeviceProfile);

            QPushButton* const ActivateProfileButton = new QPushButton("Activate", SelectedMidiDeviceEditorFooter);
            FooterLayout->addWidget(ActivateProfileButton);
            ActivateProfileButton->connect(ActivateProfileButton, &QPushButton::pressed, [this, ActiveMidiDeviceName]()
                {
                    ActivateMidiDeviceProfile(ActiveMidiDeviceName);
                    RunInBackground();
                });
        }
    }
}

void IEMidiApp::DrawActiveMidiDeviceInputEditor(QWidget* Parent)
{
    if (QBoxLayout* const ParentLayout = qobject_cast<QBoxLayout*>(Parent->layout()))
    {
        QFrame* const MidiInputEditorFrame = new QFrame(Parent);
        ParentLayout->addWidget(MidiInputEditorFrame, 3);
        MidiInputEditorFrame->setObjectName("MidiInputEditorFrame");

        QVBoxLayout* const MidiInputEditorLayout = new QVBoxLayout(MidiInputEditorFrame);
        MidiInputEditorLayout->setContentsMargins(0, 0, 0, 0);
        MidiInputEditorLayout->setSpacing(10);

        QLabel* const InputPropertiesLabel = new QLabel("Inputs", MidiInputEditorFrame);
        InputPropertiesLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
        MidiInputEditorLayout->addWidget(InputPropertiesLabel);
        MidiInputEditorLayout->addSpacing(20);

        // Input For Loop
        if (m_MidiProcessor && m_MidiProcessor->HasActiveMidiDeviceProfile())
        {
            IEMidiDeviceInputProperty* MidiDeviceInputProperty = m_MidiProcessor->GetActiveMidiDeviceProfile().InputPropertiesHead.get();
            while (MidiDeviceInputProperty)
            {
                IEMidiDeviceInputPropertyEditor* const MidiDeviceInputPropertyEditor = new IEMidiDeviceInputPropertyEditor(*MidiDeviceInputProperty, MidiInputEditorFrame);
                MidiInputEditorLayout->addWidget(MidiDeviceInputPropertyEditor);
                MidiDeviceInputPropertyEditor->connect(MidiDeviceInputPropertyEditor, &IEMidiDeviceInputPropertyEditor::OnRecording, [this, MidiDeviceInputPropertyEditor]()
                {
                    m_MidiListeningWidgets.Push(MidiDeviceInputPropertyEditor);
                });
                
                MidiDeviceInputProperty = MidiDeviceInputProperty->Next();
            }
        }
        // End For loop

        MidiInputEditorLayout->addSpacing(10);

        QPushButton* const AddInputPropertyButton = new QPushButton("Add Property", MidiInputEditorFrame);
        AddInputPropertyButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        MidiInputEditorLayout->addWidget(AddInputPropertyButton);
        AddInputPropertyButton->connect(AddInputPropertyButton, &QPushButton::pressed, [=, this]()
            {
                if (m_MidiProcessor && m_MidiProcessor->HasActiveMidiDeviceProfile())
                {
                    IEMidiDeviceProfile& ActiveMidiDeviceProfile = m_MidiProcessor->GetActiveMidiDeviceProfile();
                    IEMidiDeviceInputProperty& NewMidiDeviceInputProperty = ActiveMidiDeviceProfile.MakeInputProperty();

                    IEMidiDeviceInputPropertyEditor* const MidiDeviceInputPropertyEditor = new IEMidiDeviceInputPropertyEditor(NewMidiDeviceInputProperty, MidiInputEditorFrame);
                    MidiInputEditorLayout->insertWidget(MidiInputEditorLayout->count() - 3, MidiDeviceInputPropertyEditor);
                    MidiDeviceInputPropertyEditor->connect(MidiDeviceInputPropertyEditor, &IEMidiDeviceInputPropertyEditor::OnRecording, [this, MidiDeviceInputPropertyEditor]()
                    {
                        m_MidiListeningWidgets.Push(MidiDeviceInputPropertyEditor);
                    });
                }
            });
        
        MidiInputEditorLayout->addStretch(1);
    }
}

void IEMidiApp::DrawActiveMidiDeviceOutputEditor(QWidget* Parent) const
{
    if (QBoxLayout* const ParentLayout = qobject_cast<QBoxLayout*>(Parent->layout()))
    {
        QFrame* const MidiOutputEditorFrame = new QFrame(Parent);
        ParentLayout->addWidget(MidiOutputEditorFrame, 3);
        MidiOutputEditorFrame->setObjectName("MidiOutputEditorFrame");

        QVBoxLayout* const MidiOutputEditorLayout = new QVBoxLayout(MidiOutputEditorFrame);
        MidiOutputEditorLayout->setContentsMargins(0, 0, 0, 0);;
        MidiOutputEditorLayout->setSpacing(10);

        QLabel* const OutputPropertiesLabel = new QLabel("Outputs", MidiOutputEditorFrame);
        OutputPropertiesLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
        MidiOutputEditorLayout->addWidget(OutputPropertiesLabel);
        MidiOutputEditorLayout->addSpacing(20);

        // Output for loop
        if (m_MidiProcessor && m_MidiProcessor->HasActiveMidiDeviceProfile())
        {
            IEMidiDeviceOutputProperty* MidiDeviceOutputProperty = m_MidiProcessor->GetActiveMidiDeviceProfile().OutputPropertiesHead.get();
            while (MidiDeviceOutputProperty)
            {
                IEMidiDeviceOutputPropertyEditor* const MidiDeviceOutputPropertyEditor = new IEMidiDeviceOutputPropertyEditor(*MidiDeviceOutputProperty, MidiOutputEditorFrame);
                MidiOutputEditorLayout->addWidget(MidiDeviceOutputPropertyEditor);
                MidiDeviceOutputPropertyEditor->connect(MidiDeviceOutputPropertyEditor, &IEMidiDeviceOutputPropertyEditor::OnSendMidiButtonPressed,
                    [this](const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>& MidiMessage)
                    {
                        if (m_MidiProcessor && m_MidiProcessor->HasActiveMidiDeviceProfile())
                        {
                            m_MidiProcessor->SendMidiOutputMessage(MidiMessage);
                        }
                    });
                
                MidiDeviceOutputProperty = MidiDeviceOutputProperty->Next();
            }
        }
        // End For loop

        MidiOutputEditorLayout->addSpacing(10);

        QPushButton* const AddOutputPropertyButton = new QPushButton("Add Property", MidiOutputEditorFrame);
        MidiOutputEditorLayout->addWidget(AddOutputPropertyButton);
        AddOutputPropertyButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        AddOutputPropertyButton->connect(AddOutputPropertyButton, &QPushButton::pressed,
            [this, MidiOutputEditorFrame, MidiOutputEditorLayout]()
            {
                if (m_MidiProcessor && m_MidiProcessor->HasActiveMidiDeviceProfile())
                {
                    IEMidiDeviceOutputProperty& NewMidiDeviceOutputProperty = m_MidiProcessor->GetActiveMidiDeviceProfile().MakeOutputProperty();
                    IEMidiDeviceOutputPropertyEditor* const MidiDeviceOutputPropertyEditor = new IEMidiDeviceOutputPropertyEditor(NewMidiDeviceOutputProperty, MidiOutputEditorFrame);
                    MidiOutputEditorLayout->insertWidget(MidiOutputEditorLayout->count() - 3, MidiDeviceOutputPropertyEditor);
                    MidiDeviceOutputPropertyEditor->connect(MidiDeviceOutputPropertyEditor, &IEMidiDeviceOutputPropertyEditor::OnSendMidiButtonPressed,
                    [this](const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>& MidiMessage)
                    {
                        if (m_MidiProcessor && m_MidiProcessor->HasActiveMidiDeviceProfile())
                        {
                            m_MidiProcessor->SendMidiOutputMessage(MidiMessage);
                        }
                    });
                }
            });

        MidiOutputEditorLayout->addStretch(1);
    }
}

void IEMidiApp::RunInBackground()
{
    if (m_SystemTrayIcon)
    {
        m_SystemTrayIcon->showMessage(
            "IEMidi",                  
            "App is running in the background",
            QSystemTrayIcon::MessageIcon::NoIcon,
            5000
        );
    }

    while (!m_MidiListeningWidgets.IsEmpty())
    {
        m_MidiListeningWidgets.Pop();
    }

    if (m_MidiProcessor && m_MidiProcessor->HasActiveMidiDeviceProfile())
    {
        IEMidiDeviceInputProperty* MidiDeviceInputProperty = m_MidiProcessor->GetActiveMidiDeviceProfile().InputPropertiesHead.get();
        while (MidiDeviceInputProperty)
        {
            MidiDeviceInputProperty->bIsRecording = false;
            MidiDeviceInputProperty = MidiDeviceInputProperty->Next();
        }
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
    if (m_MidiProcessor && m_MidiProfileManager && m_MidiProcessor->HasActiveMidiDeviceProfile())
    {
        if (m_MidiProfileManager->SaveProfile(m_MidiProcessor->GetActiveMidiDeviceProfile()))
        {
            if (m_SystemTrayIcon)
            {
                m_SystemTrayIcon->showMessage(
                    "IEMidi",                  
                    "Successfully saved profile",
                    QSystemTrayIcon::MessageIcon::NoIcon,
                    2000
                );
            }
        }
        else
        {
            if (m_SystemTrayIcon)
            {
                m_SystemTrayIcon->showMessage(
                    "IEMidi",                  
                    "Failed to save profile",
                    QSystemTrayIcon::MessageIcon::Critical,
                    5000
                );
            }
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

            if (m_MidiLogger)
            {
                m_MidiLogger->repaint();
            }
        }, Qt::QueuedConnection);
}
