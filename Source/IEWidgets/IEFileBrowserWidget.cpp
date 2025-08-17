// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEFileBrowserWidget.h"

#include "qboxlayout.h"
#include "qfiledialog.h"
#include "qlineedit.h"
#include "qpushbutton.h"

IEFileBrowserWidget::IEFileBrowserWidget(QWidget* Parent) :
    QWidget(Parent)
{
    m_FilePathTextWidget = new QLineEdit(this);
    m_FilePathTextWidget->setPlaceholderText("File");
    m_FilePathTextWidget->connect(m_FilePathTextWidget, &QLineEdit::editingFinished, this, &IEFileBrowserWidget::OnTextCommitted);
    
    m_BrowseButtonWidget = new QPushButton("...", this);
    m_BrowseButtonWidget->setMaximumWidth(30);
    m_BrowseButtonWidget->connect(m_BrowseButtonWidget, &QPushButton::pressed, this, &IEFileBrowserWidget::OnBrowseButtonPressed);
    
    QHBoxLayout* const Layout = new QHBoxLayout(this);
    Layout->setContentsMargins(0, 0, 0, 0);
    Layout->setSpacing(5);
    Layout->addWidget(m_FilePathTextWidget);
    Layout->addWidget(m_BrowseButtonWidget);
}

void IEFileBrowserWidget::SetFilePath(const std::filesystem::path& FilePath) const
{
    if (m_FilePathTextWidget)
    {
        if (std::filesystem::exists(FilePath))
        {
            m_FilePathTextWidget->setText(FilePath.string().c_str());
            m_FilePathTextWidget->update();
        }
    }
}

std::filesystem::path IEFileBrowserWidget::GetFilePath() const
{
    if (m_FilePathTextWidget)
    {
        const std::filesystem::path& FilePath = std::filesystem::path(m_FilePathTextWidget->text().toStdString());
        if (std::filesystem::exists(FilePath))
        {
            return FilePath;
        }
    }
    return std::filesystem::path();
}

void IEFileBrowserWidget::OnBrowseButtonPressed()
{
    const QString SelectedFilePath = QFileDialog::getOpenFileName(this, "Select File");
    if (!SelectedFilePath.isEmpty())
    {
        m_FilePathTextWidget->setText(SelectedFilePath);
        emit m_FilePathTextWidget->editingFinished();
    }
}

void IEFileBrowserWidget::OnTextCommitted() const
{
    emit OnFilePathCommitted();
}
