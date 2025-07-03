// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include <map>
#include <memory>
#include <optional>
#include <vector>

#include "IEActions.h"
#include "IEConcurrency.h"
#include "IELog.h"
#include "RtMidi.h"

#include "IEMidiTypes.h"

class IEMidiProcessor
{
public:
    IEMidiProcessor() :
        m_MidiIn(std::make_unique<RtMidiIn>()),
        m_MidiOut(std::make_unique<RtMidiOut>()),
        m_VolumeAction(IEAction::GetVolumeAction()),
        m_MuteAction(IEAction::GetMuteAction()),
        m_ConsoleCommandAction(IEAction::GetConsoleCommandAction()),
        m_OpenFileAction(IEAction::GetOpenFileAction())
    {
        m_MidiIn->setErrorCallback(&IEMidiProcessor::OnRtMidiErrorCallback);
        m_MidiOut->setErrorCallback(&IEMidiProcessor::OnRtMidiErrorCallback);
    };
   
public:
    IEResult ProcessMidiInputMessage(const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>& MidiMessage);
    IEResult SendMidiOutputMessage(const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>& MidiMessage);

    std::vector<std::string> GetAvailableMidiDevices() const;
    std::string GetAPIName() const;
    IEResult ActivateMidiDeviceProfile(const std::string& MidiDeviceName);
    void DeactivateMidiDeviceProfile();
    bool HasActiveMidiDeviceProfile() const;
    IEMidiDeviceProfile& GetActiveMidiDeviceProfile();
    const IEMidiDeviceProfile& GetActiveMidiDeviceProfile() const;
    IESPSCQueue<std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>>& GetMidiLogMessagesBuffer() { return m_MidiLogMessagesBuffer; }
    const IESPSCQueue<std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>>& GetMidiLogMessagesBuffer() const { return m_MidiLogMessagesBuffer; }

public:
    [[nodiscard]] uint32_t AddOnMidiCallback(std::function<void(double, const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>)> Func);
    template<typename T>
    [[nodiscard]] uint32_t AddOnMidiCallback(T* Object, void (T::*MemFunc)(double, const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>));
    void RemoveOnMidiCallback(uint32_t CallbackID);

private:
    static void OnRtMidiCallback(double TimeStamp, std::vector<unsigned char>* Message, void* UserData);
    static void OnRtMidiErrorCallback(RtMidiError::Type RtMidiErrorType, const std::string& ErrorText, void* UserData);

private:
    std::string GetSanitizedMidiDeviceName(const std::string& MidiDeviceName, uint32_t InputPortNumber) const;

private:
    std::unique_ptr<RtMidiIn> m_MidiIn;
    std::unique_ptr<RtMidiOut> m_MidiOut;

private:
    std::optional<IEMidiDeviceProfile> m_ActiveMidiDeviceProfile;
    IESPSCQueue<std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>> m_MidiLogMessagesBuffer = IESPSCQueue<std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>>(10);
    std::map<uint32_t, std::function<void(double, const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>)>> m_MidiCallbackFuncs;

private:
    std::unique_ptr<IEAction_Volume> m_VolumeAction;
    std::unique_ptr<IEAction_Mute> m_MuteAction;
    std::unique_ptr<IEAction_ConsoleCommand> m_ConsoleCommandAction;
    std::unique_ptr<IEAction_OpenFile> m_OpenFileAction;
};

template<typename T>
inline uint32_t IEMidiProcessor::AddOnMidiCallback(T* Object, void(T::* MemFunc)(double, const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>))
{
    return AddOnMidiCallback([Object, MemFunc](double Timestamp, const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT> MidiMessage)
        {
            if (Object)
            {
                (Object->*MemFunc)(Timestamp, MidiMessage);
            }
        });
}