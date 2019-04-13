#include "audiocontrolparmwidget.h"
#include "audiocontrolparmfeatureswidget.h"
#include "ui_audiocontrolparmwidget.h"

#include <QLabel>
#include <QLayout>
#include <QScrollArea>

AudioControlParmWidget::AudioControlParmWidget(
	Device *device, unsigned int portId,
	std::shared_ptr<RetSetAudioDeviceParm> retSetAudioDeviceParm,
	QWidget *parent)
	: QWidget(parent), m_pDevice(device), m_iPortId(portId),
	  m_pRetSetAudioDeviceParm(retSetAudioDeviceParm) {
	m_pAudioControlParms =
		new QMap<AudioControllerType,
				 QVector<std::shared_ptr<RetSetAudioControlParm>> *>();
	createLayout();
	loadAudioControlParms();
	addAudioControllerSections();
}

AudioControlParmWidget::~AudioControlParmWidget() {
	m_pAudioControlParms->clear();
	delete m_pAudioControlParms;
}

void AudioControlParmWidget::createLayout() {
	m_pLayout = new QVBoxLayout();
	setLayout(m_pLayout);
	m_pFeatureTabWidget = new QTabWidget();
	m_pLayout->addWidget(m_pFeatureTabWidget);
}

void AudioControlParmWidget::loadAudioControlParms() {
	std::unique_ptr<GetAudioControlParm> getAudioControlParm =
		std::make_unique<GetAudioControlParm>(m_pDevice);
	unsigned int maxControllers = m_pRetSetAudioDeviceParm->getMaxControllers();
	getAudioControlParm->setPortId(static_cast<unsigned int>(m_iPortId));
	for (unsigned int controllerNumber = 1; controllerNumber <= maxControllers;
		 controllerNumber++) {
		getAudioControlParm->setControllerNumber(controllerNumber);
		std::shared_ptr<RetSetAudioControlParm> retSetAudioControlParm =
			std::dynamic_pointer_cast<RetSetAudioControlParm>(
				getAudioControlParm->querySmart());
		if (retSetAudioControlParm == nullptr)
			break;
		addAudioControlParm(retSetAudioControlParm);
	}
}

void AudioControlParmWidget::addAudioControlParm(
	std::shared_ptr<RetSetAudioControlParm> retSetAudioControlParm) {
	AudioControllerType controllerType =
		retSetAudioControlParm->getControllerType();
	QVector<std::shared_ptr<RetSetAudioControlParm>> *v =
		m_pAudioControlParms->value(controllerType);
	if (!v) {
		v = new QVector<std::shared_ptr<RetSetAudioControlParm>>();
		m_pAudioControlParms->insert(controllerType, v);
	}
	v->push_back(retSetAudioControlParm);
}

void AudioControlParmWidget::addAudioControllerSections() {
	QMap<AudioControllerType,
		 QVector<std::shared_ptr<RetSetAudioControlParm>> *>::iterator i;
	for (i = m_pAudioControlParms->begin(); i != m_pAudioControlParms->end();
		 ++i) {
		AudioControllerType type = i.key();
		switch (type) {
		case CT_FEATUERE: {
			AudioControlParmFeaturesWidget *featureWidget =
				new AudioControlParmFeaturesWidget(m_pDevice, i.value());
			this->m_pFeatureTabWidget->addTab(featureWidget,
											  getFeatureName(type));
		} break;
		default:
			break;
		}
	}
}

QString AudioControlParmWidget::getFeatureName(
	AudioControllerType audioControllerType) {
	switch (audioControllerType) {
	case CT_FEATUERE:
		return tr("Features");
	case CT_SELECTOR:
		return tr("Selector");
	case CT_CLOCK_SOURCE:
		return tr("ClockSource");
	case CT_NONE:
		return tr("Unknown");
	}
	return "";
}
