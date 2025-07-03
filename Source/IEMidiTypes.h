// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <set>
#include <string>

#include "IELog.h"

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

struct IEMidiDeviceInputProperty;
struct IEMidiDeviceOutputProperty;
    
struct IEMidiDeviceProfile
{
public:
    explicit IEMidiDeviceProfile(const std::string& _NameID, uint32_t _InputPortNumber, uint32_t _OutputPortNumer) :
        NameID(_NameID),
        InputPortNumber(_InputPortNumber),
        OutputPortNumber(_OutputPortNumer)
    {}
    IEMidiDeviceProfile(const IEMidiDeviceProfile&) = delete;
    IEMidiDeviceProfile& operator=(const IEMidiDeviceProfile&) = delete;
    IEMidiDeviceProfile(IEMidiDeviceProfile&&) = delete;
    IEMidiDeviceProfile& operator=(IEMidiDeviceProfile&&) = delete;

    IEMidiDeviceInputProperty& MakeInputProperty();
    IEMidiDeviceOutputProperty& MakeOutputProperty();

public:
    const std::string NameID;
    const uint32_t InputPortNumber;
    const uint32_t OutputPortNumber;

public:
    std::shared_ptr<IEMidiDeviceInputProperty> InputPropertiesHead;
    std::shared_ptr<IEMidiDeviceOutputProperty> OutputPropertiesHead;
};

struct IEMidiDeviceInputProperty
{
private:
    IEMidiDeviceInputProperty(IEMidiDeviceProfile& _MidiDeviceProfile, const std::shared_ptr<IEMidiDeviceInputProperty>& PreviousProperty) :
        MidiDeviceProfile(_MidiDeviceProfile),
        m_PreviousProperty(PreviousProperty)
    {}
    IEMidiDeviceInputProperty(const IEMidiDeviceInputProperty&) = delete;
    IEMidiDeviceInputProperty& operator=(const IEMidiDeviceInputProperty&) = delete;
    IEMidiDeviceInputProperty(IEMidiDeviceInputProperty&&) = delete;
    IEMidiDeviceInputProperty& operator=(IEMidiDeviceInputProperty&&) = delete;

public:
    ~IEMidiDeviceInputProperty();

public:
    IEMidiDeviceInputProperty* Next() const;

public:
    void Delete();

public:
    IEMidiDeviceProfile& MidiDeviceProfile;
    friend IEMidiDeviceInputProperty& IEMidiDeviceProfile::MakeInputProperty();

public:
    // Serialized variables
    IEMidiMessageType MidiMessageType = IEMidiMessageType::None;
    IEMidiActionType MidiActionType = IEMidiActionType::None;
    std::string ConsoleCommand = std::string();
    std::filesystem::path OpenFilePath = std::filesystem::path();
    std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT> MidiMessage = {0, 0, 0};
    bool bIsMidiToggle = false;

public:
    // Runtime
    bool bIsRecording = false;
    bool bIsConsoleCommandActive = false;

private:
    std::weak_ptr<IEMidiDeviceInputProperty> m_PreviousProperty;
    std::shared_ptr<IEMidiDeviceInputProperty> m_NextProperty;
};

struct IEMidiDeviceOutputProperty
{
private:
    IEMidiDeviceOutputProperty(IEMidiDeviceProfile& _MidiDeviceProfile, const std::shared_ptr<IEMidiDeviceOutputProperty>& PreviousProperty) :
        MidiDeviceProfile(_MidiDeviceProfile),
        m_PreviousProperty(PreviousProperty)
    {}
    IEMidiDeviceOutputProperty(const IEMidiDeviceOutputProperty&) = delete;
    IEMidiDeviceOutputProperty& operator=(const IEMidiDeviceOutputProperty&) = delete;
    IEMidiDeviceOutputProperty(IEMidiDeviceOutputProperty&&) = delete;
    IEMidiDeviceOutputProperty& operator=(IEMidiDeviceOutputProperty&&) = delete;

public:
    ~IEMidiDeviceOutputProperty();

public:
    IEMidiDeviceOutputProperty* Next() const;

public:
    void Delete();

public:
    IEMidiDeviceProfile& MidiDeviceProfile;
    friend IEMidiDeviceOutputProperty& IEMidiDeviceProfile::MakeOutputProperty();

public:
    // Serialized variables
    std::array<uint8_t, MIDI_MESSAGE_BYTE_COUNT> MidiMessage = { 0, 0, 0 };

private:
    std::weak_ptr<IEMidiDeviceOutputProperty> m_PreviousProperty;
    std::shared_ptr<IEMidiDeviceOutputProperty> m_NextProperty;
};