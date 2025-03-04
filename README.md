<div align="center">
  <picture>
    <source media="(prefers-color-scheme: light)" srcset="https://github.com/Interactive-Echoes/IECore/raw/master/Resources/IE-Brand-Kit/IE-Logo-Alt-NoBg.png?">
    <source media="(prefers-color-scheme: dark)" srcset="https://github.com/Interactive-Echoes/IECore/raw/master/Resources/IE-Brand-Kit/IE-Logo-NoBg.png?">
  <img alt="IELogo" width="128">
  </picture>
</div>

# IEMidi

**IEMidi** is a cross-platform MIDI editor that leverages RtMidi for its MIDI message handling and ImGui for its rendering backend.  
The editor is designed to map any MIDI message to various actions such as volume control, mute, console commands or opening files. It also includes a MIDI logger for monitoring MIDI messages in real-time.
<div align="center">
<img src="https://github.com/Interactive-Echoes/IEMidi/raw/master/Resources/Demos/IEMidi-Demo-Editor.png" alt="Demo" width="1920"> 
</div>

## Installation

### Windows 

Download the win installer from:
```sh
https://interactive-echoes.github.io/Downloads/Win/IEMidi/amd64/IEMidi-1.2.0-win64.zip
```
To ensure the integrity and authenticity of the IEMidi installer, you can run the Verify.bat file provided by the package.

### Linux

#### RPM-Based Distributions (RHEL/CentOS/Fedora)

```sh
sudo wget -O /etc/yum.repos.d/IE.repo https://interactive-echoes.github.io/Downloads/Linux/RPM/IE.repo
sudo dnf install iemidi
```

#### DEB-Based Distributions (Debian/Ubuntu)

```sh
sudo wget -O /etc/apt/trusted.gpg.d/ie-public.gpg https://interactive-echoes.github.io/Downloads/ie-public.gpg
sudo wget -O /etc/apt/sources.list.d/IE.list https://interactive-echoes.github.io/Downloads/Linux/DEB/IE.list
sudo apt update
sudo apt install iemidi
```

## Features
- **MIDI Map Editor**: Map MIDI messages to various actions like volume, mute, console commands, or opening files.
- **MIDI Logger**: Monitor and log MIDI messages in real-time for debugging and analysis.
- **Run in background**: Activate your MIDI device and keep the application running in the background.

## Third-Party Libraries Used
- [IECore](https://github.com/mozahzah/IECore.git)
- [IEActions](https://github.com/mozahzah/IEActions.git)
- [RtMidi](https://github.com/thestk/rtmidi)
- [Rapid YAML](https://github.com/biojppm/rapidyaml)

## Contribution
Contributors are welcomed to this open-source project. Any feedback or assistance, whether in coding, packaging, documentation, design, or testing, is greatly appreciated. 

## License
This work is licensed under the [SPDX-License-Identifier: GPL-2.0-only](./LICENSE).
