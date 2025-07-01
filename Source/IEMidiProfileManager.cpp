// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMidiProfileManager.h"

#include "QStandardPaths.h"
#include "ryml.hpp"
#include "ryml_std.hpp"

static constexpr char IEMIDI_PROFILES_FILENAME[] = "profiles.yaml";
static constexpr char MIDI_PROFILE_INPUT_PROPERTIES_NODE_NAME[] = "InputProperties";
static constexpr char MIDI_PROFILE_OUTPUT_PROPERTIES_NODE_NAME[] = "OutputProperties";

static constexpr char MIDI_MESSAGE_TYPE_KEY_NAME[] = "Midi Message Type";
static constexpr char MIDI_TOGGLE_KEY_NAME[] = "Midi Toggle";
static constexpr char MIDI_ACTION_TYPE_KEY_NAME[] = "Midi Action Type";
static constexpr char CONSOLE_COMMAND_KEY_NAME[] = "Console Command";
static constexpr char OPEN_FILE_PATH_KEY_NAME[] = "Open File Path";
static constexpr char MIDI_MESSAGE_KEY_NAME[] = "Midi Message";

static constexpr uint32_t INITIAL_TREE_NODE_COUNT = 30;
static constexpr uint32_t INITIAL_TREE_ARENA_CHAR_COUNT = 2048;

template<typename T, std::size_t N>
void operator>>(const ryml::ConstNodeRef& Node, std::array<T, N>& Array)
{
    if (Node.is_seq() && Node.num_children() == N)
    {
        for (size_t i = 0; i < N; i++)
        {
            Node.at(i) >> Array[i];
        }
    }
}

template<typename T, std::size_t N>
void operator<<(ryml::NodeRef Node, const std::array<T, N>& Array)
{
    Node.clear();
    Node |= ryml::SEQ;

    for (const T& Element : Array)
    {
        Node.append_child() << Element;
    }
}

void operator>>(const ryml::ConstNodeRef& Node, std::filesystem::path& Path)
{
    std::string TempString;
    Node >> TempString;
    Path = std::filesystem::path(TempString);
}

void operator<<(ryml::NodeRef Node, const std::filesystem::path& Path)
{
    Node << Path.string();
}

IEMidiProfileManager::IEMidiProfileManager()
{
    const std::filesystem::path IEMidiConfigFolderPath; //= IEUtils::GetIEConfigFolderPath();
    if (!IEMidiConfigFolderPath.empty())
    {
        const std::filesystem::path MidiProfilesFilePath = IEMidiConfigFolderPath / IEMIDI_PROFILES_FILENAME;
        const std::string ProfilesFilePathString = MidiProfilesFilePath.string();
        if (!std::filesystem::exists(MidiProfilesFilePath))
        {
            if (std::FILE* const ProfilesFile = std::fopen(ProfilesFilePathString.c_str(), "w"))
            {
                IELOG_SUCCESS("Successfully created profiles settings file %s", ProfilesFilePathString.c_str());
            }
        }
        IELOG_SUCCESS("Using profiles settings file %s", ProfilesFilePathString.c_str());
    }
}

std::filesystem::path IEMidiProfileManager::GetIEMidiProfilesFilePath() const
{
    std::filesystem::path MidiDeviceProfilesFilePath;
    const std::filesystem::path IEMidiConfigFolderPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
    if (std::filesystem::exists(IEMidiConfigFolderPath))
    {
        const std::filesystem::path PotentialMidiDeviceProfilesFilePath = IEMidiConfigFolderPath / IEMIDI_PROFILES_FILENAME;
        if (std::filesystem::exists(PotentialMidiDeviceProfilesFilePath))
        {
            MidiDeviceProfilesFilePath = PotentialMidiDeviceProfilesFilePath;
        }
    }
    return MidiDeviceProfilesFilePath;
}

bool IEMidiProfileManager::HasProfile(const IEMidiDeviceProfile& MidiDeviceProfile) const
{
    bool bHasProfile = false;

    const std::filesystem::path MidiProfilesFilePath = GetIEMidiProfilesFilePath();
    if (std::filesystem::exists(MidiProfilesFilePath))
    {
        const std::string Content = ExtractFileContent(MidiProfilesFilePath);

        ryml::Tree MidiProfilesTree;
        MidiProfilesTree.reserve(INITIAL_TREE_NODE_COUNT);
        MidiProfilesTree.reserve_arena(INITIAL_TREE_ARENA_CHAR_COUNT);
        ryml::parse_in_arena(ryml::to_csubstr(Content), &MidiProfilesTree);

        const ryml::ConstNodeRef Root = MidiProfilesTree.rootref();
        if (Root.is_map())
        {
            if (Root.has_child(MidiDeviceProfile.GetName().c_str()))
            {
                bHasProfile = true;
            }
        }
    }
    
    return bHasProfile;
}

IEResult IEMidiProfileManager::SaveProfile(const IEMidiDeviceProfile& MidiDeviceProfile) const
{
    IEResult Result(IEResult::Type::Fail, "Failed to save profile");

    const std::filesystem::path MidiProfilesFilePath = GetIEMidiProfilesFilePath();
    if (std::filesystem::exists(MidiProfilesFilePath))
    {
        if (std::FILE* const ProfilesFile = std::fopen(MidiProfilesFilePath.string().c_str(), "w"))
        {
            const std::string Content = ExtractFileContent(MidiProfilesFilePath);

            ryml::Tree MidiProfilesTree;
            MidiProfilesTree.reserve(INITIAL_TREE_NODE_COUNT);
            MidiProfilesTree.reserve_arena(INITIAL_TREE_ARENA_CHAR_COUNT);
            ryml::parse_in_arena(ryml::to_csubstr(Content), &MidiProfilesTree);

            ryml::NodeRef Root = MidiProfilesTree.rootref();
            if (!Root.is_map() && Root.empty())
            {
                Root |= ryml::MAP;
            }

            const char* MidiDeviceName = MidiDeviceProfile.GetName().c_str();
            ryml::NodeRef MidiProfileNode = Root[MidiDeviceName];
            if (MidiProfileNode.is_seed())
            {
                MidiProfileNode.create();
                MidiProfileNode |= ryml::MAP;
            }

            // Input properties serialization
            {
                ryml::NodeRef MidiProfileInputPropertiesNode = MidiProfileNode[MIDI_PROFILE_INPUT_PROPERTIES_NODE_NAME];
                if (MidiProfileInputPropertiesNode.is_seed())
                {
                    MidiProfileInputPropertiesNode.create();
                    MidiProfileInputPropertiesNode |= ryml::SEQ;
                }
                MidiProfileInputPropertiesNode.clear_children();
                for (const IEMidiDeviceInputProperty& MidiDeviceInputProperty : MidiDeviceProfile.InputProperties)
                {
                    ryml::NodeRef MidiProfileInputPropertyNode = MidiProfileInputPropertiesNode.append_child();
                    MidiProfileInputPropertyNode.create();
                    MidiProfileInputPropertyNode |= ryml::MAP;

                    MidiProfileInputPropertyNode[MIDI_MESSAGE_TYPE_KEY_NAME] << static_cast<uint8_t>(MidiDeviceInputProperty.MidiMessageType);
                    MidiProfileInputPropertyNode[MIDI_TOGGLE_KEY_NAME] << MidiDeviceInputProperty.bIsMidiToggle;
                    MidiProfileInputPropertyNode[MIDI_ACTION_TYPE_KEY_NAME] << static_cast<uint8_t>(MidiDeviceInputProperty.MidiActionType);
                    MidiProfileInputPropertyNode[CONSOLE_COMMAND_KEY_NAME] << MidiDeviceInputProperty.ConsoleCommand;
                    MidiProfileInputPropertyNode[OPEN_FILE_PATH_KEY_NAME] << MidiDeviceInputProperty.OpenFilePath;
                    MidiProfileInputPropertyNode[MIDI_MESSAGE_KEY_NAME] << MidiDeviceInputProperty.MidiMessage;
                    // Other input properties go here
                }
            }

            // Output properties serialization
            {
                ryml::NodeRef MidiProfileOutputPropertiesNode = MidiProfileNode[MIDI_PROFILE_OUTPUT_PROPERTIES_NODE_NAME];
                if (MidiProfileOutputPropertiesNode.is_seed())
                {
                    MidiProfileOutputPropertiesNode.create();
                    MidiProfileOutputPropertiesNode |= ryml::SEQ;
                }
                MidiProfileOutputPropertiesNode.clear_children();
                for (const IEMidiDeviceOutputProperty& MidiDeviceOutputProperty : MidiDeviceProfile.OutputProperties)
                {
                    ryml::NodeRef MidiProfileOutputPropertyNode = MidiProfileOutputPropertiesNode.append_child();
                    MidiProfileOutputPropertyNode.create();
                    MidiProfileOutputPropertyNode |= ryml::MAP;

                    MidiProfileOutputPropertyNode[MIDI_MESSAGE_KEY_NAME] << MidiDeviceOutputProperty.MidiMessage;
                    // Other Output properties go here
                }
            }

            const size_t EmitSize = ryml::emit_yaml(MidiProfilesTree, ProfilesFile);
            if (EmitSize)
            {
                Result.Type = IEResult::Type::Success;
                Result.Message = std::format("Successfully saved profile {}, into {}", MidiDeviceName, MidiProfilesFilePath.string());
            }

            std::fclose(ProfilesFile);
        }
    }
    return Result;
}

IEResult IEMidiProfileManager::LoadProfile(IEMidiDeviceProfile& MidiDeviceProfile) const
{
    IEResult Result(IEResult::Type::Fail, "Failed to load profile");

    const std::filesystem::path MidiProfilesFilePath = GetIEMidiProfilesFilePath();
    if (std::filesystem::exists(MidiProfilesFilePath))
    {
        Result.Type = IEResult::Type::Success;
        Result.Message = std::format("Successfully found profiles config file {}", MidiProfilesFilePath.string());

        const std::string Content = ExtractFileContent(MidiProfilesFilePath);

        ryml::Tree MidiProfilesTree;
        MidiProfilesTree.reserve(INITIAL_TREE_NODE_COUNT);
        MidiProfilesTree.reserve_arena(INITIAL_TREE_ARENA_CHAR_COUNT);
        ryml::parse_in_arena(ryml::to_csubstr(Content), &MidiProfilesTree);

        const ryml::ConstNodeRef Root = MidiProfilesTree.rootref();
        if (Root.is_map() && Root.has_child(MidiDeviceProfile.GetName().c_str()))
        {
            const ryml::ConstNodeRef MidiProfileNode = Root[MidiDeviceProfile.GetName().c_str()];

            if (MidiProfileNode.has_child(MIDI_PROFILE_INPUT_PROPERTIES_NODE_NAME))
            {
                const ryml::ConstNodeRef MidiProfileInputPropertiesNode = MidiProfileNode[MIDI_PROFILE_INPUT_PROPERTIES_NODE_NAME];
                MidiDeviceProfile.InputProperties.clear();
                MidiDeviceProfile.InputProperties.reserve(MidiProfileInputPropertiesNode.num_children());

                for (int ChildPos = 0; ChildPos < MidiProfileInputPropertiesNode.num_children(); ChildPos++)
                {
                    const ryml::ConstNodeRef MidiProfileInputPropertyNode = MidiProfileInputPropertiesNode.at(ChildPos);
                    IEMidiDeviceInputProperty& MidiDeviceInputProperty = MidiDeviceProfile.InputProperties.emplace_back(MidiDeviceProfile.GetName());

                    if (MidiProfileInputPropertyNode.has_child(MIDI_MESSAGE_TYPE_KEY_NAME))
                    {
                        uint8_t MidiMessageType = 0;
                        MidiProfileInputPropertyNode[MIDI_MESSAGE_TYPE_KEY_NAME] >> MidiMessageType;
                        MidiDeviceInputProperty.MidiMessageType = static_cast<IEMidiMessageType>(MidiMessageType);
                    }

                    if (MidiProfileInputPropertyNode.has_child(MIDI_TOGGLE_KEY_NAME))
                    {
                        MidiProfileInputPropertyNode[MIDI_TOGGLE_KEY_NAME] >> MidiDeviceInputProperty.bIsMidiToggle;
                    }

                    if (MidiProfileInputPropertyNode.has_child(MIDI_ACTION_TYPE_KEY_NAME))
                    {
                        uint8_t MidiActionType = 0;
                        MidiProfileInputPropertyNode[MIDI_ACTION_TYPE_KEY_NAME] >> MidiActionType;
                        MidiDeviceInputProperty.MidiActionType = static_cast<IEMidiActionType>(MidiActionType);
                    }

                    if (MidiProfileInputPropertyNode.has_child(CONSOLE_COMMAND_KEY_NAME))
                    {
                        if (!MidiProfileInputPropertyNode[CONSOLE_COMMAND_KEY_NAME].val().empty())
                        {
                            MidiProfileInputPropertyNode[CONSOLE_COMMAND_KEY_NAME] >> MidiDeviceInputProperty.ConsoleCommand;
                        }
                    }

                    if (MidiProfileInputPropertyNode.has_child(OPEN_FILE_PATH_KEY_NAME))
                    {
                        if (!MidiProfileInputPropertyNode[OPEN_FILE_PATH_KEY_NAME].val().empty())
                        {
                            MidiProfileInputPropertyNode[OPEN_FILE_PATH_KEY_NAME] >> MidiDeviceInputProperty.OpenFilePath;
                        }
                    }

                    if (MidiProfileInputPropertyNode.has_child(MIDI_MESSAGE_KEY_NAME))
                    {
                        MidiProfileInputPropertyNode[MIDI_MESSAGE_KEY_NAME] >> MidiDeviceInputProperty.MidiMessage;
                    }
                }
            }

            if (MidiProfileNode.has_child(MIDI_PROFILE_OUTPUT_PROPERTIES_NODE_NAME))
            {
                const ryml::ConstNodeRef MidiProfileOutputPropertiesNode = MidiProfileNode[MIDI_PROFILE_OUTPUT_PROPERTIES_NODE_NAME];
                MidiDeviceProfile.OutputProperties.clear();
                MidiDeviceProfile.OutputProperties.reserve(MidiProfileOutputPropertiesNode.num_children());

                for (int ChildPos = 0; ChildPos < MidiProfileOutputPropertiesNode.num_children(); ChildPos++)
                {
                    const ryml::ConstNodeRef MidiProfileOutputPropertyNode = MidiProfileOutputPropertiesNode.at(ChildPos);
                    IEMidiDeviceOutputProperty& MidiDeviceOutputProperty = MidiDeviceProfile.OutputProperties.emplace_back(MidiDeviceProfile.GetName());

                    if (MidiProfileOutputPropertyNode.has_child(MIDI_MESSAGE_KEY_NAME))
                    {
                        MidiProfileOutputPropertyNode[MIDI_MESSAGE_KEY_NAME] >> MidiDeviceOutputProperty.MidiMessage;
                    }
                }
            }

            Result.Type = IEResult::Type::Success;
            Result.Message = std::format("Successfully loaded profile {} from {}", MidiDeviceProfile.GetName(), MidiProfilesFilePath.string());
        }
    }
    return Result;
}

IEResult IEMidiProfileManager::RemoveProfile(const IEMidiDeviceProfile& MidiDeviceProfile) const
{
    IEResult Result(IEResult::Type::Fail, "Failed to remove profile");
    const std::filesystem::path IEMidiConfigFolderPath; // = IEUtils::GetIEConfigFolderPath();
    // TODO
    // if (!IEMidiConfigFolderPath.empty())
    // {
    //     const std::filesystem::path MidiProfilesFilePath = IEMidiConfigFolderPath / IEMIDI_PROFILES_FILENAME;
    //     if (std::filesystem::exists(MidiProfilesFilePath))
    //     {
    //         const std::string Content = ExtractFileContent(MidiProfilesFilePath);

    //         ryml::Tree MidiProfilesTree;
    //         MidiProfilesTree.reserve(INITIAL_TREE_NODE_COUNT);
    //         MidiProfilesTree.reserve_arena(INITIAL_TREE_ARENA_CHAR_COUNT);
    //         ryml::parse_in_arena(ryml::to_csubstr(Content), &MidiProfilesTree);

    //         ryml::NodeRef Root = MidiProfilesTree.rootref();
    //         if (Root.is_map())
    //         {
    //             const char* MidiDeviceName = MidiDeviceProfile.Name.c_str();
    //             Root.remove_child(MidiDeviceName);

    //             if (std::FILE* const ProfilesFile = std::fopen(MidiProfilesFilePath.string().c_str(), "w"))
    //             {
    //                 if (!Root.empty())
    //                 {
    //                     size_t EmitSize = ryml::emit_yaml(MidiProfilesTree, ProfilesFile);
    //                 }

    //                 Result.Type = IEResult::Type::Success;
    //                 Result.Message = std::format("Successfully removed profile {}, from {}", MidiDeviceProfile.Name, MidiProfilesFilePath.string());
                    
    //                 std::fclose(ProfilesFile);
    //             }
    //         }
    //     }
    // }
    return Result;
}

std::string IEMidiProfileManager::ExtractFileContent(const std::filesystem::path& FilePath) const
{
    std::string Content;
    if (std::FILE* const File = std::fopen(FilePath.string().c_str(), "rb"))
    {
        std::fseek(File, 0, SEEK_END);
        const long Size = std::ftell(File);
        if (Size)
        {
            Content.resize(Size);
            std::rewind(File);
            std::fread(Content.data(), 1, Size, File);
        }
        std::fclose(File);
    }
    return Content;
}