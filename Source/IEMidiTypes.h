// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

static constexpr size_t MIDI_MESSAGE_BYTE_COUNT = 3;

enum class IEMidiMessageType : uint8_t
{
    None,
    NoteOnOff,
    ControlChange,

    Count,
};

enum class IEMidiActionType : uint8_t
{
    None,
    Volume,
    Mute,
    ConsoleCommand,
    OpenFile,

    Count,
};

struct MidiDevicePropertyIDGenerator
{
private:
    static uint32_t MidiDevicePropertyID;
    
public:
    static uint32_t Generate() { return MidiDevicePropertyID++; };
};

struct IEMidiDeviceInputProperty
{
public:
    IEMidiDeviceInputProperty(const std::string& MidiDeviceNameID) :
        MidiDeviceName(MidiDeviceNameID),
        RuntimeID(MidiDevicePropertyIDGenerator::Generate())
    {}

    IEMidiDeviceInputProperty(const IEMidiDeviceInputProperty&) = delete;
    IEMidiDeviceInputProperty& operator=(const IEMidiDeviceInputProperty&) = delete;
    IEMidiDeviceInputProperty(IEMidiDeviceInputProperty&&) = default;
    IEMidiDeviceInputProperty& operator=(IEMidiDeviceInputProperty&&) = default;

public:
    const std::string& GetMidiDeviceName() const { return MidiDeviceName; };

public:
    IEMidiMessageType MidiMessageType = IEMidiMessageType::None;
    IEMidiActionType MidiActionType = IEMidiActionType::None;
    std::string ConsoleCommand = std::string();
    std::filesystem::path OpenFilePath = std::filesystem::path();
    std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT> MidiMessage = {0, 0, 0};
    bool bIsMidiToggle = false;

public:
    bool bIsRecording = false;
    bool bIsConsoleCommandActive = false;

private:
    std::string MidiDeviceName = std::string();
    uint32_t RuntimeID = -1;
};

struct IEMidiDeviceOutputProperty
{
public:
    IEMidiDeviceOutputProperty(const std::string& MidiDeviceNameID) :
        MidiDeviceName(MidiDeviceNameID),
        RuntimeID(MidiDevicePropertyIDGenerator::Generate())
    {}
    IEMidiDeviceOutputProperty(const IEMidiDeviceOutputProperty&) = delete;
    IEMidiDeviceOutputProperty& operator=(const IEMidiDeviceOutputProperty&) = delete;
    IEMidiDeviceOutputProperty(IEMidiDeviceOutputProperty&&) = default;
    IEMidiDeviceOutputProperty& operator=(IEMidiDeviceOutputProperty&&) = default;

    bool operator==(const IEMidiDeviceOutputProperty& Other) const
    {
        return RuntimeID == Other.RuntimeID;
    }

public:
    const std::string& GetMidiDeviceName() const { return MidiDeviceName; };

public:
    std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT> MidiMessage = {0, 0, 0};

private:
    std::string MidiDeviceName = std::string();
    uint32_t RuntimeID = -1;
};

struct IEMidiDeviceProfile
{
public:
    IEMidiDeviceProfile() = default;
    IEMidiDeviceProfile(const std::string& NameID, uint32_t InputPortNumber, uint32_t OutputPortNumer) :
                        Name(NameID), m_InputPortNumber(InputPortNumber), m_OutputPortNumber(OutputPortNumer) {}
    bool operator==(const IEMidiDeviceProfile& Other) const
    {
        return Name == Other.Name;
    }

public:
    const std::string& GetName() const { return Name; };
    uint32_t GetInputPortNumber() const { return m_InputPortNumber; }
    uint32_t GetOutputPortNumber() const { return m_OutputPortNumber; }

public:
    
    std::vector<IEMidiDeviceInputProperty> InputProperties;
    std::vector<IEMidiDeviceOutputProperty> OutputProperties;
    
private:
    std::string Name = std::string();
    uint32_t m_InputPortNumber = -1;
    uint32_t m_OutputPortNumber = -1;
};