// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include "QApplication.h"
#include "QMainWindow.h"
#include "QPointer.h"
#include "IEConcurrency.h"

#include "IEMidiProcessor.h"
#include "IEMidiProfileManager.h"
#include "IEMidiTypes.h"

class QWidget;

enum class IEAppState : uint16_t
{
    Loading,
    MidiDeviceSelection,
    MidiDeviceEditor,
    Background,
    None
};

class IEMidiApp : public QApplication
{
public:
    IEMidiApp(int& Argc, char** Argv);

public:
    IEAppState GetAppState() const;
    void SetAppState(IEAppState AppState);

    void OnAppStateChanged();

private:
    void DrawMidiDeviceSelectionWindow();
    void DrawActiveMidiDeviceEditorWindow();

    void DrawActiveMidiDeviceInputEditorFrameWidget(QWidget* Parent);
    void DrawActiveMidiDeviceOutputEditorFrameWidget(QWidget* Parent);
    void DrawActiveMidiDeviceSideBar(QWidget* Parent);

    void ResetMainWindowCentralWidget();

private:
    void ActivateMidiDeviceProfile(const std::string& MidiDeviceName);
    void SaveActiveMidiDeviceProfile() const;

private:
    const std::unique_ptr<QMainWindow> m_MainWindow;
    const std::unique_ptr<IEMidiProcessor> m_MidiProcessor;
    const std::unique_ptr<IEMidiProfileManager> m_MidiProfileManager;

private:
    IESPSCQueue<QPointer<QWidget>> m_MidiDependentActiveWidgets = IESPSCQueue<QPointer<QWidget>>(6);

private:
    IEAppState m_AppState = IEAppState::None;
};