<div align="center">
  <picture>
    <source media="(prefers-color-scheme: light)" srcset="https://github.com/Interactive-Echoes/IEResources/raw/master/IE-Brand-Kit/IE-Logo-Alt-NoBg.png?">
    <source media="(prefers-color-scheme: dark)" srcset="https://github.com/Interactive-Echoes/IEResources/raw/master/IE-Brand-Kit/IE-Logo-NoBg.png?">
  <img alt="IELogo" width="128">
  </picture>
</div>

# IEMidi

**IEMidi** is a cross-platform MIDI editor built with Qt and RtMidi.  
The editor is designed to map any MIDI message to various actions such as volume control, mute, console commands or opening files.
<div align="center">
<img src="https://github.com/Interactive-Echoes/IEMidi/raw/master/Resources/Demos/IEMidi-2-Demo-Editor.png" alt="Demo" width="1920"> 
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

## Third-Party Libraries Used
- [Qt](https://github.com/qt)
- [IEActions](https://github.com/Interactive-Echoes/IEActions)
- [IEConcurrency](https://github.com/Interactive-Echoes/IEConcurrency)
- [RtMidi](https://github.com/thestk/rtmidi)
- [Rapid YAML](https://github.com/biojppm/rapidyaml)

## Contribution
Contributors are welcomed to this open-source project. Any feedback or assistance, whether in coding, packaging, documentation, design, or testing, is greatly appreciated. 

## License
This work is licensed under the [SPDX-License-Identifier: GPL-2.0-only](./LICENSE).
