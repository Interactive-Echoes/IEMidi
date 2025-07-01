// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include "QApplication.h"
#include "QMainWindow.h"
#include "QPointer.h"

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
    IEMidiProcessor& GetMidiProcessor() const { return *m_MidiProcessor; }
    IEMidiProfileManager& GetMidiProfileManager() const { return *m_MidiProfileManager; }

public:
    IEAppState GetAppState() const;
    void SetAppState(IEAppState AppState);

    void OnAppStateChanged();

private:
    void DrawMidiDeviceSelectionWindow();
    void DrawSelectedMidiDeviceEditorWindow();

    void DrawSelectedMidiDeviceInputEditorFrameWidget(QWidget* Parent);
    void DrawSelectedMidiDeviceOutputEditorFrameWidget(QWidget* Parent);
    void DrawSideBar(QWidget* Parent);

    void ResetMainWindowCentralWidget();

private Q_SLOTS:
    void OnMidiDeviceEditButtonPressed(const std::string& MidiDeviceName);
    void OnMidiDeviceActivateButtonPressed(const std::string& MidiDeviceName);

private:
    const std::unique_ptr<QMainWindow> m_MainWindow;
    const std::shared_ptr<IEMidiProcessor> m_MidiProcessor;
    const std::unique_ptr<IEMidiProfileManager> m_MidiProfileManager;

private:
    std::vector<QPointer<QWidget>> m_MidiDependentWidgets;

private:
    IEAppState m_AppState = IEAppState::None;
};