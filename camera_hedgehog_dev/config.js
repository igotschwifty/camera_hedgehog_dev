var configure = require('/opt/euresys/egrabber-linux-x86_64-14.0.1.167/scripts/configurator.js');

var grabber = grabbers[0];

var RemotePort = grabber.RemotePort;
var StreamPort = grabber.StreamPort;
var DevicePort = grabber.DevicePort;
StreamPort.execute("StreamReset");
DevicePort.execute("DeviceReset");

RemotePort.set("Width", 128);
RemotePort.set("Height", 128);
RemotePort.set("CxpLinkConfiguration", "CXP12_X4");
RemotePort.set("AcquisitionFrameRate", 10000);