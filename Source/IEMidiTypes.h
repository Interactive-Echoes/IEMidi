// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include <array>
#include <string>
#include <cstdint>
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

    IEMidiDeviceInputProperty& operator=(const IEMidiDeviceInputProperty& Other)
    {
        if (this != &Other)
        {
            MidiDeviceName = Other.MidiDeviceName;
            MidiMessageType = Other.MidiMessageType;
            MidiActionType = Other.MidiActionType;
            ConsoleCommand = Other.ConsoleCommand;
            OpenFilePath = Other.OpenFilePath;
            MidiMessage = Other.MidiMessage;
            bToggle = Other.bToggle;
        }
        return *this;
    }

    bool operator==(const IEMidiDeviceInputProperty& Other) const
    {
        return RuntimeID == Other.RuntimeID;
    }

public:
    const uint32_t RuntimeID;
    bool bIsRecording = false;
    bool bIsConsoleCommandActive = false;

public:
    std::string MidiDeviceName = std::string();
    IEMidiMessageType MidiMessageType = IEMidiMessageType::None;
    IEMidiActionType MidiActionType = IEMidiActionType::None;
    std::string ConsoleCommand = std::string();
    std::string OpenFilePath = std::string();
    std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT> MidiMessage;
    bool bToggle = false;
};

struct IEMidiDeviceOutputProperty
{
public:
    IEMidiDeviceOutputProperty(const std::string& MidiDeviceNameID) :
            MidiDeviceName(MidiDeviceNameID),
            RuntimeID(MidiDevicePropertyIDGenerator::Generate())
    {}

    IEMidiDeviceOutputProperty& operator=(const IEMidiDeviceOutputProperty& Other)
    {
        if (this != &Other)
        {
            MidiDeviceName = Other.MidiDeviceName;
            MidiMessage = Other.MidiMessage;
        }
        return *this;
    }

    bool operator==(const IEMidiDeviceOutputProperty& Other) const
    {
        return RuntimeID == Other.RuntimeID;
    }

public:
    const uint32_t RuntimeID;

public:
    std::string MidiDeviceName = std::string();
    std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT> MidiMessage;
};

struct IEMidiDevicePropertyHash
{
    size_t operator()(const IEMidiDeviceInputProperty& MidiDeviceProperty) const
    {
        return std::hash<uint32_t>()(MidiDeviceProperty.RuntimeID);
    }
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

    uint32_t GetInputPortNumber() const { return m_InputPortNumber; }
    uint32_t GetOutputPortNumber() const { return m_OutputPortNumber; }

public:
    std::string Name;
    std::vector<IEMidiDeviceInputProperty> InputProperties;
    std::vector<IEMidiDeviceOutputProperty> OutputProperties;
    
private:
    uint32_t m_InputPortNumber = -1;
    uint32_t m_OutputPortNumber = -1;
};

struct IEMidiDeviceProfileHash
{
    size_t operator()(const IEMidiDeviceProfile& MidiDeviceProfile) const
    {
        return std::hash<std::string>()(MidiDeviceProfile.Name);
    }
};