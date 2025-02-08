// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include "imgui.h"
#include "Extensions/ie.imgui.h"

#include "IECore.h"

#include "IEMidiEditor.h"
#include "IEMidiProcessor.h"
#include "IEMidiProfileManager.h"
#include "IEMidiTypes.h"

enum class IEAppState : uint16_t
{
    Loading,
    MidiDeviceSelection,
    MidiDeviceEditor,
    Background,
    None
};

class IEMidi
{
public:
    IEMidi();

public:
    IERenderer& GetRenderer() const { return *m_Renderer; }
    IEMidiProcessor& GetMidiProcessor() const { return *m_MidiProcessor; }
    IEMidiProfileManager& GetMidiProfileManager() const { return *m_MidiProfileManager; }
    IEMidiEditor& GetMidiEditor() const { return *m_MidiEditor; }

public:
    IEAppState GetAppState() const;
    void SetAppState(IEAppState AppState);

    void PostRendererInitialized();
    void OnPreFrameRender();
    void OnPostFrameRender();

private:
    void DrawMidiDeviceSelectionWindow();
    void DrawSelectedMidiDeviceEditorWindow();
    void DrawSideBar();

private:
    void OnAppWindowClosed(uint32_t WindowID);
    void OnAppWindowRestored(uint32_t WindowID);

private:
    std::shared_ptr<IERenderer> m_Renderer;
    std::shared_ptr<IEMidiProcessor> m_MidiProcessor;
    std::unique_ptr<IEMidiProfileManager> m_MidiProfileManager;
    std::unique_ptr<IEMidiEditor> m_MidiEditor;

private:
    IEAppState m_AppState = IEAppState::None;
    float m_WindowOffsetAbs = 30.0f;
};