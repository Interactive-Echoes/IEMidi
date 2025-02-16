// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMidiEditor.h"

static constexpr size_t MidiDevicePropertyEditorColumnCount = 7;
static constexpr size_t MidiDeviceInitialOutputMessageEditorColumnCount = 3;
static constexpr float InputBoxSizeWidth = 150.0f;

void IEMidiEditor::DrawMidiDeviceProfileEditor(IEMidiDeviceProfile& MidiDeviceProfile) const
{
    ImGui::PushFont(ImGui::IEStyle::GetTitleFont());
    ImGui::WindowPositionedText(0.5f, 0.1f, "%s", MidiDeviceProfile.Name.c_str());
    ImGui::PopFont();

    ImGui::PushFont(ImGui::IEStyle::GetSubtitleFont());
    ImGui::WindowPositionedText(0.02f, 0.2f, "Input Editor");
    ImGui::PopFont();

    ImGui::SetSmartCursorPosXRelative(0.02f);
    if (ImGui::BeginTable("Profile Midi Input Editor", 1))
    {
        for (std::vector<IEMidiDeviceProperty>::iterator It = MidiDeviceProfile.Properties.begin();
            It != MidiDeviceProfile.Properties.end();)
        {
            ImGui::PushID(&*It);
            ImGui::TableNextColumn();
            bool bDeleteRequested = false;
            DrawMidiDevicePropertyEditor(*It, bDeleteRequested);
            It = bDeleteRequested ? MidiDeviceProfile.Properties.erase(It) : It+1;
            ImGui::PopID();
        }
        MidiDeviceProfile.Properties.shrink_to_fit();

        ImGui::TableNextColumn();
        ImGui::PushFont(ImGui::IEStyle::GetSubtitleFont());
        if (ImGui::IEStyle::SquareButton("+"))
        {
            MidiDeviceProfile.Properties.push_back(IEMidiDeviceProperty(MidiDeviceProfile.Name));
        }
        ImGui::PopFont();

        ImGui::EndTable();
    }

    ImGui::PushFont(ImGui::IEStyle::GetSubtitleFont());
    ImGui::WindowPositionedText(0.02f, 0.6f, "Output Editor");
    ImGui::PopFont();

    ImGui::SetSmartCursorPosXRelative(0.02f);
    if (ImGui::BeginTable("Profile Output Midi Message Editor", 1))
    {
        for (std::vector<std::vector<unsigned char>>::iterator It = MidiDeviceProfile.InitialOutputMidiMessages.begin();
            It != MidiDeviceProfile.InitialOutputMidiMessages.end();)
        {
            ImGui::PushID(&*It);
            ImGui::TableNextColumn();
            bool bDeleteRequested = false;
            DrawInitialOutputMessageEditor(*It, bDeleteRequested);
            It = bDeleteRequested ? MidiDeviceProfile.InitialOutputMidiMessages.erase(It) : It+1;
            ImGui::PopID();
        }
        MidiDeviceProfile.InitialOutputMidiMessages.shrink_to_fit();

        ImGui::TableNextColumn();
        ImGui::PushFont(ImGui::IEStyle::GetSubtitleFont());
        if (ImGui::IEStyle::SquareButton("+"))
        {
            MidiDeviceProfile.InitialOutputMidiMessages.push_back(std::vector<unsigned char>(MIDI_MESSAGE_BYTE_COUNT));
        }
        ImGui::PopFont();

        ImGui::EndTable();
    }
}

void IEMidiEditor::DrawMidiDevicePropertyEditor(IEMidiDeviceProperty& MidiDeviceProperty, bool& bDeleteRequested) const
{
    if (ImGui::BeginTable("Midi Property Editor", MidiDevicePropertyEditorColumnCount, ImGuiTableFlags_Hideable | ImGuiTableFlags_SizingFixedFit))
    {
        static const char MessageTypesStringArray[static_cast<int>(IEMidiMessageType::Count)][std::size("-Select Message Type")] =
        {   "-Select Message Type",
            "NoteOnOff",
            "ControlChange" };
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(ImGui::CalcTextSize("-Select Message Type").x * 1.3f);
        if (ImGui::BeginCombo("##1", MessageTypesStringArray[static_cast<int>(MidiDeviceProperty.MidiMessageType)]))
        {
            for (int i = 0; i < std::size(MessageTypesStringArray); i++)
            {
                if (ImGui::Selectable(MessageTypesStringArray[i]))
                {
                    MidiDeviceProperty.MidiMessageType = static_cast<IEMidiMessageType>(i);
                }
            }

            ImGui::EndCombo();
        }

        if (MidiDeviceProperty.MidiMessageType == IEMidiMessageType::NoteOnOff)
        {
            ImGui::SameLine();
            ImGui::Checkbox("Toggle", &MidiDeviceProperty.bToggle);
        }

        static const char ActionTypesStringArray[static_cast<int>(IEMidiActionType::Count)][std::size("-Select Action Type")] =
        {   "-Select Action Type",
            "Volume",
            "Mute",
            "Console Command",
            "Open File" };
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(ImGui::CalcTextSize("-Select Action Type").x * 1.3f);
        if (ImGui::BeginCombo("##2", ActionTypesStringArray[static_cast<int>(MidiDeviceProperty.MidiActionType)]))
        {
            for (int i = 0; i < std::size(ActionTypesStringArray); i++)
            {
                if (ImGui::Selectable(ActionTypesStringArray[i]))
                {
                    MidiDeviceProperty.MidiActionType = static_cast<IEMidiActionType>(i);
                }
            }

            ImGui::EndCombo();
        }

        switch (MidiDeviceProperty.MidiActionType)
        {
            case IEMidiActionType::ConsoleCommand:
            {
                ImGui::TableNextColumn();
                ImGui::TableSetColumnEnabled(-1, true);
                char Buffer[256] = {0}; 
                std::strncpy(Buffer, MidiDeviceProperty.ConsoleCommand.c_str(), sizeof(Buffer) - 1);
                Buffer[sizeof(Buffer) - 1] = '\0';
                ImGui::SetNextItemWidth(InputBoxSizeWidth);
                ImGui::InputText("##Input Console Command", Buffer, std::size(Buffer));
                MidiDeviceProperty.ConsoleCommand = Buffer;
                ImGui::TableNextColumn();
                ImGui::TableSetColumnEnabled(-1, false);
                MidiDeviceProperty.OpenFilePath.clear();
                break;
            }
            case IEMidiActionType::OpenFile:
            {
                MidiDeviceProperty.MidiMessageType = IEMidiMessageType::NoteOnOff;

                ImGui::TableNextColumn();
                ImGui::TableSetColumnEnabled(-1, false);
                MidiDeviceProperty.ConsoleCommand.clear();
                ImGui::TableNextColumn();
                ImGui::TableSetColumnEnabled(-1, true);
                char Buffer[256] = {0}; 
                std::strncpy(Buffer, MidiDeviceProperty.OpenFilePath.c_str(), sizeof(Buffer) - 1);
                Buffer[sizeof(Buffer) - 1] = '\0';
                ImGui::SetNextItemWidth(InputBoxSizeWidth);
                ImGui::InputText("##Input Open File Path", Buffer, std::size(Buffer));
                MidiDeviceProperty.OpenFilePath = Buffer;

                ImGui::SameLine();
                ImGui::FileFinder("Find File", 3, MidiDeviceProperty.OpenFilePath);
                break;
            }
            default:
            {
                ImGui::TableNextColumn();
                ImGui::TableSetColumnEnabled(-1, false);
                MidiDeviceProperty.ConsoleCommand.clear();

                ImGui::TableNextColumn();
                ImGui::TableSetColumnEnabled(-1, false);
                MidiDeviceProperty.OpenFilePath.clear();
                break;
            }
        }
  
        ImGui::TableNextColumn();
        ImGui::SetSmartCursorPosXRelative(0.4f);

        ImGui::GetWindowDrawList()->ChannelsSplit(2);
        if (m_MidiDeviceProcessor)
        {
            ImGui::GetWindowDrawList()->ChannelsSetCurrent(1);
            if (ImGui::Selectable("Record Midi", MidiDeviceProperty.bIsRecording, ImGuiSelectableFlags_AllowOverlap, ImVec2(0.f, 0.f)))
            {
                MidiDeviceProperty.bIsRecording = true;
            }

            ImGui::GetWindowDrawList()->ChannelsSetCurrent(0);
            if (ImGui::IsItemHovered() || MidiDeviceProperty.bIsRecording)
            {
                ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImColor(ImGui::IEStyle::Colors::RedButtonHoveredColor));
            }
            else 
            {
                ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImColor(ImGui::IEStyle::Colors::RedButtonColor));
            }
        }
        else
        {
            ImGui::GetWindowDrawList()->ChannelsSetCurrent(1);
            ImGui::Text("Recording not available");
            ImGui::GetWindowDrawList()->ChannelsSetCurrent(0);
            ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImColor(ImGui::IEStyle::Colors::RedButtonColor));
        }
        ImGui::GetWindowDrawList()->ChannelsMerge();

        ImGui::TableNextColumn();
        std::array<int, MIDI_MESSAGE_BYTE_COUNT> MidiMessageBuf = { MidiDeviceProperty.MidiMessage[0], MidiDeviceProperty.MidiMessage[1], MidiDeviceProperty.MidiMessage[2] };
        ImGui::SetNextItemWidth(InputBoxSizeWidth);
        ImGui::InputInt3("##Input Midi Message", MidiMessageBuf.data());
        MidiDeviceProperty.MidiMessage.resize(MIDI_MESSAGE_BYTE_COUNT);
        std::copy(MidiMessageBuf.begin(), MidiMessageBuf.end(), MidiDeviceProperty.MidiMessage.begin());

        ImGui::TableNextColumn();
        static const char Delete[] = "Delete";
        const ImVec2 DeleteSelectableSize = ImVec2(ImGui::CalcTextSize(Delete));
        ImGui::SetSmartCursorPosX(ImGui::GetWindowSize().x - DeleteSelectableSize.x - 40.0f);
        bDeleteRequested = ImGui::IEStyle::RedButton(Delete);

        ImGui::EndTable();
    }
}

void IEMidiEditor::DrawInitialOutputMessageEditor(std::vector<unsigned char>& MidiDeviceInitialOutputMidiMessage, bool& bDeleteRequested) const
{
    if (ImGui::BeginTable("Profile Midi Output Editor", MidiDeviceInitialOutputMessageEditorColumnCount, ImGuiTableFlags_SizingFixedFit))
    {
        ImGui::TableNextColumn();
        if (ImGui::Button("Send Midi Out"))
        {
            m_MidiDeviceProcessor->SendMidiOutputMessage(MidiDeviceInitialOutputMidiMessage);
        }

        ImGui::TableNextColumn();
        std::array<int, MIDI_MESSAGE_BYTE_COUNT> InitialOutputMidiMessageBuf = { MidiDeviceInitialOutputMidiMessage[0], MidiDeviceInitialOutputMidiMessage[1], MidiDeviceInitialOutputMidiMessage[2] };
        ImGui::SetNextItemWidth(InputBoxSizeWidth);
        ImGui::InputInt3("##Input Midi Message", InitialOutputMidiMessageBuf.data());
        MidiDeviceInitialOutputMidiMessage.resize(MIDI_MESSAGE_BYTE_COUNT);
        std::copy(InitialOutputMidiMessageBuf.begin(), InitialOutputMidiMessageBuf.end(), MidiDeviceInitialOutputMidiMessage.begin());

        ImGui::TableNextColumn();
        static const char Delete[] = "Delete";
        const ImVec2 DeleteSelectableSize = ImVec2(ImGui::CalcTextSize(Delete));
        ImGui::SetSmartCursorPosX(ImGui::GetWindowSize().x - DeleteSelectableSize.x - 40.0f);
        bDeleteRequested = ImGui::IEStyle::RedButton(Delete);

        ImGui::EndTable();
    }
}