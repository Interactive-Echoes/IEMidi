// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include "qapplication.h"
#include "qpointer.h"
#include "IEConcurrency.h"

#include "IEMidiProcessor.h"
#include "IEMidiProfileManager.h"
#include "IEMidiTypes.h"

class IEMidiLogger;
class QMainWindow;
class QSystemTrayIcon;
class QWidget;

class IEMidiApp : public QApplication
{
public:
    IEMidiApp(int& Argc, char** Argv);
    ~IEMidiApp();

private:
    void SetupMainWindow();
    void SetupTrayIcon();

private:
    void DrawMidiDeviceSelection();
    void DrawActiveMidiDeviceEditor();

    void DrawActiveMidiDeviceSideBar(QWidget* Parent);
    void DrawActiveMidiDeviceEditor(QWidget* Parent);
    void DrawActiveMidiDeviceInputEditor(QWidget* Parent);
    void DrawActiveMidiDeviceOutputEditor(QWidget* Parent) const;
     
private:
    void ActivateMidiDeviceProfile(const std::string& MidiDeviceName) const;
    void SaveActiveMidiDeviceProfile() const;
    void RunInBackground();

private:
    void OnMidiCallback(double Timestamp, const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT> MidiMessage);

private:
    QPointer<QMainWindow> m_MainWindow;
    QPointer<QSystemTrayIcon> m_SystemTrayIcon;

private:
    const std::unique_ptr<IEMidiProcessor> m_MidiProcessor;
    const std::unique_ptr<IEMidiProfileManager> m_MidiProfileManager;
    
private:
    IESPSCQueue<QPointer<QWidget>> m_MidiListeningWidgets = IESPSCQueue<QPointer<QWidget>>(6);
    QPointer<IEMidiLogger> m_MidiLogger;
    uint32_t m_OnMidiCallbackID = 0;

private:
    inline static const std::string m_IEIconPath = std::string(IEResources_Folder_Path) + "/IE-Brand-Kit/IE-Logo-NoBg.png";
    inline static const std::string m_IEFontPath = std::string(IEResources_Folder_Path) + "/Fonts/Roboto/Roboto-VariableFont_wdth,wght.ttf";
};