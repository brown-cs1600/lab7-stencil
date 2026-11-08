# [Arduino BLE functions](https://docs.arduino.cc/libraries/arduinoble/) and their impact on network congestion

BLE uses three channels (carrier frequencies) for advertising/device discovery, and 37 channels for data (communicating between devices once connected). Reducing congestion on the advertising channels is higher priority, so this is what "congestion" refers to unless stated otherwise.

## Functions used by central devices
- **BLE.scanFor..**: the Arduino BLE scan functions use an *active* scan[^1], which means they send responses to advertisements on the network and therefore ***contribute to congestion*** (note that the peripheral devices then respond to the scan requests with the necessary information). The *post*-processing filters out peripherals that do not match the scan criteria or that have already been found, and stores matching peripherals in an internal buffer Note that **BLE.stopScan** stops the scanning/scan requests being sent on the network.
- **BLE.available**: checks the internal buffer that the scan functions populate, and therefore does not contribute to network congestion. Will return an empty device if the buffer is empty (no matching peripheral was found).
- **peripheral.connect** and **peripheral.discoverAttributes**: send requests to the peripheral device on the network and therefore ***contribute somewhat to congestion*** if done repeatedly. Attribute discovery may involve multiple exchanges, but is done on the data channels.
- **peripheral.characteristic** does not contribute to congestion because it uses internal data that discoverAttributes writes
- **characteristic.writeValue** and **characteristic.read** make write/read requests and therefore ***contribute to network congestion*** on the data channels if done repeatedly. Note that using the `BLEWrite` property for a write characteristic (rather than `BLEWriteWithoutResponse` means every write is accompanied with an ACK but allows us to check that the write was accepted so that we can try again if it wasn't.

## Functions used by peripheral devices
- All functions leading up to advertising (**BLE.setLocalName**, **BLE.addService**, **BLE.setAdvertisedService**, functions to set up characteristics): modify internal data structures and do not contribute to network congestion
- **BLE.advertise**: advertises this device on the network and therefore ***contributes to congestion***. **BLE.setAdvertisingInterval** changes how often the advertising packet is sent on the network.
- **BLE.central**, **central.connected**, **central.disconnect**: the first two just check internal data structures and do not contribute to network congestion. Same idea with the functions that check aspects of the device when connected (such as **.address**, **.localName**). ***Note:*** keeping a connection open sends periodic messages between devices, and disconnecting sends a request on the network data channels, but this is negligible because it's not continuous.
- **characteristic.written**: checks internal data structures. Does not contribute to network congestion.

[^1]: A [PR exists](https://github.com/arduino-libraries/ArduinoBLE/pull/283) to add passive scanning, but the problem is that it won't work with all of the scan functions out of the box, since now the scan is fully dependent on what information each peripheral puts into its advertisement packet.