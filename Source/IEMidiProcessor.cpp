// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMidiProcessor.h"

IEResult IEMidiProcessor::ProcessMidiInputMessage(const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>& MidiMessage) const
{
    IEResult Result(IEResult::Type::Fail, "Failed to process Midi");

    if (IEAssert(MidiMessage.size() >= 3))
    {
        if (m_ActiveMidiDeviceProfile.has_value())
        {
            IEMidiDeviceInputProperty* ActiveMidiDeviceInputProperty = m_ActiveMidiDeviceProfile->InputPropertiesHead.get();
            while (ActiveMidiDeviceInputProperty)
            {
                if (ActiveMidiDeviceInputProperty->MidiMessage.size() >= 3 &&
                    ActiveMidiDeviceInputProperty->MidiMessage[0] == MidiMessage[0] &&
                    ActiveMidiDeviceInputProperty->MidiMessage[1] == MidiMessage[1])
                {
                    switch (ActiveMidiDeviceInputProperty->MidiActionType)
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

                                if (ActiveMidiDeviceInputProperty->MidiMessageType == IEMidiMessageType::NoteOnOff)
                                {
                                    if (ActiveMidiDeviceInputProperty->bIsMidiToggle)
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

                                switch (ActiveMidiDeviceInputProperty->MidiMessageType)
                                {
                                    case IEMidiMessageType::NoteOnOff:
                                    {
                                        if (ActiveMidiDeviceInputProperty->bIsMidiToggle)
                                        {
                                            const bool bOn = static_cast<unsigned int>(MidiMessage[2]) != 0;
                                            if (bOn)
                                            {
                                                if (ActiveMidiDeviceInputProperty->bIsConsoleCommandActive)
                                                {
                                                    m_ConsoleCommandAction->ExecuteConsoleCommand(ActiveMidiDeviceInputProperty->ConsoleCommand, 0.0f);
                                                    ActiveMidiDeviceInputProperty->bIsConsoleCommandActive = false;
                                                }
                                                else
                                                {
                                                    m_ConsoleCommandAction->ExecuteConsoleCommand(ActiveMidiDeviceInputProperty->ConsoleCommand, 1.0f);
                                                    ActiveMidiDeviceInputProperty->bIsConsoleCommandActive = true;
                                                }
                                            }
                                        }
                                        else
                                        {
                                            m_ConsoleCommandAction->ExecuteConsoleCommand(ActiveMidiDeviceInputProperty->ConsoleCommand, 1.0f);
                                        }
                                        break;
                                    }
                                    case IEMidiMessageType::ControlChange:
                                    {
                                        const float Value = static_cast<float>(MidiMessage[2]);
                                        m_ConsoleCommandAction->ExecuteConsoleCommand(ActiveMidiDeviceInputProperty->ConsoleCommand, Value);
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

                                if (ActiveMidiDeviceInputProperty->MidiMessageType == IEMidiMessageType::NoteOnOff)
                                {
                                    const bool bOn = static_cast<unsigned int>(MidiMessage[2]) != 0;
                                    if (bOn)
                                    {
                                        m_OpenFileAction->OpenFile(ActiveMidiDeviceInputProperty->OpenFilePath);
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
                ActiveMidiDeviceInputProperty = ActiveMidiDeviceInputProperty->Next();
            }
        }
    }
    if (Result.Type == IEResult::Type::Success)
    {
        Result.Message = std::string("Successfully processed Midi");
    }
    return Result;
}

IEResult IEMidiProcessor::SendMidiOutputMessage(const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>& MidiMessage) const
{
    IEResult Result(IEResult::Type::Fail, "Failed to send midi output message");
    if (m_MidiOut && m_ActiveMidiDeviceProfile)
    {
        m_MidiOut->sendMessage(MidiMessage.data(), MIDI_MESSAGE_BYTE_COUNT);
        Result.Type = IEResult::Type::Success;
        Result.Message = std::string("Successfully sent midi output message");
    }
    return Result;
}

std::vector<std::string> IEMidiProcessor::GetAvailableMidiDevices() const
{
    std::vector<std::string> AvailableMidiDevices;
    if (m_bTestMode)
    {
        AvailableMidiDevices.emplace_back(std::string("Faderport"));
        AvailableMidiDevices.emplace_back(std::string("M-Audio"));
    }
    else if (m_MidiIn)
    {
        for (int InputPortNumber = 0; InputPortNumber < m_MidiIn->getPortCount(); InputPortNumber++)
        {
            const std::string MidiDeviceName = GetSanitizedMidiDeviceName(m_MidiIn->getPortName(InputPortNumber), InputPortNumber);
            AvailableMidiDevices.emplace_back(MidiDeviceName);
        }
    }
    return AvailableMidiDevices;
}

std::string IEMidiProcessor::GetAPIName() const
{
    return RtMidiOut::getApiDisplayName(m_MidiOut->getCurrentApi());
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

void IEMidiProcessor::SetTestMode(bool bTestMode)
{
    m_bTestMode = bTestMode;
    for (int i = 0; i < 10; i++)
    {
        m_MidiLogMessagesBuffer.Push(std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>{127, 0, 0});
    }
}

IEResult IEMidiProcessor::ActivateMidiDeviceProfile(const std::string& MidiDeviceName)
{
    IEResult Result(IEResult::Type::Fail);
    Result.Message = std::format("Failed to activate midi device profile {}.", MidiDeviceName);

    if (m_bTestMode)
    {
        m_ActiveMidiDeviceProfile.emplace(MidiDeviceName, 0, 0);
        Result.Type = IEResult::Type::Success;
        Result.Message = std::format("Successfully activated test midi device profile {}", MidiDeviceName);
    }
    else if (m_MidiIn && m_MidiOut)
    {
        for (int InputPortNumber = 0; InputPortNumber < m_MidiIn->getPortCount(); InputPortNumber++)
        {
            const std::string& MidiDeviceNameIn = GetSanitizedMidiDeviceName(m_MidiIn->getPortName(InputPortNumber), InputPortNumber);
            if (MidiDeviceNameIn.find(MidiDeviceName) != std::string::npos)
            {
                for (int OutputPortNumber = 0; OutputPortNumber < m_MidiOut->getPortCount(); OutputPortNumber++)
                {
                    const std::string& MidiDeviceNameOut = GetSanitizedMidiDeviceName(m_MidiOut->getPortName(OutputPortNumber), InputPortNumber);
                    if (MidiDeviceNameOut.find(MidiDeviceName) != std::string::npos)
                    {
                        m_ActiveMidiDeviceProfile.emplace(MidiDeviceName, InputPortNumber, OutputPortNumber);

                        if (m_MidiIn->isPortOpen())
                        {
                            m_MidiIn->cancelCallback();
                            m_MidiIn->closePort();
                        }
                        m_MidiIn->setCallback(&IEMidiProcessor::OnRtMidiCallback, this);
                        m_MidiIn->openPort(m_ActiveMidiDeviceProfile->InputPortNumber);

                        if (m_MidiOut->isPortOpen())
                        {
                            m_MidiOut->closePort();
                        }
                        m_MidiOut->openPort(m_ActiveMidiDeviceProfile->OutputPortNumber);

                        IEMidiDeviceOutputProperty* MidiDeviceOutputProperty = GetActiveMidiDeviceProfile().OutputPropertiesHead.get();
                        while (MidiDeviceOutputProperty)
                        {
                            m_MidiOut->sendMessage(MidiDeviceOutputProperty->MidiMessage.data(), MIDI_MESSAGE_BYTE_COUNT);
                            MidiDeviceOutputProperty = MidiDeviceOutputProperty->Next();
                        }

                        Result.Type = IEResult::Type::Success;
                        Result.Message = std::format("Successfully activated midi device profile {}", MidiDeviceName);
                    }
                }
            }
        }
    }
    
    return Result;
}

void IEMidiProcessor::DeactivateMidiDeviceProfile()
{
    if (m_MidiIn)
    {
        if (m_MidiIn->isPortOpen())
        {
            m_MidiIn->closePort();
            m_MidiIn->cancelCallback();
        }
    }

    if (m_MidiOut)
    {
        if (m_MidiOut->isPortOpen())
        {
            m_MidiOut->closePort();
        }
    }

    m_ActiveMidiDeviceProfile.reset();
}

bool IEMidiProcessor::HasActiveMidiDeviceProfile() const
{
    return m_ActiveMidiDeviceProfile.has_value();
}

uint32_t IEMidiProcessor::AddOnMidiCallback(std::function<void(double, const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>)> Func)
{
    static uint32_t CallbackIDGenerator = 0;
    m_MidiCallbackFuncs.emplace(CallbackIDGenerator++, Func);
    return CallbackIDGenerator;
}

void IEMidiProcessor::RemoveOnMidiCallback(uint32_t CallbackID)
{
    m_MidiCallbackFuncs.erase(CallbackID);
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
                    IEMidiDeviceInputProperty* MidiDeviceInputProperty = MidiProcessor->m_ActiveMidiDeviceProfile->InputPropertiesHead.get();
                    while (MidiDeviceInputProperty)
                    {
                        if (MidiDeviceInputProperty->bIsRecording)
                        {
                            MidiDeviceInputProperty->MidiMessage = MidiMessage;
                            MidiDeviceInputProperty->bIsRecording = false;
                            bIncludeProcess = false;
                        }
                        MidiDeviceInputProperty = MidiDeviceInputProperty->Next();
                    }
                }

                if (MidiProcessor->m_MidiLogMessagesBuffer.IsFull())
                {
                    MidiProcessor->m_MidiLogMessagesBuffer.Pop();
                }
                MidiProcessor->m_MidiLogMessagesBuffer.Push(MidiMessage);

                if (bIncludeProcess)
                {
                    MidiProcessor->ProcessMidiInputMessage(MidiMessage);
                }

                for (const auto& Func : MidiProcessor->m_MidiCallbackFuncs)
                {
                    Func.second(TimeStamp, MidiMessage);
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

    const size_t ColonSuffixIndex = MidiDeviceName.find(":");
    if (ColonSuffixIndex != std::string::npos)
    {
        SanitizedMidiDeviceName.erase(ColonSuffixIndex, SanitizedMidiDeviceName.length() - ColonSuffixIndex);
    }

    return SanitizedMidiDeviceName;
}