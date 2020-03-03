NI-VISA(TM) Software for Linux/x86 64-bit, Version 18.2
June 2018

--------------------------------------------------------------------------------
This file contains late breaking news about NI-VISA 18.2 and supersedes any 
information found in other documentation included in the distribution. Refer to
ni.com/linux for the most recent information about Linux support at National
Instruments.
--------------------------------------------------------------------------------

---------------------
Table of Contents
---------------------
System Requirements
Installation
Documentation and Help
Linux Limitations
Known Issues
Legal Information

-------------------
System Requirements
-------------------
NI-VISA 18.2 for Linux software for the Linux/x86 64-bit architecture has been 
tested on the following distributions:

openSUSE LEAP 42.2
openSUSE LEAP 42.3
Red Hat Enterprise Linux Desktop + Workstation 6.x
Red Hat Enterprise Linux Desktop + Workstation 7.x
CentOS 7.x

NI-VISA 18.2 for Linux requires a 64-bit distribution and does not support
32-bit applications.

For more information about supported Linux versions and distributions supported
by NI, refer to ni.com/linux.

You must use the provided INSTALL script to install NI-VISA. Manual
installation using rpm is not supported.

At the time of release, NI supports NI-VISA 18.2 on only the above listed 
distributions and versions. For information about NI-VISA compatibility 
on other versions of these distributions, including earlier versions 
of the distributions listed above, refer to KB 5CNEG20S on ni.com, 
"What Linux distributions do NI's drivers and software support?".

After installation, you may need to reboot your machine before attempting
to use NI-VISA. The installer will prompt you to reboot if it is required.

------------------------------
Installation
------------------------------

NI-VISA 18.2 supports the 64-bit LabVIEW application development environment:
   - LabVIEW 2015, LabVIEW 2016, LabVIEW 2017, LabVIEW 2018 

National Instruments provides a repository add-on for all supported drivers
that you can install using your Linux distribution's native package manager.

1. Visit ni.com/info and enter the Info Code LinuxRepository.
2. Download the installer for the repository add-on for your Linux distribution.
3. Install the repository add-on using the rpm command.
4. Select and install the following NI-VISA 18.2 driver package through your
Linux distribution's package manager:
   - ni-visa
5. For LabVIEW support, select and install the driver package that corresponds
to your version of LabVIEW:
   - ni-visa-labview-2015-support
   - ni-visa-labview-2016-support
   - ni-visa-labview-2017-support
   - ni-visa-labview-2018-support

----------------------
Documentation and Help
----------------------
Use a web browser to view the NI-VISA 18.2 help at ni.com/manuals.

-------------------
Linux Limitations
-------------------

NI-VISA 18.2 for Linux does not support 32-bit versions of LabVIEW.

The VISA user must have write access to the file that represents
the USB device, which is typically somewhere in a subdirectory within
"/dev/bus/usb". If this is not the case, the USB device is not
accessible by VISA (it will not be found using viFindRsrc and viOpen will
fail). The default configuration on most systems is that the 'root' user
has write access; however, no other user has this access.

There are a number of options that you can take to provide a non-root
user access to a USB device.

1) Use udev rules. By default, NI-VISA installs
scripts to give all users write access to all USB TMC devices and a
framework for USB RAW devices. To add write permissions for a specific
USB RAW device, run the included script:
"/usr/lib/x86_64-linux-gnu/ni-visa/usb/AddUsbRawPermissions.sh".
For more information about udev, please visit:
https://www.kernel.org/pub/linux/utils/kernel/hotplug/udev/udev.html


2) The 'root' user may add write permissions to the file that represents
the USB device which is typically somewhere in a subdirectory within
"/dev/bus/usb". Unfortunately, these permissions will be lost if the
device is unplugged and then plugged back in. Because of this, this
approach is not recommended.

--------------------------
Known Issues and Bug Fixes
--------------------------

For a list of Known Issues of NI-VISA 18.2, visit ni.com/info and
enter the Info Code NIVISA180KnownIssues.

For a list of bug fixes for NI-VISA, visit ni.com/info and enter
the Info Code NIVISAFixList.

-----------------------------
New Features and Improvements
-----------------------------

------------
NI-VISA 18.2
------------
Incremental updates and bug fixes.

------------
NI-VISA 17.5
------------
Incremental updates and bug fixes.

------------
NI-VISA 17.0
------------
Added support for NI System Configuration API.
Added NI I/O Trace support for 64-bit VISA applications.

------------
NI-VISA 15.0
------------
Added support for developing 64-bit VISA applications.

------------
NI-VISA 14.0
------------
Added support for viGpibControlREN on TCPIP resources that use the 
VXI-11 protocol.

-----------
NI-VISA 5.3
-----------
Improved performance of viRead for a HiSLIP resource.

-----------
NI-VISA 5.2
-----------
Improved performance of viWrite for a HiSLIP resource.

-----------
NI-VISA 5.1
-----------
Added support for HiSLIP.

Updated the VISA Interactive Control stand-alone application. Refer to the
NI-VISA Help for more information on the updates to VISAIC.

-----------
NI-VISA 5.0
-----------
Added support for openSUSE 11.1, openSUSE 11.2, and Scientific Linux 5.x

Added support for 64-bit formatted I/O.

Dropped support for GPIB-VXI.


-----------------
Legal Information
-----------------
----------
COPYRIGHT:
----------
(c) 2018 National Instruments.
All rights reserved.

Under the copyright laws, this publication may not be reproduced or transmitted
in any form, electronic or mechanical, including photocopying, recording,
storing in an information retrieval system, or translating, in whole or in
part, without the prior written consent of National Instruments Corporation.

NI respects the intellectual property of others, and we ask
our users to do the same. NI software is protected by copyright and other
intellectual property laws. Where NI software may be used to reproduce
software or other materials belonging to others, you may use NI software
only to reproduce materials that you may reproduce in accordance with the
terms of any applicable license or other legal restriction.

Third-Party Legal Notices
You can find NI-VISA third-party legal notices in the 
/usr/local/vxipnp/share/nivisa directory. 

U.S. Government Restricted Rights
If you are an agency, department, or other entity of the United States
Government ("Government"), the use, duplication, reproduction, release,
modification, disclosure or transfer of the technical data included in this
manual is governed by the Restricted Rights provisions under Federal
Acquisition Regulation 52.227-14 for civilian agencies and Defense Federal
Acquisition Regulation Supplement Section 252.227-7014 and 252.227-7015 for
military agencies.

IVI Foundation Copyright Notice
Content from the IVI specifications reproduced with permission from the IVI
Foundation.

The IVI Foundation and its member companies make no warranty of any kind with
regard to this material, including, but not limited to, the implied warranties
of merchantability and fitness for a particular purpose. The IVI Foundation and
its member companies shall not be liable for errors contained herein or for
incidental or consequential damages in connection with the furnishing,
performance, or use of this material.

-----------
TRADEMARKS:
-----------
Refer to the NI Trademarks and Logo Guidelines at ni.com/trademarks for
information on National Instruments trademarks. Other product and company names
mentioned herein are trademarks or trade names of their respective companies.

--------
PATENTS:
--------
For patents covering the National Instruments products/technology, refer to the
appropriate location: Help»Patents in your software, the patents.txt file on
your media, or the National Instruments Patent Notice at ni.com/patents.

