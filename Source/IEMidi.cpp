// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMidi.h"

IEMidi::IEMidi() :
#ifdef GLFW_INCLUDE_VULKAN
    m_Renderer(std::make_shared<IERenderer_Vulkan>()),
#endif
    m_MidiProcessor(std::make_shared<IEMidiProcessor>()),
    m_MidiProfileManager(std::make_unique<IEMidiProfileManager>()),
    m_MidiEditor(std::make_unique<IEMidiEditor>(m_MidiProcessor))
{}

IEAppState IEMidi::GetAppState() const
{
    return m_AppState;
}

void IEMidi::SetAppState(IEAppState AppState)
{
    m_AppState = AppState;
    GetRenderer().PostEmptyEvent();
}

void IEMidi::PostRendererInitialized()
{
    if (m_Renderer)
    {
        const uint32_t AppWindowID = m_Renderer->GetAppWindowID();
        m_Renderer->AddOnWindowCloseCallbackFunc(AppWindowID, std::bind(&IEMidi::OnAppWindowClosed, this, std::placeholders::_1));
        m_Renderer->AddOnWindowRestoreCallbackFunc(AppWindowID, std::bind(&IEMidi::OnAppWindowRestored, this, std::placeholders::_1));
    }
}

void IEMidi::OnPreFrameRender()
{
    switch (m_AppState)
    {
        case IEAppState::MidiDeviceSelection:
        {
            DrawMidiDeviceSelectionWindow();
            break;
        }
        case IEAppState::MidiDeviceEditor:
        {
            DrawSideBar();
            DrawSelectedMidiDeviceEditorWindow();
            break;
        }
        default:
        {
            break;
        }
    }
}

void IEMidi::OnPostFrameRender()
{}

void IEMidi::DrawMidiDeviceSelectionWindow()
{
    static constexpr uint32_t WindowFlags = ImGuiWindowFlags_NoResize |
                                            ImGuiWindowFlags_NoMove |
                                            ImGuiWindowFlags_NoCollapse |
                                            ImGuiWindowFlags_NoTitleBar;
                                            
    ImGuiViewport& MainViewport = *ImGui::GetMainViewport();
    const float WindowWidth = 250.0f;
    const float WindowHeight = MainViewport.Size.y * 0.5f;
    const float WindowPosX = (MainViewport.Pos.x + (MainViewport.Size.x - WindowWidth) * 0.5f);
    const float WindowPosY = (MainViewport.Pos.y + (MainViewport.Size.y - WindowHeight) * 0.5f);

    ImGui::SetNextWindowSize(ImVec2(WindowWidth, WindowHeight), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(WindowPosX, WindowPosY));

    ImGui::Begin("Select Midi Device", nullptr, WindowFlags);

    ImGui::SetSmartCursorPosYRelative(0.04f);
    ImGui::PushFont(ImGui::IEStyle::GetSubtitleFont());
    ImGui::CenteredText("Select MIDI Device");
    ImGui::PopFont();

    IEMidiProcessor& MidiProcessor = GetMidiProcessor();
    const std::vector<std::string> AvailableMidiDevices = MidiProcessor.GetAvailableMidiDevices();
    if (!AvailableMidiDevices.empty())
    {
        for (const std::string& MidiDeviceName : AvailableMidiDevices)
        {
            ImGui::PushID(MidiDeviceName.c_str());

            ImGui::PushFont(ImGui::IEStyle::GetSubtitleFont());
            ImGui::SetSmartCursorPosYRelative(0.2f);
            ImGui::PushTextWrapPos();
            ImGui::CenteredText("%s", MidiDeviceName.c_str());
            ImGui::PopTextWrapPos();
            ImGui::PopFont();
            ImGui::Separator();

            ImGui::SetSmartCursorPosY(80.0f);
            static const char ActivateText[] = "Activate";
            ImGui::SetSmartCursorPosX(WindowWidth * 0.5f - ImGui::IEStyle::GetDefaultButtonSize().x - 8.0f);
            if (ImGui::IEStyle::DefaultButton(ActivateText))
            {
                if (MidiProcessor.ActivateMidiDeviceProfile(MidiDeviceName))
                {
                    IEMidiDeviceProfile& ActiveMidiDeviceProfile = MidiProcessor.GetActiveMidiDeviceProfile();
                    GetMidiProfileManager().LoadProfile(ActiveMidiDeviceProfile);
                    for (const std::vector<unsigned char>& MidiMessage : ActiveMidiDeviceProfile.InitialOutputMidiMessages)
                    {
                        MidiProcessor.SendMidiOutputMessage(MidiMessage);
                    }

                    IERenderer& Renderer = GetRenderer();
                    if (Renderer.SupportsRunInBackground())
                    {
                        Renderer.CloseAppWindow();
                    }
                    else
                    {
                        Renderer.MinimizeAppWindow();
                    }
                }
            }

            ImGui::SameLine();

            static const char EditText[] = "Edit";
            ImGui::SetSmartCursorPosX(WindowWidth * 0.5f + 8.0f);
            if (ImGui::IEStyle::DefaultButton(EditText))
            {
                if (MidiProcessor.ActivateMidiDeviceProfile(MidiDeviceName))
                {
                    IEMidiDeviceProfile& ActiveMidiDeviceProfile = MidiProcessor.GetActiveMidiDeviceProfile();
                    GetMidiProfileManager().LoadProfile(ActiveMidiDeviceProfile);
                    for (const std::vector<unsigned char>& MidiMessage : ActiveMidiDeviceProfile.InitialOutputMidiMessages)
                    {
                        MidiProcessor.SendMidiOutputMessage(MidiMessage);
                    }

                    SetAppState(IEAppState::MidiDeviceEditor);
                }
            }

            ImGui::NewLine();

            ImGui::PopID();
        }
    }
    else 
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));
        ImGui::PushFont(ImGui::IEStyle::GetSubtitleFont());
        ImGui::SetSmartCursorPosYRelative(0.1f);
        ImGui::CenteredText("No Devices Found");
        ImGui::PopFont();
        ImGui::PopStyleColor();
    }

    ImGui::End();
}

void IEMidi::DrawSelectedMidiDeviceEditorWindow()
{
    static constexpr uint32_t WindowFlags = ImGuiWindowFlags_NoResize |
                                            ImGuiWindowFlags_NoMove |
                                            ImGuiWindowFlags_NoCollapse |
                                            ImGuiWindowFlags_NoTitleBar;

    ImGuiViewport& MainViewport = *ImGui::GetMainViewport();
    const float WindowWidth = MainViewport.Size.x * 0.75f - m_WindowOffsetAbs * 0.5f;
    const float WindowHeight = MainViewport.Size.y - m_WindowOffsetAbs;
    const float WindowPosX = MainViewport.Pos.x + (MainViewport.Size.x - WindowWidth) - m_WindowOffsetAbs * 0.5f;
    const float WindowPosY = MainViewport.Pos.y + ((MainViewport.Size.y - WindowHeight) * 0.5f);
    ImGui::SetNextWindowSize(ImVec2(WindowWidth, WindowHeight), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(WindowPosX, WindowPosY));

    IEMidiProcessor& MidiProcessor = GetMidiProcessor();
    IEMidiDeviceProfile& ActiveMidiDeviceProfile = MidiProcessor.GetActiveMidiDeviceProfile();

    const std::string WindowLabel = std::format("Editing {}", ActiveMidiDeviceProfile.Name);

    ImGui::Begin(WindowLabel.c_str(), nullptr, WindowFlags);

    m_MidiEditor->DrawMidiDeviceProfileEditor(ActiveMidiDeviceProfile);

    static const char SaveAndClose[] = "Save & Close";
    const ImVec2 CloseSelectableSize = ImGui::CalcTextSize(SaveAndClose);
    ImGui::SetSmartCursorPos(ImVec2(WindowWidth - CloseSelectableSize.x - 40.0f, WindowHeight - CloseSelectableSize.y - 40.0f));
    if (ImGui::IEStyle::GreenButton(SaveAndClose))
    {
        if (GetMidiProfileManager().SaveProfile(ActiveMidiDeviceProfile))
        {
            MidiProcessor.DeactivateMidiDeviceProfile();
            SetAppState(IEAppState::MidiDeviceSelection);
        }
    }

    ImGui::End();
}

void IEMidi::DrawSideBar()
{
    static constexpr uint32_t WindowFlags = ImGuiWindowFlags_NoResize |
                                            ImGuiWindowFlags_NoMove |
                                            ImGuiWindowFlags_NoCollapse |
                                            ImGuiWindowFlags_NoTitleBar;

    ImGuiViewport& MainViewport = *ImGui::GetMainViewport();
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::IEStyle::Colors::SideBarBgColor);

    /* Begin Midi Device Info */

    const float MidiDeviceInfoWindowWidth = MainViewport.Size.x * 0.25f - m_WindowOffsetAbs;
    const float MidiDeviceInfoWindowHeight = MainViewport.Size.y * 0.5f - m_WindowOffsetAbs * 0.75f;
    const float MidiDeviceInfoWindowPosX = MainViewport.Pos.x + m_WindowOffsetAbs * 0.5f;
    const float MidiDeviceInfoWindowPosY = MainViewport.Pos.y + m_WindowOffsetAbs * 0.5f;
    ImGui::SetNextWindowSize(ImVec2(MidiDeviceInfoWindowWidth, MidiDeviceInfoWindowHeight), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(MidiDeviceInfoWindowPosX, MidiDeviceInfoWindowPosY));
    
    ImGui::Begin("MidiDeviceInfoWindow", nullptr, WindowFlags);

    const IEMidiProcessor& MidiProcessor = GetMidiProcessor();
    RtMidiIn& MidiIn = MidiProcessor.GetMidiIn();

    ImGui::PushFont(ImGui::IEStyle::GetTitleFont());
    ImGui::WindowPositionedText(0.5f, 0.035f, "Midi Device Info");
    ImGui::PopFont();

    ImGui::SetSmartCursorPosYRelative(0.15f);
    static constexpr int MidiDeviceInfoColumnsNum = 2;
    const float MidiDeviceInfoColumnWidth = MidiDeviceInfoWindowWidth / 2.75f;
    const float MidiDeviceInfoTableWidth = MidiDeviceInfoColumnsNum * MidiDeviceInfoColumnWidth;
    const float MidiDeviceInfoTableStartCursor = MidiDeviceInfoWindowWidth - MidiDeviceInfoTableWidth;

    ImGui::SetSmartCursorPosX(MidiDeviceInfoTableStartCursor * 0.4f);

    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::IEStyle::Colors::SecondaryTextColor);
    if (ImGui::BeginTable("##MidiDeviceInfoTable", MidiDeviceInfoColumnsNum, ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_NoHostExtendX))
    {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, MidiDeviceInfoColumnWidth);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, MidiDeviceInfoColumnWidth);
        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::PushFont(ImGui::IEStyle::GetBoldFont());
        ImGui::Text("Name:");
        ImGui::PopFont();
        ImGui::TableNextColumn();
        ImGui::Text("%s", MidiProcessor.HasActiveMidiDeviceProfile() ? MidiProcessor.GetActiveMidiDeviceProfile().Name.c_str() : "No Active Midi Profile");

        ImGui::TableNextColumn();
        ImGui::PushFont(ImGui::IEStyle::GetBoldFont());
        ImGui::Text("Input Port:");
        ImGui::PopFont();
        ImGui::TableNextColumn();
        ImGui::Text("%s", MidiProcessor.HasActiveMidiDeviceProfile() ?
            std::to_string(MidiProcessor.GetActiveMidiDeviceProfile().GetInputPortNumber()).c_str() : "No Active Midi Profile");

        ImGui::TableNextColumn();
        ImGui::PushFont(ImGui::IEStyle::GetBoldFont());
        ImGui::Text("Output Port:");
        ImGui::PopFont();
        ImGui::TableNextColumn();
        ImGui::Text("%s", MidiProcessor.HasActiveMidiDeviceProfile() ?
            std::to_string(MidiProcessor.GetActiveMidiDeviceProfile().GetOutputPortNumber()).c_str() : "No Active Midi Profile");

        ImGui::TableNextColumn();
        ImGui::PushFont(ImGui::IEStyle::GetBoldFont());
        ImGui::Text("Current API:");
        ImGui::PopFont();
        ImGui::TableNextColumn();
        ImGui::TextWrapped("%s", MidiIn.getApiDisplayName(MidiIn.getCurrentApi()).c_str());

        ImGui::TableNextColumn();
        ImGui::PushFont(ImGui::IEStyle::GetBoldFont());
        ImGui::Text("RtMidi Version:");
        ImGui::PopFont();
        ImGui::TableNextColumn();
        ImGui::Text("%s", MidiIn.getVersion().c_str());

        ImGui::TableNextColumn();
        ImGui::PushFont(ImGui::IEStyle::GetBoldFont());
        ImGui::Text("Save File:");
        ImGui::PopFont();
        ImGui::TableNextColumn();
        ImGui::TextWrapped("%s", GetMidiProfileManager().GetIEMidiProfilesFilePath().string().c_str());
        ImGui::TableNextColumn();

        ImGui::EndTable();
    }
    ImGui::PopStyleColor();
    ImGui::End();

    /* End Midi Device Info */
    
    /* Begin Midi Logger */

    const float MidiLoggerWindowWidth = MainViewport.Size.x * 0.25f - m_WindowOffsetAbs;
    const float MidiLoggerWindowHeight = MainViewport.Size.y * 0.5f - m_WindowOffsetAbs * 0.75f;
    const float MidiLoggerWindowPosX = MainViewport.Pos.x + m_WindowOffsetAbs * 0.5f;
    const float MidiLoggerWindowPosY = MainViewport.Pos.y + (MainViewport.Size.y - MidiLoggerWindowHeight -  m_WindowOffsetAbs * 0.5f);
    ImGui::SetNextWindowSize(ImVec2(MidiLoggerWindowWidth, MidiLoggerWindowHeight), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(MidiLoggerWindowPosX, MidiLoggerWindowPosY));
    
    ImGui::Begin("MidiLoggerWindow", nullptr, WindowFlags);
    
    ImGui::PushFont(ImGui::IEStyle::GetTitleFont());
    ImGui::WindowPositionedText(0.5f, 0.035f, "Midi Logger");
    ImGui::PopFont();

    ImGui::SetSmartCursorPosYRelative(0.15f);
    static constexpr int MidiLoggerColumnsNum = 3;
    const float MidiLoggerColumnWidth = MidiLoggerWindowWidth / 4.0f;
    const float MidiLoggerTableWidth = MidiLoggerColumnsNum * MidiLoggerColumnWidth;
    const float MidiLoggerTableStartCursor = MidiLoggerWindowWidth - MidiLoggerTableWidth;

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.650f, 0.765f, 1.000f, 0.900f));

    ImGui::PushFont(ImGui::IEStyle::GetBoldFont());
    
    static constexpr char StatusText[] = "Status";
    ImGui::SetSmartCursorPosX(MidiLoggerTableStartCursor - ImGui::CalcTextSize(StatusText).x * 0.5f);
    ImGui::Text(StatusText);

    ImGui::SameLine();
    static constexpr char Data1Text[] = "Data 1";
    ImGui::SetSmartCursorPosX(MidiLoggerTableStartCursor + MidiLoggerColumnWidth - ImGui::CalcTextSize(Data1Text).x * 0.5f);
    ImGui::Text(Data1Text);

    ImGui::SameLine();
    static constexpr char Data2Text[] = "Data 2";
    ImGui::SetSmartCursorPosX(MidiLoggerTableStartCursor + MidiLoggerColumnWidth * 2.0f - ImGui::CalcTextSize(Data2Text).x * 0.5f);
    ImGui::Text(Data2Text);

    ImGui::PopFont();

    ImGui::SetSmartCursorPosX(MidiLoggerTableStartCursor * 0.5f);
    if (ImGui::BeginTable("##MidiLoggerTable", MidiLoggerColumnsNum, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoHostExtendX))
    {
        ImGui::PushFont(ImGui::IEStyle::GetBoldFont());

        ImGui::TableSetupColumn("StatusColumn", ImGuiTableColumnFlags_WidthStretch, MidiLoggerColumnWidth);
        ImGui::TableSetupColumn("Data1Column", ImGuiTableColumnFlags_WidthStretch, MidiLoggerColumnWidth);
        ImGui::TableSetupColumn("Data2Column", ImGuiTableColumnFlags_WidthStretch, MidiLoggerColumnWidth);
        ImGui::TableNextRow();

        std::deque<std::vector<unsigned char>> Copy = GetMidiProcessor().GetIncomingMidiMessages();
        while (!Copy.empty())
        {
            if (ImGui::GetCursorPosY() > MidiLoggerWindowHeight - ImGui::TableGetHeaderRowHeight() * 2.0f)
            {
                break;
            }

            const std::vector<unsigned char>& MidiMessage = Copy.front();
            for (int i = 0; i < MidiMessage.size(); i++)
            {
                const unsigned char& Byte = MidiMessage[i];
                ImGui::TableNextColumn();
                const std::string ByteString = std::to_string(static_cast<int>(Byte));
                ImGui::SetSmartCursorPosX(MidiLoggerTableStartCursor + MidiLoggerColumnWidth * static_cast<float>(i) - ImGui::CalcTextSize(ByteString.c_str()).x * 0.5f);
                ImGui::Text("%s", ByteString.c_str());
            }
            Copy.pop_front();
        }
        ImGui::PopFont();
        ImGui::EndTable();
    }

    ImGui::PopStyleColor();
    ImGui::End();

    /* End Midi Logger */

    ImGui::PopStyleColor();
}

void IEMidi::OnAppWindowClosed(uint32_t WindowID)
{
    SetAppState(IEAppState::Background);
}

void IEMidi::OnAppWindowRestored(uint32_t WindowID)
{
    GetMidiProcessor().DeactivateMidiDeviceProfile();
    SetAppState(IEAppState::MidiDeviceSelection);
}