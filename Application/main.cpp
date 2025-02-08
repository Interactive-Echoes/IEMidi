// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMidi.h"

int main()
{
    IEMidi IEMidiApp;
    IEMidiApp.SetAppState(IEAppState::Loading);

    IERenderer& Renderer = IEMidiApp.GetRenderer();
    if (Renderer.Initialize("IEMidi", true))
    {
        IEMidiApp.PostRendererInitialized();
        if (ImGuiContext* const CreatedImGuiContext = ImGui::CreateContext())
        {
            ImGuiIO& IO = ImGui::GetIO();
            IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_IsSRGB;
            if (Renderer.PostImGuiContextCreated())
            {
                ImGui::IEStyle::StyleIE();
                IO.IniFilename = nullptr;
                IO.LogFilename = nullptr;

                IEMidiApp.SetAppState(IEAppState::MidiDeviceSelection);

                IEClock::time_point StartFrameTime = IEClock::now();
                IEDurationMs CapturedDeltaTime = IEDurationMs::zero();

                while (Renderer.IsAppRunning())
                {
                    StartFrameTime = IEClock::now();

                    Renderer.CheckAndResizeSwapChain();
                    Renderer.NewFrame();
                    ImGui::NewFrame();

                    IEMidiApp.OnPreFrameRender();
                        
                    ImGui::Render();
                    Renderer.RenderFrame(*ImGui::GetDrawData());
                    Renderer.PresentFrame();

                    IEMidiApp.OnPostFrameRender();

                    CapturedDeltaTime = std::chrono::duration_cast<IEDurationMs>(IEClock::now() - StartFrameTime);
                    if (Renderer.IsAppWindowOpen())
                    {
                        Renderer.WaitEventsTimeout(0.1f);
                    }
                    else
                    {
                        Renderer.WaitEvents();
                    }
                }
            }
        }

        Renderer.Deinitialize();
    }

    return 0;
}