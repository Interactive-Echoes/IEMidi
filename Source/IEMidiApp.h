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

class IEMidiLoggerTableFrameWidget;
class QMainWindow;
class QSystemTrayIcon;
class QWidget;

class IEMidiApp : public QApplication
{
public:
    IEMidiApp(int& Argc, char** Argv);
    ~IEMidiApp();

private:
    void SetupMainWindow() const;
    void SetupTrayIcon();

private:
    void DrawMidiDeviceSelection();
    void DrawActiveMidiDeviceEditor();

    void DrawActiveMidiDeviceSideBarFrameWidget(QWidget* Parent);
    void DrawActiveMidiDeviceEditorFrameWidget(QWidget* Parent);
    void DrawActiveMidiDeviceInputEditorFrameWidget(QWidget* Parent);
    void DrawActiveMidiDeviceOutputEditorFrameWidget(QWidget* Parent) const;
     
private:
    void ActivateMidiDeviceProfile(const std::string& MidiDeviceName) const;
    void SaveActiveMidiDeviceProfile() const;
    void RunInBackground();

private:
    void OnMidiCallback(double Timestamp, const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT> MidiMessage);

private:
    const QPointer<QMainWindow> m_MainWindow;
    const QPointer<QSystemTrayIcon> m_SystemTrayIcon;

private:
    const std::unique_ptr<IEMidiProcessor> m_MidiProcessor;
    const std::unique_ptr<IEMidiProfileManager> m_MidiProfileManager;
    
private:
    IESPSCQueue<QPointer<QWidget>> m_MidiListeningWidgets = IESPSCQueue<QPointer<QWidget>>(6);
    QPointer<IEMidiLoggerTableFrameWidget> m_MidiLoggerTableFrameWidget;
    uint32_t m_OnMidiCallbackID = 0;

private:
    const std::string m_IEIconPath = std::format("{0}/IE-Brand-Kit/IE-Logo-NoBg.png", IEResources_Folder_Path); // compile time definition
};