#ifndef RETSETMIXERINPUTPARM_H
#define RETSETMIXERINPUTPARM_H

#include "portsysexmessage.h"

class RetSetMixerInputParm : public PortSysExMessage {
  public:
	RetSetMixerInputParm(Device *device);
	RetSetMixerInputParm(Command cmd, BYTE_VECTOR *message, Device *device)
		: PortSysExMessage(cmd, message, device) {}

	// SysExMessage interface
  public:
	void parseAnswerData() override;

	// SysExMessage interface
  protected:
	std::vector<unsigned char> *getMessageData() override;

  private:
	unsigned int m_iMixerInputNumber = 0;
	unsigned int m_iAudioSourcePortId = 0;
	unsigned int m_iAudioSourceChannelNumber = 0;
};

#endif // RETSETMIXERINPUTPARM_H
