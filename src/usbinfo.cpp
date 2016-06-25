// Copyright 2007 Matthew A. Kucenski
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <popt.h>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <boost/lexical_cast.hpp>

#include "libusbInfo/usb.h"
#include "libusbInfo/usbDevice.h"
#include "libusbInfo/usbConfiguration.h"
#include "libusbInfo/usbOtherConfiguration.h"

using namespace std;

#define VERSION "1.0"

#define ERR(msg) (cerr << "ERROR: " << msg << "\n")

wstring deviceSummary(usbDevice* pDevice) {
	wstring rv;
	if (pDevice) {
		rv = L"<" + pDevice->getVendor() + L">, <" + pDevice->getProduct() + L">, Rev. <" + pDevice->getRevision() + L">, S/N <" + pDevice->getSerialNumber() + L">";
	} else {
		ERR("Invalid device");
	}
	return rv;
}

wstring deviceConfigDetails(usbConfiguration* pConfig, wstring strLeader, u_int8_t uiVerboseLevel) {
	wstring rv;
	if (pConfig) {
		rv += strLeader + strLeader + strLeader + L"Description:   " + pConfig->getDescription() + L"\n";
		rv += strLeader + strLeader + strLeader + L"Bus Powered:   " + (pConfig->getBusPowered() ? L"Enabled" : L"Disabled") + L"\n";
		rv += strLeader + strLeader + strLeader + L"Self Powered:  " + (pConfig->getSelfPowered() ? L"Enabled" : L"Disabled") + L"\n";
		rv += strLeader + strLeader + strLeader + L"Remote Wakeup: " + (pConfig->getRemoteWakeup() ? L"Enabled" : L"Disabled") + L"\n";
		rv += strLeader + strLeader + strLeader + L"Max Power:     " + boost::lexical_cast<wstring>(pConfig->getMaxPower()) + L" mA\n";
		if (uiVerboseLevel > 2) {
			for (u_int8_t j = 0; j < pConfig->getInterfaceCount(); j++) {
				usbInterface* pInterface = NULL;
				if (pConfig->getInterface(j, &pInterface) != -1) {
					rv += strLeader + strLeader + strLeader + L"Interface " + boost::lexical_cast<wstring>(j) + L":\n";
					rv += strLeader + strLeader + strLeader + strLeader + L"Description: " + pInterface->getDescription() + L"\n";
					rv += strLeader + strLeader + strLeader + strLeader + L"Class:       " + boost::lexical_cast<wstring>(pInterface->getClass()) + L"\n";
					rv += strLeader + strLeader + strLeader + strLeader + L"SubClass:    " + boost::lexical_cast<wstring>(pInterface->getSubClass()) + L"\n";
					rv += strLeader + strLeader + strLeader + strLeader + L"Protocol:    " + boost::lexical_cast<wstring>(pInterface->getProtocol()) + L"\n";
					if (uiVerboseLevel > 3) {
						for (u_int8_t k = 0; k < pInterface->getEndpointCount(); k++) {
							usbEndpoint* pEndpoint = NULL;
							if (pInterface->getEndpoint(k, &pEndpoint) != -1) {
								rv += strLeader + strLeader + strLeader + strLeader + L"Endpoint " + boost::lexical_cast<wstring>(k) + L":\n";
								rv += strLeader + strLeader + strLeader + strLeader + strLeader + L"Address:           " + boost::lexical_cast<wstring>(pEndpoint->getEndpointNumber()) + L"\n";
								rv += strLeader + strLeader + strLeader + strLeader + strLeader + L"Direction:         " + pEndpoint->getEndpointDirectionString() + L"\n";
								rv += strLeader + strLeader + strLeader + strLeader + strLeader + L"Transfer Type:     " + pEndpoint->getTransferTypeString() + L"\n";
								rv += strLeader + strLeader + strLeader + strLeader + strLeader + L"Sync Type:         " + pEndpoint->getSyncTypeString() + L"\n";
								rv += strLeader + strLeader + strLeader + strLeader + strLeader + L"Usage Type:        " + pEndpoint->getUsageTypeString() + L"\n";
								rv += strLeader + strLeader + strLeader + strLeader + strLeader + L"Max Packet Size:   " + boost::lexical_cast<wstring>(pEndpoint->getMaxPacketSize()) + L" bytes\n";
								rv += strLeader + strLeader + strLeader + strLeader + strLeader + L"Transfer Interval: " + boost::lexical_cast<wstring>(pEndpoint->getDataTransferInterval()) + L" frames\n";
							} else {
								ERR("Failure loading device endpoint.");
							}
						}
					}
				} else {
					ERR("Failure loading device interface.");
				}
			}
		}
	} else {
		ERR("Invalid configuration.");
	}
	return rv;
}

wstring deviceDetails(usbDevice* pDevice, wstring strLeader, u_int8_t uiVerboseLevel) {
	wstring rv;
	if (pDevice) {
		wstring wstrVendor = pDevice->getVendor();
		rv =  strLeader + strLeader + L"Vendor:          " + (wstrVendor.size() > 0 ? wstrVendor + L" " : L"") + L"(" + boost::lexical_cast<wstring>(pDevice->getVendorID()) + L")\n";
		wstring wstrProduct = pDevice->getProduct();
		rv += strLeader + strLeader + L"Product:         " + (wstrProduct.size() > 0 ? wstrProduct + L" " : L"") + L"(" + boost::lexical_cast<wstring>(pDevice->getProductID()) + L")\n";
		rv += strLeader + strLeader + L"Revision:        " + pDevice->getRevision() + L"\n";
		rv += strLeader + strLeader + L"Serial #:        " + pDevice->getSerialNumber() + L"\n";
		rv += strLeader + strLeader + L"USB Spec:        " + pDevice->getUSBSpec() + L"\n";
		rv += strLeader + strLeader + L"Class:           " + boost::lexical_cast<wstring>(pDevice->getClass()) + L"\n";
		rv += strLeader + strLeader + L"SubClass:        " + boost::lexical_cast<wstring>(pDevice->getSubClass()) + L"\n";
		rv += strLeader + strLeader + L"Protocol:        " + boost::lexical_cast<wstring>(pDevice->getProtocol()) + L"\n";
		rv += strLeader + strLeader + L"Max Packet Size: " + boost::lexical_cast<wstring>(pDevice->getMaxPacketSize()) + L" bytes\n";
		if (uiVerboseLevel > 1) {
			for (u_int8_t i = 0; i < pDevice->getConfigCount(); i++) {
				usbConfiguration config;
				if (pDevice->getConfiguration(i, &config) != -1) {
					rv += strLeader + strLeader + L"Configuration " + boost::lexical_cast<wstring>(i) + L":\n";
					rv += deviceConfigDetails(&config, strLeader, uiVerboseLevel);
				} else {
					ERR("Failure loading device configuration.");
				}
	
				usbOtherConfiguration otherConfig;
				if (pDevice->getOtherConfiguration(i, &otherConfig) != -1) {
					rv += strLeader + strLeader + L"Hi-Speed Configuration " + boost::lexical_cast<wstring>(i) + L":\n";
					rv += deviceConfigDetails(&otherConfig, strLeader, uiVerboseLevel);
				} else {
					//ERR("Failure loading device configuration.");  //Not necessarily an error since not all devices have a Hi-Speed configuration.
				}
			}
		}
	} else {
		ERR("Invalid device");
	}
	return rv;
}

void usage(poptContext optCon, int exitcode, const char *error, const char *addl) {
	if (error) {
		cerr << error << ": " << (addl ? addl : "") << "\n\n";
	}
	poptPrintUsage(optCon, stderr, 0);
	exit(exitcode);
}	//void usage(poptContext optCon, int exitcode, const char *error, const char *addl) {

int main(int argc, const char** argv) {
	int rv = EXIT_FAILURE;
	
	bool bScan = true;
	u_int8_t uiVerbose = 0;
	string strHost;
	u_int8_t uiDeviceAddr = 0;
	
	struct poptOption optionsTable[] = {
		{"host-controller",	'h',	POPT_ARG_STRING,	NULL,	10, "Host controller device.", "devicepath"},
		{"device-address",	'd',	POPT_ARG_INT,		NULL,	20,	"Address of device to be displayed.", "address"},
		{"scan", 			's', 	POPT_ARG_NONE,		NULL, 	30, "Display summary of all hosts/devices. Default when no options are specified.", NULL},
		{"verbose",			'v',	POPT_ARG_NONE,		NULL,	40, "May be specified multiple times for additional details.", NULL},
		{"version", 		NULL,	POPT_ARG_NONE,		NULL,	50, "Display usbinfo version.", NULL},
		POPT_AUTOHELP
		{NULL, 0, POPT_ARG_NONE, NULL, 0, NULL, NULL}
	};
	poptContext optCon = poptGetContext(NULL, argc, argv, optionsTable, 0);
	poptSetOtherOptionHelp(optCon, "");

	int iOption = poptGetNextOpt(optCon);
	while (iOption >= 0) {
		switch (iOption) {
			case 10:
				strHost = poptGetOptArg(optCon);
				bScan = false;
				break;
			case 20:
				uiDeviceAddr = strtol(poptGetOptArg(optCon), NULL, 10);
				bScan = false;
				break;
			case 30:
				bScan = true;
				break;
			case 40:
				uiVerbose += 1;
				break;
			case 50:
				cout << "usbinfo version: " << VERSION << "\n";
				exit(0);
				break;
		}
		iOption = poptGetNextOpt(optCon);
	}
	if (iOption != -1) {
		usage(optCon, EXIT_FAILURE, poptBadOption(optCon, POPT_BADOPTION_NOALIAS), poptStrerror(iOption));
	}
	
	if (bScan) { 
		char tmp[16];
		for (unsigned int uiHost = 0; ; uiHost++) {
			snprintf(tmp, sizeof(tmp)-1, "/dev/usb%d", uiHost);
			int hostDev = usbOpenHost(tmp);
			if (hostDev != -1) {
				cout << "Controller " << tmp << ":\n";
				for (u_int8_t uiDeviceAddr = 1; ; uiDeviceAddr++) {
					usbDevice device;
					if (device.load(hostDev, uiDeviceAddr) != -1) {
						if (uiVerbose == 0) {
							wcout << L"   Device " << uiDeviceAddr << L": " << deviceSummary(&device) << L"\n";
						} else {
							wcout << L"   Device " << uiDeviceAddr << L":\n" << deviceDetails(&device, L"   ", uiVerbose) << L"";
						}
					} else {
						break;
					}
				}
				usbCloseHost(hostDev);
			} else {
				break;	
			}	//if (hostDev != -1) {
		}
	} else {
		int hostDev = usbOpenHost(strHost.c_str());
		if (hostDev != -1) {
			cout << "Controller " << strHost << ":\n";
			if (uiDeviceAddr > 0) {	//Single Device
				usbDevice device;
				if (device.load(hostDev, uiDeviceAddr) != -1) {
					if (uiVerbose == 0) {
						wcout << L"   Device " << uiDeviceAddr << L": " << deviceSummary(&device) << L"\n";
					} else {
						wcout << L"   Device " << uiDeviceAddr << L":\n" << deviceDetails(&device, L"   ", uiVerbose) << L"";
					}
				} else {
					ERR("Unable to open device " << uiDeviceAddr);
				}
			} else {				//All devices on this host
				for (u_int8_t uiDeviceAddr = 1; ; uiDeviceAddr++) {
					usbDevice device;
					if (device.load(hostDev, uiDeviceAddr) != -1) {
						if (uiVerbose == 0) {
							wcout << L"   Device " << uiDeviceAddr << L": " << deviceSummary(&device) << L"\n";
						} else {
							wcout << L"   Device " << uiDeviceAddr << L":\n" << deviceDetails(&device, L"   ", uiVerbose) << L"";
						}
					} else {
						break;
					}
				}
			}
			usbCloseHost(hostDev);
		} else {
			ERR("Unable to open host controller " << strHost);
		}
	}	//if (bScan) { 

	exit(rv);	
}	//int main(int argc, const char** argv) {
