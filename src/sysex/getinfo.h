#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include "retinfolist.h"
#include "retsetinfo.h"
#include "sysexmessage.h"

class GetInfo : public SysExMessage {
	Q_DECLARE_TR_FUNCTIONS(InfoItem)

  public:
	GetInfo(Device *m_pDevice);
	GetInfo(Device *m_pDevice, std::shared_ptr<RetInfoList> m_pInfoList);
	GetInfo(Command cmd, BYTE_VECTOR *message, Device *device)
		: SysExMessage(cmd, message, device) {}
	~GetInfo();

	// methods
	void setInfoItem(DeviceInfoItem infoItem) { this->m_InfoItem = infoItem; }
	BYTE_VECTOR *getMessageData();
	std::string getDataAsString();
	DeviceInfoItem getDeviceInfoItem();
	std::map<DeviceInfoItem, std::shared_ptr<RetSetInfo>> *getRetSetInfos();
	void deviceInfoChanged(DeviceInfoItem item, std::string value);
	std::string getItemValue(SysExMessage::DeviceInfoItem item);
	int getSettingsId() { return m_Command; }
	int getSettingsIndex() { return 0; }
	std::string getStorableValue() { return ""; }

	// methods
  private:
	void createAnswer(Command m_Command, BYTE_VECTOR *message,
					  Device *m_pDevice);
	void setInfoData(DeviceInfoItem item, std::string value);

  private:
	DeviceInfoItem m_InfoItem = DEVICE_NAME;
	std::shared_ptr<RetInfoList> m_pInfoList = nullptr;
	std::map<DeviceInfoItem, std::shared_ptr<RetSetInfo>> *m_pRetSetInfos;
};

#endif // DEVICEINFO_H
