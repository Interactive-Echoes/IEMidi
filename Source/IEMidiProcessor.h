// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include <deque>
#include <memory>
#include <optional>
#include <vector>

#include "IELog.h"
#include "RtMidi.h"
#include "IEActions.h"

#include "IEMidiTypes.h"

static constexpr size_t INCOMING_MIDI_MESSAGES_SIZE = 20;

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
        m_IncomingMidiMessages.resize(INCOMING_MIDI_MESSAGES_SIZE);
    };

public:
    RtMidiIn& GetMidiIn() const { return *m_MidiIn; }
    RtMidiOut& GetMidiOut() const { return *m_MidiOut; }
    
public:
    IEResult ProcessMidiInputMessage(const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>& MidiMessage);
    IEResult SendMidiOutputMessage(const std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>& MidiMessage);

    std::vector<std::string> GetAvailableMidiDevices() const;
    IEResult ActivateMidiDeviceProfile(const std::string& MidiDeviceName);
    void DeactivateMidiDeviceProfile();
    bool HasActiveMidiDeviceProfile() const;
    IEMidiDeviceProfile& GetActiveMidiDeviceProfile();
    const IEMidiDeviceProfile& GetActiveMidiDeviceProfile() const;
    const std::deque<std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>>& GetIncomingMidiMessages() const { return m_IncomingMidiMessages; }

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
    std::deque<std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT>> m_IncomingMidiMessages;

private:
    std::unique_ptr<IEAction_Volume> m_VolumeAction;
    std::unique_ptr<IEAction_Mute> m_MuteAction;
    std::unique_ptr<IEAction_ConsoleCommand> m_ConsoleCommandAction;
    std::unique_ptr<IEAction_OpenFile> m_OpenFileAction;
};