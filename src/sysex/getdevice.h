#ifndef DEVICEDETECTIONQUERY_H
#define DEVICEDETECTIONQUERY_H

#include "midi.h"
#include "sysexmessage.h"

class GetDevice : public SysExMessage {
public:
	GetDevice();
	GetDevice(Device *device);
	~GetDevice();
	int getSettingsId() { return cmd; }
	int getSettingsIndex() { return 0; }
	std::string getStorableValue() { return ""; }
};

#endif// DEVICEDETECTIONQUERY_H
