NI-488.2 for Linux/x86 64-Bit, Version 18.0
June 2018

This file contains important information about the NI-488.2 software
for Linux, including installation issues, compatibility issues, API
changes from previous drivers, and a list of FAQs. Refer to the
Installation Guide in the Documentation directory on your NI-488.2 for
Linux media for information about installing the software.

-----------------------------------------------------------------------------
This file contains late breaking news about NI-488.2 for Linux and
supersedes any information found in other documentation included in the
distribution. Refer to ni.com/linux for the most recent information about
Linux support at National Instruments.
-----------------------------------------------------------------------------

System Requirements
Supported GPIB Interfaces
Installation
Description of Utilities
C/C++ Language Interface
Compatibility with LabVIEW
Compatibility with Previous NI-488.2 Drivers
Uninstallation
FAQs
Documentation and Help
Bug Fixes and Enhancements
Linux Limitations
Legal Information


-------------------
System Requirements
-------------------
This release of NI-488.2 for Linux software for the Linux/x86 64-bit
architecture has been tested on the following distributions:

CentOS 7
openSUSE LEAP 42.2
openSUSE LEAP 42.3
Red Hat Enterprise Linux 6
Red Hat Enterprise Linux 7

NI-488.2 for Linux requires a 64-bit distribution and does not support
32-bit applications.

An X Windows graphical environment is required to run graphical configuration
and troubleshooting utilities.

For more information about supported Linux versions and distributions supported
by NI, refer to ni.com/linux.


-------------------------
Supported GPIB Interfaces
-------------------------
NI PCI-GPIB
NI PCI-GPIB/LP
NI PCIe-GPIB
NI PCI-8232
NI PXI-GPIB
NI PXI-8232
NI GPIB-ENET/1000

To configure a supported GPIB interface, refer to the Installation Guide
located in /usr/share/doc/ni-488.2-documentation.


------------
Installation
------------
National Instruments provides a repository add-on for all supported drivers
that you can install using your Linux distribution's native package manager.

1. Visit ni.com/info and enter the Info Code LinuxRepository.
2. Download the installer for the repository add-on for your Linux
distribution.
3. Install the repository add-on using the rpm command.
4. Select and install the following NI-488.2 for Linux driver packages
through your Linux distribution's package manager:
   - ni-488.2
   - libni4882-devel
   - ni-488.2-iotrace-support
   - ni-488.2-runtime
   - ni-488.2-utils
   - libni4882
   - ni-488.2-config
   - ni-488.2-dkms
   - ni-488.2-documentation
   - ni-488.2-errors
   - ni-488.2-gpibenumsvc
   - ni-488.2-gpibexplorer
   - ni-488.2-gpibintctrl
   - ni-488.2-gpibtroubleshooting
   - ni-488.2-sysapi-support
   - ni-488.2-enetfirmwareupdate


------------------------
Description of Utilities
------------------------
gpibexplorer    - GPIB Explorer is used to browse, add, and delete GPIB
                  interfaces.
gpibtsw         - GPIB Troubleshooting Wizard is used to verify your
                  hardware and software installation.
gpibintctrl     - GPIB Interactive Control is used to communicate with
                  GPIB devices interactively.

Refer to the NI-488.2 User Manual for more information about using these
utilities.


------------------------
C/C++ Language Interface
------------------------
ni4882.h          - NI-488.2 header file
libni4882.so      - Dynamic library
libni4882.a       - Object file that allows explicit loading of libni4882.so

NI4882 API Examples:

To have the ni4882 library loaded explicitly when the program makes the
first call into the library:

   gcc prog.c libni4882.a -ldl

   g++ prog.cpp libni4882.a -ldl

For more NI-488.2 API use examples, refer to
/usr/share/ni4882/examples/README.


--------------------------
Compatibility with LabVIEW
--------------------------
This driver version was tested with LabVIEW 2016, LabVIEW 2017,
and LabVIEW 2018.


--------------------------------------------
Compatibility with Previous NI-488.2 Drivers
--------------------------------------------
This installation of NI-488.2 software may have issues installing over
some previous versions of the NI-488.2 drivers.  You should manually uninstall
previous NI-488.2 drivers before installing this software.

----
FAQs
----

Q1: When I try to open Help from GPIB Explorer, I am prompted for my default
    web browser. What does this do?

A1: This problem only occurs on systems where htmlview is not installed.
    Each user's response to this dialog box is stored in "~/natinst/.lvbrowser"
    (where ~ is the user's home directory). If you need to change this
    response, you can edit this file or simply delete it so that you will
    be prompted again when entering help.


Q2: When I launch GPIB Explorer from a prompt opened using 'su', I am unable to
    launch Interactive Control from the GPIB Explorer menu.

A2: When elevating to root privileges, use 'su -' instead of 'su'. This will
    properly configure the environment for the root user, and will resolve the
    issue launching Interactive Control.


Q3: When launching 'gpibexplorer' or 'gpibtsw' using 'sudo', I see an error
    similar to "Unable to open X display."

A3: This can be resolved by granting access to the root user to utilize the
    current display, and passing the display information to the 'sudo'
    command, as shown below:

      xhost local:root
      sudo DISPLAY=$DISPLAY gpibexplorer


Q4: Why can't I discover a GPIB-ENET/1000 device in GPIB Explorer?

A4: It is likely that your firewall is blocking the discovery of the
    device. You can get around this by disabling the firewall during
    discovery. This can be reverted after you have added the device.


Q5: Launching ethernetconfig or gpibintctrl through GPIB Explorer causes the
    text utilities to launch twice in different terminals.

A5: This problem is due to a bug (#646317 bugzilla) in gnome-terminal
    (2.32.1-6.1). This causes GPIB Explorer to believe that gnome-terminal
    has failed to launch, so it re-launches the utility in another terminal.
    You can simply close the duplicated window or update gnome-terminal to
    version 3.2.1 or above.


----------------------
Documentation and Help
----------------------
You can access NI-488.2 Help from GPIB Explorer.

-----------------
Linux Limitations
-----------------

Some features discussed in the NI-488.2 documentation and help (for example,
GPIB Analyzer, GPIB Ethernet Wizard) are unavailable on Linux.


Power Management

NI-488.2 software for Linux does not support power management.
You must disable the power management settings or make sure all GPIB handles
are offline before the system enters the power management modes.


fork() System Call

NI-488.2 software for Linux does not work with the fork() system call.
Specifically, the interface and device handles opened by the parent process
are not properly inherited by the child process. Any attempt to use already
opened handles in the child process will result in a Segmentation Fault for
the child process.  A workaround is either to open a new interface/device
handle in the child process or use threads instead of processes to implement
multitasking. Linux provides a POSIX-compliant thread library called pthreads.


---------------------------
Bug Fixes and Enhancements
---------------------------
The following items are a subset of issues fixed in this and previous NI-488.2
releases. If you have a CAR ID, you can search this list to validate the issue
has been fixed.

----
NI-488.2 18.0
----

Adds support for LabVIEW 2018 and openSUSE LEAP 42.3.

----
NI-488.2 17.0
----

Adds support for LabVIEW 2016 and 2017.

----
NI-488.2 FOR LINUX 15.1
----

Updated Linux distribution support.

NI-488.2 now reports GPIB controller interfaces through the NI System
Configuration API.

The following specific issues were fixed in this release:

   121091: Calling ibonl when a board level ibwait is in progress could result
           in application crash.

   121181: A deadlock might occur when attempting to use a board handle with
           pending ibonl call on another thread.


----
NI-488.2 FOR LINUX 3.2
----
Updated Linux distribution support.

Modified the integer types used in ni4882.h and libni4882.so. All uses of
'unsigned long' have been replaced with 'unsigned int' to improve compatibility
with 64-bit compilers. This API has never been documented with prior versions
of NI-488.2 for Linux, but ni4882.h and a 32-bit version of libni4882.so were
installed on the system. Any user utilizing these files should update their
applications according to the updated function prototypes in ni4882.h. This
change does not impact ni488.h or libgpibapi.so.

Added 64-bit Application Interface. NI-488.2 for Linux, Version 3.2, adds a
64-bit application interface that allows users to create 64-bit applications
using libni4882.so. Using the 32-bit version of libni4882.so, users can use
the same NI4882 API to build corresponding 32-bit applications with no source
code changes. For more information about the new NI4882 API, refer to the
NI-488.2 Help. You can access NI-488.2 Help from GPIB Explorer. To start GPIB
Explorer from a terminal, enter the following command "gpibexplorer". In GPIB
Explorer, select Help>>NI-488.2 Help from the menu bar.

NI I/O Trace functionality is limited to use with 32-bit GPIB APIs. As of this
release, it does not function with 64-bit applications.

The command line utility ethernetconfig has been replaced with an interactive
utility accessible through gpibexplorer. Several usability improvements and bug
fixes were also made in gpibexplorer.

The following specific issues were fixed in this release:

   122997: The EOIP status bit could remain set incorrectly if an ibwrta or
           ibrda call returns an EDVR error.

   171688: An ibnotify callback could fail to be triggered by END if the notify
           condition included END but not CMPL.

   205182: Asynchronous transfers could sometimes report EABO even though the
           transfer completed successfully.

   359887: GPIB Help could not be opened from the GPIB Troubleshooting Wizard
           if it had not already been opened from GPIB Explorer.

   372832: A callback from ibnotify might not be invoked after an asynchronous
           transfer was completed if the notify mask only contained END.

   374822: Calling ibonl while ibnotify is pending could cause the process to
           crash under some circumstances.

   380705: Aborting an ibrda very quickly after starting it could allow a
           PCI-based GPIB interface to receive data when addressed as a
           listener, without a read in progress. This data was lost, and
           could not be recovered.

   404267: In some situations, the GPIB-ENET/1000 could hang while performing
           asynchronous transfers. This included fixes in both the NI-488.2
           driver and the GPIB-ENET/1000 firmware. Users of the GPIB-ENET/1000
           should update the GPIB-ENET/1000 firmware to version 1.1.0 to
           receive the full benefit of this fix.

   405414: In some cases the DCAS bit was not correctly cleared after being
           reported, causing it to be reported with subsequent calls.

   406437: GPIB Explorer and GPIB Troubleshooting Wizard could fail silently if
           needed libraries were missing from the system. They now provide more
           explicit error information if supporting libraries are missing.

   418661: In some situations, when an ibnotify callback function was triggered
           due to the completion of an asynchronous transfer, the ibcnt and
           ibcntl variables would be incorrect.

   425931: Calling ibstop with an asynchronous operation in progress would
           sometimes not report an EABO error as expected.


------------
Known Issues
------------

-----------------
Legal Information
-----------------

----
COPYRIGHT:
----
(c) 2007–2018 National Instruments.
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

For end-user license agreements (EULAs) and copyright notices, conditions, and
disclaimers, including information regarding certain third-party components
used in LabVIEW, refer to the Copyright topic of the LabVIEW Help.

----
U.S. Government Restricted Rights
----
If you are an agency, department, or other entity of the United States
Government ("Government"), the use, duplication, reproduction, release,
modification, disclosure or transfer of the technical data included in this
manual is governed by the Restricted Rights provisions under Federal
Acquisition Regulation 52.227-14 for civilian agencies and Defense Federal
Acquisition Regulation Supplement Section 252.227-7014 and 252.227-7015 for
military agencies.

----
IVI Foundation Copyright Notice
----
Content from the IVI specifications reproduced with permission from the IVI
Foundation.

The IVI Foundation and its member companies make no warranty of any kind with
regard to this material, including, but not limited to, the implied warranties
of merchantability and fitness for a particular purpose. The IVI Foundation and
its member companies shall not be liable for errors contained herein or for
incidental or consequential damages in connection with the furnishing,
performance, or use of this material.


----
Trademarks
----
Refer to the NI Trademarks and Logo Guidelines at ni.com/trademarks for
information on NI trademarks. Other product and company names
mentioned herein are trademarks or trade names of their respective companies.


----
Patents
----
For patents covering the NI products/technology, refer to the
appropriate location: Help»Patents in your software, the patents.txt file on
your media, or the NI Patent Notice at ni.com/patents.