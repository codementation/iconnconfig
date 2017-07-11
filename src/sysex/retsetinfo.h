#ifndef RETSETINFO_H
#define RETSETINFO_H

#include "sysexmessage.h"

#include <QObject>
#include <QtCore/QCoreApplication>

class RetSetInfo : public SysExMessage {
	Q_DECLARE_TR_FUNCTIONS(InfoItem)

public:
	// Constructors
	RetSetInfo(Device *device);
	RetSetInfo(CommandFlags flags, Device *device);
	RetSetInfo(Command cmd, BYTE_VECTOR *message, Device *device)
		: SysExMessage(cmd, message, device) {}

	// getter
	SysExMessage::DeviceInfoItem getInfoItem();
	std::string getValue();
	bool isItemEditable();
	std::string getItemName();

	// setter
	bool setValue(std::string value);

	// methods
	int getSettingsId() { return cmd; }
	int getSettingsIndex() { return 0; }
	std::string getStorableValue() { return ""; }

protected:
	void parseAnswerData();
	BYTE_VECTOR *getMessageData();

private:
	// variables
	SysExMessage::DeviceInfoItem infoItem;
	std::string value;
};

#endif// RETSETINFO_H
