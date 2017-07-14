#include "miomain.h"
#include "config/configuration.h"
#include "config/qsettings-xml.h"
#include "devicedetection.h"
#include "sysex/communicationexception.h"
#include "sysex/midi.h"
#include "sysex/protocolexception.h"
#include "sysex/retcommandlist.h"
#include "ui_miomain.h"
#include "widgets/centralwidget.h"
#include "widgets/deviceinfowidget.h"
#include "widgets/multiinfowidget.h"
#include "widgets/portswidget.h"

#include <cerrno>  // for errno
#include <csignal> // for sigaction()
#include <cstring> // for strerror()
#include <unistd.h>// for pipe()

#include <QCloseEvent>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QPixmap>
#include <QProgressDialog>
#include <QSettings>
#include <QSignalMapper>
#include <QSocketNotifier>
#include <QStyle>
#include <QTimer>
#include <QToolButton>
#include <QtDebug>

int MioMain::sigpipe[2];

MioMain::MioMain(QCommandLineParser *parser, QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MioMain) {
	ui->setupUi(this);
	setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::ForceTabbedDocks |
				   QMainWindow::VerticalTabs);
	/*QPixmap *pm = new QPixmap("/develop/mioconfig/graphik/restore.svg");
	pm->save("/develop/mioconfig/graphik/restore.xpm", "xpm");
	pm->load("/develop/mioconfig/graphik/SaveToDevice.svg");
	pm->save("/develop/mioconfig/graphik/SaveToDevice.xpm", "xpm");*/
	if (parser->isSet("filename")) {
		QString *fileName = new QString(parser->value("filename"));
		setConfigurationFile(fileName);
	}
	if (configurationFile) {
		configuration = new QSettings(*configurationFile, xmlFormat);
		configuration->beginGroup("Test");
		configuration->setValue("ConfigName", "lala");
		configuration->endGroup();
	}
	if (!installSignalHandlers())
		qWarning("%s", "Signal handlers not installed!");
	readSettings();
	if (readDevicesFromSettings())
		openDefaultDevice();
	else
		QTimer::singleShot(100, this, SLOT(openDetectionWindow()));
}

MioMain::~MioMain() {
	if (deviceDetectionWindow)
		delete deviceDetectionWindow;
	delete ui;
}

void MioMain::openDefaultDevice() {
	writeDevicesToSettings();
	long defaultDeviceSN = Configuration::getInstance().getDefaultDevice();
	Device *d = Configuration::getInstance().getDevices()->at(defaultDeviceSN);
	addDevicesToSelectionMenu(defaultDeviceSN);
	openDeviceGUI(d);
}

void MioMain::addDevicesToSelectionMenu(long defaultDeviceSN) {
	QSignalMapper *signalMapper = new QSignalMapper();
	Devices *devices = Configuration::getInstance().getDevices();
	QActionGroup *devicesGroup = new QActionGroup(this);
	devicesGroup->setExclusive(true);
	for (Devices::iterator it = devices->begin(); it != devices->end(); ++it) {
		Device *d = it->second;
		QAction *a = ui->menuSelect->addAction(
			QString::fromStdString(d->getDeviceName()));
		a->setCheckable(true);
		devicesGroup->addAction(a);
		connect(a, SIGNAL(triggered()), signalMapper, SLOT(map()));
		signalMapper->setMapping(a, new DeviceMenuMapper(d));
		if (it->first == defaultDeviceSN)
			a->setChecked(true);
	}
	connect(signalMapper, SIGNAL(mapped(QObject *)), this,
			SLOT(openDeviceGUI(QObject *)));
}

void MioMain::openDeviceGUI(QObject *o) {
	DeviceMenuMapper *m = dynamic_cast<DeviceMenuMapper *>(o);
#ifdef __MIO_DEBUG__
	std::cout << "open device GUI: " << m->device->getDeviceName() << std::endl;
#endif//__MIO_DEBUG__
	openDeviceGUI(m->device);
}

void MioMain::addDock(QDockWidget *dockWidget, Qt::DockWidgetArea area) {
	if (MultiInfoWidget *miw = dynamic_cast<MultiInfoWidget *>(dockWidget)) {
		miw->createInfoSections();
	}
	switch (area) {
	case Qt::NoDockWidgetArea:
		setCentralWidget(dockWidget);
		break;
	case Qt::LeftDockWidgetArea:
		this->addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
		break;
	default:
		break;
	}
	std::vector<QDockWidget *> v = dockWidgetAreas[area];
	if (v.size() > 0) {
		tabifyDockWidget(v[v.size() - 1], dockWidget);
	}
	dockWidgetAreas[area].push_back(dockWidget);
}

void MioMain::clearDocWidgets() {
	removeToolBar(toolBar);
	delete toolBar;
	toolBar = 0;
	for (std::map<Qt::DockWidgetArea, std::vector<QDockWidget *>>::iterator it =
			 dockWidgetAreas.begin();
		 it != dockWidgetAreas.end(); ++it) {
		std::vector<QDockWidget *> v = it->second;
		for (unsigned int j = 0; j < v.size(); j++) {
			QWidget *w = v.at(j);
			delete w;
		}
		v.clear();
	}
	dockWidgetAreas.clear();
}

void MioMain::replacePanel(QWidget *w) {
	CentralWidget *cw = dynamic_cast<CentralWidget *>(centralWidget());
	cw->replacePanel(w);
}

void MioMain::addDeviceToolButtons() {
	BYTE_VECTOR *saveRestoreList = this->currentDevice->saveRestoreList;
	for (unsigned int i = 0; i < saveRestoreList->size(); ++i) {
		switch ((SaveRestore::SaveResstoreId)(*saveRestoreList)[i]) {
		case SaveRestore::SAVE_TO_DEVICE: {
			QToolButton *btn = new QToolButton();
			btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
			btn->setText("Save");
			btn->setToolTip(tr("Save current settings to device"));
			toolBar->addWidget(btn);
			btn->setIcon(QIcon(":/pixmaps/saveto"));
			connect(btn, SIGNAL(pressed()), this, SLOT(storeToDevice()));
		} break;
		case SaveRestore::RESTORE_FROM_DEVICE: {
			QToolButton *btn = new QToolButton();
			btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
			btn->setText("Restore");
			btn->setToolTip(tr("Restore settings from device"));
			toolBar->addWidget(btn);
			btn->setIcon(QIcon(":/pixmaps/readfrom"));
			connect(btn, SIGNAL(pressed()), this, SLOT(restoreFromDevice()));
		} break;
		case SaveRestore::SET_TO_FACTORY_DEFAULT: {
			QToolButton *btn = new QToolButton();
			btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
			btn->setText("Fact");
			btn->setToolTip(tr("Reset settings to factory default"));
			toolBar->addWidget(btn);
			btn->setIcon(QIcon(":/pixmaps/restore"));
			connect(btn, SIGNAL(pressed()), this,
					SLOT(resetToFactoryDefaults()));
		} break;
		default:
			break;
		}
	}
}

void MioMain::openDeviceGUI(Device *d) {
	clearDocWidgets();
	toolBar = new QToolBar(tr("Device Actions"), this);
	toolBar->setObjectName("DeviceActions");
	this->addToolBar(toolBar);

	this->currentDevice = d;
	d->connect();
	RetCommandList *c = d->getCommands();
	if (c == 0) {
		// TODO throw error
		exit(2);
	}
	setWindowTitle(this->title + QString(": ") +
				   QString::fromStdString(d->getDeviceName()));
	CentralWidget *centralWidget = new CentralWidget(this, d);
	this->addDock(centralWidget);

	DeviceInfoWidget *deviceInfoWidget =
		new DeviceInfoWidget(this, d, d->getDeviceInfo());
	this->addDock(deviceInfoWidget, Qt::LeftDockWidgetArea);

	PortsWidget *portsWidget = new PortsWidget(this, d);
	this->addDock(portsWidget, Qt::LeftDockWidgetArea);

	addDeviceToolButtons();

	QSettings *settings = Configuration::getInstance().getSettings();
	settings->beginGroup("MainWindow");
	restoreGeometry(settings->value("geometry").toByteArray());
	settings->endGroup();
	settings->beginGroup("Docks");
	// restoreState(settings->value("DockWindows").toByteArray());
	settings->endGroup();
	deviceInfoWidget->show();
	deviceInfoWidget->raise();
}

void MioMain::storeToDevice() {
	QMessageBox msgBox;
	msgBox.setText(tr("Store current setings to device?"));
	msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Save);
	int ret = msgBox.exec();
	if (ret == QMessageBox::Ok)
		saveRestore(SaveRestore::SAVE_TO_DEVICE);
}

void MioMain::reinitDevice() {
	QProgressDialog progress(
		tr("Waiting 10 seconds for device to be responsive again"),
		tr("Exit application"), 0, 10, this);
	progress.setWindowModality(Qt::WindowModal);

	for (int i = 0; i < 10; i++) {
		SLEEP(1000);
		progress.setValue(i);
		if (progress.wasCanceled())
			exit(-1);
		if (i == 5)
			currentDevice->disconnect();
	}
	currentDevice->connect();
	progress.setValue(10);
	openDeviceGUI(currentDevice);
	bool valid = currentDevice->isDeviceValid();
	if (!valid)
		exit(-1);
}

void MioMain::readConfigurationFromFile() {}

void MioMain::writeConfigurtionToFile() {}

void MioMain::restoreFromDevice() {
	QMessageBox msgBox;
	msgBox.setText(tr("Read all settings from device?"));
	msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Save);
	int ret = msgBox.exec();
	if (ret == QMessageBox::Ok) {
		saveRestore(SaveRestore::RESTORE_FROM_DEVICE);
		reinitDevice();
	}
}

void MioMain::resetToFactoryDefaults() {
	QMessageBox msgBox;
	msgBox.setText(tr("Reset all settings to factory default?"));
	msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Save);
	int ret = msgBox.exec();
	if (ret == QMessageBox::Ok) {
		saveRestore(SaveRestore::SET_TO_FACTORY_DEFAULT);
		reinitDevice();
	}
}

void MioMain::saveRestore(SaveRestore::SaveResstoreId saveRestoreId) {
	SaveRestore *saveRestore = new SaveRestore(currentDevice);
	saveRestore->setSaveRestoreId(saveRestoreId);
	saveRestore->execute();
}

void MioMain::closeEvent(QCloseEvent *event) {
	writeSettings();
	event->accept();
}

void MioMain::openDetectionWindow() {
	deviceDetectionWindow = new DeviceDetection(this);
	deviceDetectionWindow->exec();
}

void MioMain::writeSettings() {
	QSettings *settings = Configuration::getInstance().getSettings();
	settings->beginGroup("MainWindow");
	settings->setValue("geometry", saveGeometry());
	settings->setValue("size", size());
	settings->setValue("pos", pos());
	settings->endGroup();
	settings->beginGroup("Docks");
	settings->setValue("DockWindows", saveState());
	settings->endGroup();
}

void MioMain::writeDevicesToSettings() {
	QSettings *settings = Configuration::getInstance().getSettings();
	Devices *devices = Configuration::getInstance().getDevices();
	settings->remove("Devices");
	settings->beginWriteArray("Devices");
	int i = 0;
	for (Devices::iterator it = devices->begin(); it != devices->end(); ++it) {
		settings->setArrayIndex(i);
		Device *d = it->second;
		settings->setValue("Device Name",
						   QString::fromStdString(d->getDeviceName()));
		settings->setValue(
			"Serial Number",
			static_cast<qlonglong>(d->getSerialNumber()->getLongValue()));
		settings->setValue("Input Port", d->getInPortNumer());
		settings->setValue("Output Port", d->getOutPortNumer());
		settings->setValue(
			"Product Id",
			static_cast<qlonglong>(d->getProductId()->getLongValue()));
		++i;
	}
	settings->endArray();
}

void MioMain::connectSlots() {
	// connect(this->)
}

void MioMain::readSettings() {
	this->title = windowTitle();
	QSettings *settings = Configuration::getInstance().getSettings();
	settings->beginGroup("MainWindow");
	resize(settings->value("size", QSize(400, 400)).toSize());
	move(settings->value("pos", QPoint(200, 200)).toPoint());
	settings->endGroup();
}

bool MioMain::readDevicesFromSettings() {
	Devices *devices = Configuration::getInstance().getDevices();
	devices->clear();
	QSettings *settings = Configuration::getInstance().getSettings();
	int size = settings->beginReadArray("Devices");
	if (size == 0)
		return false;
	for (int i = 0; i < size; ++i) {
		Device *device = 0;
		settings->setArrayIndex(i);
		unsigned int productId =
			static_cast<unsigned int>(settings->value("Product Id").toInt());
		unsigned long serialNumber = static_cast<unsigned long>(
			settings->value("Serial Number").toLongLong());
		unsigned int inputPort =
			static_cast<unsigned int>(settings->value("Input Port").toInt());
		unsigned int outputPort =
			static_cast<unsigned int>(settings->value("Output Port").toInt());
		bool simulate = settings->value("Simulate").toBool();
		if (!simulate)
			device = new Device(inputPort, outputPort, serialNumber, productId);
		if (device)
			try {
				device->queryDeviceInfo();
				devices->insert(
					std::pair<long, Device *>(serialNumber, device));
			} catch (CommunicationException *e) {
				std::cerr << e->getErrorMessage();
				std::cerr << "Device: Product Id: " << productId
						  << ", Serial Number: " << serialNumber
						  << " did not answer" << std::endl;
				std::cerr << "Device not added" << std::endl;
			} catch (ProtocolException *e) {
				std::cerr << e->getErrorMessage();
				std::cerr << "Device: Product Id: " << productId
						  << ", Serial Number: " << serialNumber
						  << " had an error in the communication stream"
						  << std::endl;
				std::cerr << "Device not added" << std::endl;
			}
	}
	settings->endArray();
	if (devices->size() == 0)
		return false;
	return true;
}
void MioMain::on_actionQuit_triggered() { close(); }

/******************************************************************************
 *************** Methods for handling ladish events ***************************
 *****************************************************************************/

/* Handler for system signals (SIGUSR1, SIGINT...)
 * Write a message to the pipe and leave as soon as possible
 */
void MioMain::handleSignal(int sig) {
	if (write(sigpipe[1], &sig, sizeof(sig)) == -1) {
		qWarning("write() failed: %s", std::strerror(errno));
	}
}

/* Install signal handlers (may be more than one; called from the
 * constructor of your MainWindow class*/
bool MioMain::installSignalHandlers() {
	/*install pipe to forward received system signals*/
	if (pipe(sigpipe) < 0) {
		qWarning("pipe() failed: %s", std::strerror(errno));
		return false;
	}

	/*install notifier to handle pipe messages*/
	QSocketNotifier *signalNotifier =
		new QSocketNotifier(sigpipe[0], QSocketNotifier::Read, this);
	connect(signalNotifier, SIGNAL(activated(int)), this,
			SLOT(signalAction(int)));

	/*install signal handlers*/
	struct sigaction action;
	memset(&action, 0, sizeof(action));
	action.sa_handler = handleSignal;

	if (sigaction(SIGUSR1, &action, NULL) == -1) {
		qWarning("sigaction() failed: %s", std::strerror(errno));
		return false;
	}

	/* optional: register more signals to handle: */

	return true;
}

/* Slot to give response to the incoming pipe message;
e.g.: save current file */
void MioMain::signalAction(int fd) {
	int message;

	if (read(fd, &message, sizeof(message)) == -1) {
		qWarning("read() failed: %s", std::strerror(errno));
		return;
	}

	switch (message) {
	case SIGUSR1:
		writeConfigurtionToFile();
		break;
	/* optional: handle more signals: */
	default:
		qWarning("Unexpected signal received: %d", message);
		break;
	}
}
