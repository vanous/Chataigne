/*
  ==============================================================================

    CommonIncludes.cpp
    Created: 10 Mar 2021 9:55:09am
    Author:  bkupe

  ==============================================================================
*/

#include "CommonIncludes.h"

#include "DMX/DMXManager.cpp"
#include "DMX/device/DMXDevice.cpp"
#include "DMX/device/DMXSerialDevice.cpp"
#include "DMX/device/DMXArtNetDevice.cpp"
#include "DMX/device/DMXEnttecProDevice.cpp"
#include "DMX/device/DMXOpenUSBDevice.cpp"

#include "MIDI/MIDIDevice.cpp"
#include "MIDI/MIDIDeviceParameter.cpp"
#include "MIDI/MIDIManager.cpp"
#include "MIDI/MTCReceiver.cpp"
#include "MIDI/MTCSender.cpp"
#include "MIDI/ui/MIDIDeviceChooser.cpp"
#include "MIDI/ui/MIDIDeviceParameterUI.cpp"

#include "Serial/SerialDevice.cpp"
#include "Serial/SerialDeviceParameter.cpp"
#include "Serial/SerialManager.cpp"
#include "Serial/lib/cobs/cobs.cpp"
#include "Zeroconf/ZeroconfManager.cpp"
