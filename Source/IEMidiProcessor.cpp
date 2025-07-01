// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMidiProcessor.h"

IEResult IEMidiProcessor::ProcessMidiInputMessage(const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>& MidiMessage)
{
    IEResult Result(IEResult::Type::Fail, "Failed to process Midi");

    if (IEAssert(MidiMessage.size() >= 3))
    {
        if (m_ActiveMidiDeviceProfile.has_value())
        {
            for (IEMidiDeviceInputProperty& ActiveMidiDeviceProperty : m_ActiveMidiDeviceProfile->InputProperties)
            {
                if (ActiveMidiDeviceProperty.MidiMessage.size() >= 3 &&
                    ActiveMidiDeviceProperty.MidiMessage[0] == MidiMessage[0] &&
                    ActiveMidiDeviceProperty.MidiMessage[1] == MidiMessage[1])
                {
                    switch (ActiveMidiDeviceProperty.MidiActionType)
                    {
                        case IEMidiActionType::Volume:
                        {
                            if (m_VolumeAction)
                            {
                                Result.Type = IEResult::Type::Success;

                                const float Value = static_cast<float>(MidiMessage[2]);
                                m_VolumeAction->SetVolume(Value/127.0f);
                            }
                            break;
                        }
                        case IEMidiActionType::Mute:
                        {
                            if (m_MuteAction)
                            {
                                Result.Type = IEResult::Type::Success;

                                if (ActiveMidiDeviceProperty.MidiMessageType == IEMidiMessageType::NoteOnOff)
                                {
                                    if (ActiveMidiDeviceProperty.bToggle)
                                    {
                                        const bool bOn = static_cast<unsigned int>(MidiMessage[2]) != 0;
                                        if (bOn)
                                        {
                                            if (m_MuteAction->GetMute())
                                            {
                                                m_MuteAction->SetMute(false);
                                            }
                                            else
                                            {
                                                m_MuteAction->SetMute(true);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        const bool bMute = static_cast<bool>(MidiMessage[2]);
                                        m_MuteAction->SetMute(bMute);
                                    }
                                }
                            }
                            break;
                        }
                        case IEMidiActionType::ConsoleCommand:
                        {
                            if (m_ConsoleCommandAction)
                            {
                                Result.Type = IEResult::Type::Success;

                                switch (ActiveMidiDeviceProperty.MidiMessageType)
                                {
                                    case IEMidiMessageType::NoteOnOff:
                                    {
                                        if (ActiveMidiDeviceProperty.bToggle)
                                        {
                                            const bool bOn = static_cast<unsigned int>(MidiMessage[2]) != 0;
                                            if (bOn)
                                            {
                                                if (ActiveMidiDeviceProperty.bIsConsoleCommandActive)
                                                {
                                                    m_ConsoleCommandAction->ExecuteConsoleCommand(ActiveMidiDeviceProperty.ConsoleCommand, 0.0f);
                                                    ActiveMidiDeviceProperty.bIsConsoleCommandActive = false;
                                                }
                                                else
                                                {
                                                    m_ConsoleCommandAction->ExecuteConsoleCommand(ActiveMidiDeviceProperty.ConsoleCommand, 1.0f);
                                                    ActiveMidiDeviceProperty.bIsConsoleCommandActive = true;
                                                }
                                            }
                                        }
                                        else
                                        {
                                            m_ConsoleCommandAction->ExecuteConsoleCommand(ActiveMidiDeviceProperty.ConsoleCommand, 1.0f);
                                        }
                                        break;
                                    }
                                    case IEMidiMessageType::ControlChange:
                                    {
                                        const float Value = static_cast<float>(MidiMessage[2]);
                                        m_ConsoleCommandAction->ExecuteConsoleCommand(ActiveMidiDeviceProperty.ConsoleCommand, Value);
                                        break;
                                    }
                                    default:
                                    {
                                        break;
                                    }
                                }
                            }
                            break;
                        }
                        case IEMidiActionType::OpenFile:
                        {
                            if (m_OpenFileAction)
                            {
                                Result.Type = IEResult::Type::Success;

                                if (ActiveMidiDeviceProperty.MidiMessageType == IEMidiMessageType::NoteOnOff)
                                {
                                    const bool bOn = static_cast<unsigned int>(MidiMessage[2]) != 0;
                                    if (bOn)
                                    {
                                        m_OpenFileAction->OpenFile(ActiveMidiDeviceProperty.OpenFilePath);
                                    }
                                }
                            }
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }
                }
            }
        }
    }
    if (Result.Type == IEResult::Type::Success)
    {
        Result.Message = std::string("Successfully processed Midi");
    }
    return Result;
}

IEResult IEMidiProcessor::SendMidiOutputMessage(const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>& MidiMessage)
{
    IEResult Result(IEResult::Type::Fail, "Failed to send midi output message");
    if (m_ActiveMidiDeviceProfile)
    {
        GetMidiOut().sendMessage(MidiMessage.data(), MIDI_MESSAGE_BYTE_COUNT);

        Result.Type = IEResult::Type::Success;
        Result.Message = std::string("Successfully sent midi output message");
    }
    return Result;
}

std::vector<std::string> IEMidiProcessor::GetAvailableMidiDevices() const
{
    std::vector<std::string> AvailableMidiDevices;

    RtMidiIn& MidiIn = GetMidiIn();
    for (int InputPortNumber = 0; InputPortNumber < MidiIn.getPortCount(); InputPortNumber++)
    {
        const std::string MidiDeviceName = GetSanitizedMidiDeviceName(MidiIn.getPortName(InputPortNumber), InputPortNumber);
        AvailableMidiDevices.emplace_back(MidiDeviceName);
    }
    return AvailableMidiDevices;
}

IEMidiDeviceProfile& IEMidiProcessor::GetActiveMidiDeviceProfile()
{
    if (!m_ActiveMidiDeviceProfile.has_value())
    {
        IELOG_ERROR("No active midi device profile");
        abort();
    }

    return m_ActiveMidiDeviceProfile.value();
}

const IEMidiDeviceProfile& IEMidiProcessor::GetActiveMidiDeviceProfile() const
{
    if (!m_ActiveMidiDeviceProfile.has_value())
    {
        IELOG_ERROR("No active midi device profile");
        abort();
    }

    return m_ActiveMidiDeviceProfile.value();
}

IEResult IEMidiProcessor::ActivateMidiDeviceProfile(const std::string& MidiDeviceName)
{
    IEResult Result(IEResult::Type::Fail);
    Result.Message = std::format("Failed to activate midi device profile {}", MidiDeviceName);

    RtMidiIn& MidiIn = GetMidiIn();
    for (int InputPortNumber = 0; InputPortNumber < MidiIn.getPortCount(); InputPortNumber++)
    {
        const std::string& MidiDeviceName = GetSanitizedMidiDeviceName(MidiIn.getPortName(InputPortNumber), InputPortNumber);

        RtMidiOut& MidiOut = GetMidiOut();
        for (int OutputPortNumber = 0; OutputPortNumber < MidiOut.getPortCount(); OutputPortNumber++)
        {
            const std::string& MidiDeviceNameOut = GetSanitizedMidiDeviceName(MidiOut.getPortName(OutputPortNumber), InputPortNumber);
            if (MidiDeviceNameOut.find(MidiDeviceName) != std::string::npos)
            {
                m_ActiveMidiDeviceProfile = IEMidiDeviceProfile(MidiDeviceName, InputPortNumber, OutputPortNumber);

                if (MidiIn.isPortOpen())
                {
                    MidiIn.cancelCallback();
                    MidiIn.closePort();
                }
                MidiIn.setCallback(&IEMidiProcessor::OnRtMidiCallback, this);
                
                MidiIn.openPort(m_ActiveMidiDeviceProfile->GetInputPortNumber());

                if (MidiOut.isPortOpen())
                {
                    MidiOut.closePort();
                }
                MidiOut.openPort(m_ActiveMidiDeviceProfile->GetOutputPortNumber());

                for (const IEMidiDeviceOutputProperty& MidiDeviceOutputProperty : m_ActiveMidiDeviceProfile->OutputProperties)
                {
                    MidiOut.sendMessage(MidiDeviceOutputProperty.MidiMessage.data(), MIDI_MESSAGE_BYTE_COUNT);
                }

                Result.Type = IEResult::Type::Success;
                Result.Message = std::format("Successfully activated midi device profile {}", MidiDeviceName);

                break;
            }
        }
    }
    
    return Result;
}

void IEMidiProcessor::DeactivateMidiDeviceProfile()
{
    RtMidiIn& MidiIn = GetMidiIn();
    if (MidiIn.isPortOpen())
    {
        MidiIn.closePort();
        MidiIn.cancelCallback();
    }
    
    RtMidiOut& MidiOut = GetMidiOut();
    if (MidiOut.isPortOpen())
    {
        MidiOut.closePort();
    }

    m_ActiveMidiDeviceProfile.reset();
}

bool IEMidiProcessor::HasActiveMidiDeviceProfile() const
{
    return m_ActiveMidiDeviceProfile.has_value();
}

void IEMidiProcessor::OnRtMidiCallback(double TimeStamp, std::vector<unsigned char>* Message, void* UserData)
{
    if (Message && UserData)
    {
        if (IEAssert(Message->size() >= MIDI_MESSAGE_BYTE_COUNT))
        {
            std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT> MidiMessage;
            std::copy(Message->begin(), Message->begin() + MIDI_MESSAGE_BYTE_COUNT, MidiMessage.begin());

            if (IEMidiProcessor* const MidiProcessor = reinterpret_cast<IEMidiProcessor*>(UserData))
            {
                bool bIncludeProcess = true;
                if (MidiProcessor->m_ActiveMidiDeviceProfile)
                {
                    for (IEMidiDeviceInputProperty& MidiDeviceProperty : MidiProcessor->m_ActiveMidiDeviceProfile->InputProperties)
                    {
                        if (MidiDeviceProperty.bIsRecording)
                        {
                            MidiDeviceProperty.MidiMessage = MidiMessage;
                            MidiDeviceProperty.bIsRecording = false;
                            bIncludeProcess = false;
                        }
                    }
                }

                if (MidiProcessor->m_IncomingMidiMessages.size() == INCOMING_MIDI_MESSAGES_SIZE)
                {
                    MidiProcessor->m_IncomingMidiMessages.pop_back();
                }

                MidiProcessor->m_IncomingMidiMessages.push_front(MidiMessage);

                if (bIncludeProcess)
                {
                    MidiProcessor->ProcessMidiInputMessage(MidiMessage);
                }
            }
        }
    }
}

void IEMidiProcessor::OnRtMidiErrorCallback(RtMidiError::Type RtMidiErrorType, const std::string& ErrorText, void* UserData)
{
    IELOG_ERROR("%s", ErrorText.c_str());
}

std::string IEMidiProcessor::GetSanitizedMidiDeviceName(const std::string& MidiDeviceName, uint32_t InputPortNumber) const
{
    std::string SanitizedMidiDeviceName = MidiDeviceName;

    const std::string NumericSuffix = std::to_string(InputPortNumber);
    const size_t NumericSuffixIndex = MidiDeviceName.find(NumericSuffix);
    if (NumericSuffixIndex != std::string::npos)
    {
        SanitizedMidiDeviceName.erase(NumericSuffixIndex - 1, NumericSuffix.length() + 1);
    }
    return SanitizedMidiDeviceName;
}