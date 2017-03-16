#ifndef DEVICEDETECTIONPROCESSOR_H
#define DEVICEDETECTIONPROCESSOR_H

#include "RtMidi.h"
#include "device.h"
#include "libusb.h"

class DeviceDetectionProcessor {
public:
  DeviceDetectionProcessor();
  ~DeviceDetectionProcessor();
  void startDeviceDetection();

private:
  // Members
  RtMidiIn *midiin = 0;
  RtMidiOut *midiout = 0;

  libusb_device **devs;

  // MIDI-methods
  void createMidiIn();
  void createMidiOut();
  void setupMidiPorts();
  int detectDevices();
  double getMessage(BYTE_VECTOR *message);

  // USB-methods
  bool setupUSB();
  void printUSBDevs();
};

#endif // DEVICEDETECTIONPROCESSOR_H
