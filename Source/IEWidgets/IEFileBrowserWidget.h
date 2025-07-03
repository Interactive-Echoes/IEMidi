// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include <filesystem>

#include "QWidget.h"

#include "IEMidiTypes.h"

class QLineEdit;
class QPushButton;

class IEFileBrowserWidget : public QWidget
{
    Q_OBJECT
        
public:
    explicit IEFileBrowserWidget(QWidget* Parent = nullptr);

public:
    void SetFilePath(const std::filesystem::path& FilePath) const;
    std::filesystem::path GetFilePath() const;

Q_SIGNALS:
    void OnFilePathCommitted() const;

private Q_SLOTS:
    void OnTextCommitted() const;
    void OnBrowseButtonPressed();

private:
    QLineEdit* m_FilePathTextWidget;
    QPushButton* m_BrowseButtonWidget;
};