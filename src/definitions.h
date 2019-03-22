#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <vector>

/**
 * @brief The Command enum
 */
enum Command {
	CMD_ERROR = -1, /*!< Return if command is not valid */
	/* device commands */
	GET_DEVICE = 0x01,      /*!< Query for devices  */
	RET_DEVICE,             /*!< Answer from device to GET_DEVICE query*/
	GET_COMMAND_LIST,       /*!< Request devices command list. */
	RET_COMMAND_LIST,       /*!< List of commands supported by device. */
	GET_INFO_LIST,          /*!< Request list of infos, whch can be retrieved */
	RET_INFO_LIST,          /*!< List of infos supported by this device */
	GET_INFO,               /*!< Get a single information from device */
	RET_SET_INFO,           /*!< Return a single information from device */
	GET_RESET_LIST,         /*!< Request list of reset IDs */
	RET_RESET_LIST,         /*!< Return list of reset IDs */
	GET_SAVE_RESTORE_LIST,  /*!< Request the save / restore ids of */
	RET_SAVE_RESTORE_LIST,  /*!< Returns a list of the save/restore ids */
	GET_ETHERNET_PORT_INFO, /*!< Requests the information about a specific
							ethernet port */
	RET_SET_ETHERNET_PORT_INFO, /*!< Returns the information about a
								specific ethernet port or sets some
								values for a specific ethernet port */
	ACK,             /*!< Returns a sucess code (0x00) or an error code (0x01 -
				   0x03) */
	RESET,           /*!< Resets the device */
	SAVE_RESTORE,    /*!< Used to save or restore the current device
							configuration */
	GET_GIZMO_COUNT, /*!< Number of other devices connected to this device
					  */
	RET_GIZMO_COUNT, /*!< Returns the number of devices that are
					connected to this device */
	GET_GIZMO_INFO,  /*!< Queueries information about a connected
					device */
	RET_GIZMO_INFO,  /*!< Returns information about a connected
					device */
	/* MIDI Commands */
	GET_MIDI_INFO =
		0x20,           /*!< Queueries information about the midi parameters */
	RET_SET_MIDI_INFO,  /*!< Returns information about the midi parameters */
	GET_MIDI_PORT_INFO, /*!< Queuery the information about a specific MIDI
						Port */
	RET_SET_MIDI_PORT_INFO,   /*!< Returns or set the MIDI-Information
								  for a specific port */
	GET_MIDI_PORT_FILTER,     /*!< Queueries infomation about current midi port
									 filters for a specific port */
	RET_SET_MIDI_PORT_FILTER, /*!< Returns or writes infomation about
								current midi port filters for a specific
								port*/
	GET_MIDI_PORT_REMAP,      /*!< Queries remaps of MIDI ports */
	RET_SET_MIDI_PORT_REMAP,  /*!< Set the remapping of midi ports */
	GET_MIDI_PORT_ROUTE,      /*!< Queries the routing of a specific port */
	RET_SET_MIDI_PORT_ROUTE,  /*!< sets the routing destinations from a
								speific port */
	GET_MIDI_PORT_DETAIL,     /*!< get the details of a specific MIDI port */
	RET_SET_MIDI_PORT_DETAIL, /*!< set details of a specific MIDI port */
	GET_RTP_MIDI_CONNECTION_DETAIL, /*!< query details of a RTP MIDI port */
	RET_RTP_MIDI_CONNECTION_DETAIL, /*!< retrieve the details of an RTP MIDI
									port */
	GET_USB_HOST_MIDI_DETAIL,       /*!< query details of an USB host */
	RET_USB_HOST_MIDI_DETAIL,       /*!< retrieve the details of an USB host */

	GET_AUDIO_CHANNEL_NAME = 0x3c, /*!< query the audio channel name */
	RET_SET_AUDIO_CHANNEL_NAME, /*!< retrieve or set the pthe audio channel name
								 */
	GET_AUDIO_PORT_METER_VALUE, /*!< query the meter value of an audio channel
								 */
	RET_AUDIO_PORT_METER_VALUE, /*!< retrieve or set the meter value of an audio
								  channel */

	GET_AUDIO_GLOBAL_PARM =
		0x40, /*!< query global params for the audio section of an device */
	RET_SET_AUDIO_GLOBAL_PARM, /*!< retieve or set global params for the audio
								 section of an device */
	GET_AUDIO_PORT_PARM,       /*!< query the parameters for an audio port */
	RET_SET_AUDIO_PORT_PARM,   /*!< retrieve or set parameters for an audio port
								*/
	GET_AUDIO_DEVICE_PARM,     /*!< query the parameters for an audio port */
	RET_SET_AUDIO_DEVICE_PARM, /*!< retrieve or set parameters for an audio port
								*/
	GET_AUDIO_CONTROL_PARM,    /*!< query the controllers for an audio port */
	RET_SET_AUDIO_CONTROL_PARM,   /*!< retrieve or set controllers for an audio
									 port */
	GET_AUDIO_CONTROL_DETAIL,     /*!< query the parameters for an audio control
									 detail */
	RET_SET_AUDIO_CONTROL_DETAIL, /*!< retrieve or set parameters for an audio
									 control detail */
	GET_AUDIO_CONTROL_DETAIL_VALUE, /*!< query the values for an control detail
									 */
	RET_SET_AUDIO_CONTROL_DETAIL_VALUE, /*!< retrieve or set values for an
										  control detail */
	GET_AUDIO_CLOCK_PARM,     /*!< query the parameters for an audio clock
							   */
	RET_SET_AUDIO_CLOCK_PARM, /*!< retrieve or set values for an
										  control detail */
	GET_AUDIO_PATCHBAY_PARM, /*!< query the parameters for the audio patchbay */
	RET_SET_AUDIO_PATCHBAY_PARM /*!< retrieve or set the parameters for the
									audio patchbay */

};

enum AudioPortType {
	APT_NONE = 0,
	APT_USB_DEVICE = 2,
	APT_USB_HOST,
	APT_ETHERNET,
	APT_ANALOGUE
};

enum AudioControllerType {
	CT_NONE = 0,
	CT_SELECTOR = 5,
	CT_FEATUERE = 6,
	CT_CLOCK_SOURCE = 10
};

typedef struct {
	int channel;
	int volume;
} ChannelVolume;

typedef struct {
	std::vector<ChannelVolume> in;
	std::vector<ChannelVolume> out;
} ChannelVolumes;

enum ChannelDirection { CD_NONE = 0, CD_INPUT, CD_OUTPUT };

#endif // DEFINITIONS_H
