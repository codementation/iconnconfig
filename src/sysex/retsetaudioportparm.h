#ifndef RETSETAUDIOPORTPARM_H
#define RETSETAUDIOPORTPARM_H

#include "../device.h"
#include "sysexmessage.h"

typedef struct {
	unsigned int audioConfigurationNumber = 0;
	unsigned int maxAudioChannelsSupported = 0;
	unsigned int minInputChannelsSupported = 0;
	unsigned int maxInputChannelsSupported = 0;
	unsigned int minOutputChannelsSupported = 0;
	unsigned int maxOutputChannelsSupported = 0;
} AudioPortConfiguration;

class RetSetAudioPortParm : public SysExMessage {
  public:
	RetSetAudioPortParm(Device *device);
	RetSetAudioPortParm(Command cmd, BYTE_VECTOR *message, Device *device)
	    : SysExMessage(cmd, message, device) {}

	~RetSetAudioPortParm() override;

  public:
	// methods
	int getSettingsId() override { return m_Command; }
	int getSettingsIndex() override { return m_iPortId; }
	void parseAnswerData() override;
	std::string getStorableValue() override { return ""; }
	// SysExMessage interface
	std::vector<unsigned char> *getMessageData() override;

	std::string getAudioPortTypeName(AudioPortType audioPortType);
	std::string getAudioPortTypeName();

	bool getPortNameWritable();

	AudioPortConfiguration *getCurrentAudioConfiguration();
	std::string getCurrentAudioConfigurationString();

	// Getter & Setter
	AudioPortType getAudioPortType() const;

	std::string getPortName() const;
	void setPortName(const std::string &sPortName);

	bool getPortSupportsIOS() const;

	bool getPortSupportsPC() const;

	bool getPortIOSEnabled() const;
	void setPortIOSEnabled(bool bPortIOSEnabled);

	bool getPortPCEnabled() const;
	void setPortPCEnabled(bool bPortPCEnabled);

	int getDeviceSpecficPortNumber() const;

	int getJackSpecificDeviceNumber() const;

	int getPortId() const;

	int getMaxPortNameLength() const;

	unsigned int getInputChannels() const;
	void setInputChannels(unsigned int iInputChannels);

	unsigned int getOutputChannels() const;
	void setOutputChannels(unsigned int iOutputChannels);

	int getNumberOfPortConfigurationBlocks() const;

  private:
	int m_iPortId;
	AudioPortType m_audioPortType = APT_NONE;
	unsigned int m_iInputChannels = 0;
	unsigned int m_iOutputChannels = 0;
	int m_iNumberOfPortConfigurationBlocks = 0;
	int m_iMaxPortNameLength = 0;
	int m_iPortNameLength = 0;
	int m_iDeviceSpecficPortNumer = 0;
	int m_iJackSpecificDeviceNumber = 0;

	bool m_bPortIOSEnabled = false;
	bool m_bPortPCEnabled = false;
	bool m_bPortSupportsIOS = false;
	bool m_bPortSupportsPC = false;

	AudioPortConfiguration **m_pAudioPortConfigurations = nullptr;
	std::string m_sPortName;
};

#endif // RETSETAUDIOPORTPARM_H
