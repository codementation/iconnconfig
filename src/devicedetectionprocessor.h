#ifndef DEVICEDETECTIONPROCESSOR_H
#define DEVICEDETECTIONPROCESSOR_H

#include "RtMidi.h"
#include "device.h"
#include "libusb.h"

#include <QEvent>
#include <QWidget>

class DeviceDetectionProcessor {
public:
	DeviceDetectionProcessor(QWidget *gui);
	~DeviceDetectionProcessor();
	unsigned int getMidiInPortCount();
	unsigned int getMidiOutPortCount();
	void startDeviceDetection();

private:
	// Members
	RtMidiIn *midiin = 0;
	RtMidiOut *midiout = 0;

	libusb_device **devs;

	// methods
	bool isIconnectivityDevice(BYTE_VECTOR *message);

	// MIDI-methods
	void createMidiIn();
	void createMidiOut();
	void setupMidiPorts();
	unsigned long detectDevices();
	double getMessage(BYTE_VECTOR *message);

	// USB-methods
	bool setupUSB();
	void printUSBDevs();

	QWidget *gui;

	void sendProgressEvent(unsigned int progress);
};

#endif// DEVICEDETECTIONPROCESSOR_H
