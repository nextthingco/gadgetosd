# Gadget

## Installation

Gadget is available for Linux, Mac OS and Windows operating systems.

### Linux

Currently, only Ubuntu 16.04 is officially supported.
Other systems are untested but might work as well.

Download the latest version here:

[Gadget unstable .deb package](https://opensource.nextthing.co/githost/gadgetosd/unstable/latest/gadget.deb)

[Gadget testing .deb package](https://opensource.nextthing.co/githost/gadgetosd/testing/gadget.deb)

### Mac OS

Gadget has been tested on Mac OS 10.12.
Older versions might work, but are not officially supported.

Download the latest version here:

[Gadget unstable .dmg](https://opensource.nextthing.co/githost/gadgetosd/latest/Gadget.dmg)

[Gadget testing .dmg](https://opensource.nextthing.co/githost/gadgetosd/testing/Gadget.dmg)

###  Windows

Gadget is supported on Windows 7 and Windows 10.
Other versions are untested.

Download the latest version here:

[Gadget unstable installer](https://opensource.nextthing.co/githost/gadgetosd/latest/Gadget-Setup.dmg)

[Gadget testing installer](https://opensource.nextthing.co/githost/gadgetosd/testing/Gadget-Setup.exe)

## Getting Started

### Flash your Device

1. Make sure the device is powered off and not connected via USB.
2. Press and hold the FEL button.
3. Connect the device to your computer via USB.
4. Release the FEL button.
5. Go to http://flash.getchip.com and flash the Gadget OS image.
6. Disconnect your device from USB.

### Build and Deploy your first Project

Click on the Gadget icon to open up the Gadget terminal.
To create a project called `hello_world` type:
```
gadget init hello_world
```

Change into the project directory:
```
cd hello_world
```

Build the project:
```
gadget build
```

To deploy your device, connect your device to your computer via USB and type:
```
gadet deploy
```

Congratulation you just build and deployed your first Embedded Linux project onto real hardware.

## Getting Help

Type
```
gadget --help
```

and you should see
```
Create embedded Linux apps - easy.

usage: gadget <command> [<args>]

common gadget commands:
  init        Create an empty gadget project
  build       Build the project in the current directory
  deploy      Deploys the project in the current directory
  start       Starts the application on a device
  stop        Stops the application running on a device
  delete      Deletes the application instance from a device
  purge       Purges the application from a device
  status      Shows status information

optional arguments:
  -h, --help  show this help message and exit

get help for specific command:
  gadget <command> --help
```
