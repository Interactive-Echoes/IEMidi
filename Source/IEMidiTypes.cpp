// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMidiTypes.h"

IEMidiDeviceInputProperty& IEMidiDeviceProfile::MakeInputProperty()
{
    if (std::shared_ptr<IEMidiDeviceInputProperty> PropPtr = InputPropertiesHead)
    {
        while (PropPtr->Next())
        {
            PropPtr = PropPtr->m_NextProperty;
        }
        PropPtr->m_NextProperty = std::shared_ptr<IEMidiDeviceInputProperty>(new IEMidiDeviceInputProperty(*this, PropPtr));
        return *(PropPtr->m_NextProperty);
    }
    else
    {
        InputPropertiesHead = std::shared_ptr<IEMidiDeviceInputProperty>(new IEMidiDeviceInputProperty(*this, nullptr));
        return *InputPropertiesHead;
    }
}

IEMidiDeviceOutputProperty& IEMidiDeviceProfile::MakeOutputProperty()
{
    if (std::shared_ptr<IEMidiDeviceOutputProperty> PropPtr = OutputPropertiesHead)
    {
        while (PropPtr->Next())
        {
            PropPtr = PropPtr->m_NextProperty;
        }
        PropPtr->m_NextProperty = std::shared_ptr<IEMidiDeviceOutputProperty>(new IEMidiDeviceOutputProperty(*this, PropPtr));
        return *(PropPtr->m_NextProperty);
    }
    else
    {
        OutputPropertiesHead = std::shared_ptr<IEMidiDeviceOutputProperty>(new IEMidiDeviceOutputProperty(*this, nullptr));
        return *OutputPropertiesHead;
    }
}

IEMidiDeviceInputProperty::~IEMidiDeviceInputProperty()
{
    m_PreviousProperty.reset();
    m_NextProperty.reset();
}

IEMidiDeviceInputProperty* IEMidiDeviceInputProperty::Next() const
{
    return m_NextProperty.get();
}

IEMidiDeviceOutputProperty::~IEMidiDeviceOutputProperty()
{
    m_PreviousProperty.reset();
    m_NextProperty.reset();
}

IEMidiDeviceOutputProperty* IEMidiDeviceOutputProperty::Next() const
{
    return m_NextProperty.get();
}

void IEMidiDeviceInputProperty::Delete()
{
    if (m_NextProperty)
    {
        m_NextProperty->m_PreviousProperty = m_PreviousProperty;
    }

    if (std::shared_ptr<IEMidiDeviceInputProperty> const PreviousProperty = m_PreviousProperty.lock())
    {
        PreviousProperty->m_NextProperty = m_NextProperty;
    }
    else
    {
        MidiDeviceProfile.InputPropertiesHead = m_NextProperty;
    }
}

void IEMidiDeviceOutputProperty::Delete()
{
    if (m_NextProperty)
    {
        m_NextProperty->m_PreviousProperty = m_PreviousProperty;
    }

    if (std::shared_ptr<IEMidiDeviceOutputProperty> const PreviousProperty = m_PreviousProperty.lock())
    {
        PreviousProperty->m_NextProperty = m_NextProperty;
    }
    else
    {
        MidiDeviceProfile.OutputPropertiesHead = m_NextProperty;
    }
}