NI-Sync for Linux Desktop 64-bit, Version 18.1
May 2018

--------------------------------------------------------------------------------
This file contains late breaking news about NI-Sync and supersedes any
information found in other documentation included in the distribution. Refer to
ni.com/linux for the most recent information about Linux support at National
Instruments.
--------------------------------------------------------------------------------

System Requirements
Supported Hardware
Installation
Examples
Documentation and Help
Linux Limitations
Known Issues
NI-Sync Does Not Support 32-bit Linux Distributions
FAQs
Legal Information

-------------------
System Requirements
-------------------
NI-Sync for Linux software for the Linux 64-bit architecture has been
tested on the following distributions:

    openSUSE Leap 42.2
    openSUSE Leap 42.3
    Red Hat Enterprise Linux Desktop + Workstation 7.x
    CentOS 7.x

For more information about supported Linux versions and distributions supported
by NI, refer to ni.com/linux.

------------------
Supported Hardware
------------------
NI-Sync for Linux supports the following timing and synchronization hardware:
	
	PXIe-6674T

------------
Installation
------------
National Instruments provides a repository add-on for all supported drivers
that you can install using your Linux distribution's native package manager.

1. Visit ni.com/info and enter the Info Code LinuxRepository.
2. Download the installer for the repository add-on for your Linux distribution.
3. Install the repository add-on using the rpm command.
4. Select and install the following NI-Sync packages through your
Linux distribution's package manager:
   - ni-sync
   - (C support) libnisync1-devel
   - (LabVIEW support) ni-sync-labview-[year]-support

--------
Examples
--------
This release includes examples showing the use of NI-Sync 18.1 in LabVIEW.

The LabVIEW examples are located in the LabVIEW examples directory:
	Help >> Find Examples...

NI-Sync 18.1 for Linux Desktop supports only the following examples:
	* Route Ground to Trigger
	* Route Trigger
	* Route to All PXI_STAR
	* Route Clock
	* Share Trigger Between Chassis
	* Share Clock Between Chassis
	* Share PXI_CLK10 & Trigger Between Chassis
	
	
THe C example is located in the following directory:
	/usr/share/doc/libnisync1/examples/ansi_c

-------------
Documentation
-------------
Access the NI-Sync 18.1 help documentation from LabVIEW:
	Help >> NI-Sync Reference Help
	
Access the NI-Sync 18.1 help documentation for C:
	/usr/share/doc/libnisync1/c-api-help.html

Use a web browser to view the NI-Sync documentation at ni.com.

-----------------
Linux Limitations
-----------------
Some features discussed in the NI-Sync 18.1 documentation are
unavailable on Linux.

- NI-Sync 18.1 for Linux Desktop supports two application development environments:
  * LabVIEW 2016 64-bit, LabVIEW 2017 64-bit, and LabVIEW 2018 64-bit or later
  * C, C++, ANSI C

- NI-Sync 18.1 for Linux Desktop does not support other ADEs, such as the LabVIEW Real-Time
  Module, LabWindows/CVI, Microsoft Visual C/C++, or Microsoft Visual Basic.

- While NI-Sync 18.1 includes LabVIEW VIs for LabVIEW 2018, it does
  not provide other graphical utilities that are available for other operating
  systems:
  * LabVIEW Express VIs
  * The Measurement and Automation Explorer (MAX) graphical configuration
    utility.
  * Soft front panels.

- NI-Sync 18.1 for Linux Desktop supports only limited functionality for the PXIe-6674T
  timing and synchronization module. The following features are not supported:
  * Synchronous routing
  * Clock generation
  * Measure Frequency VI
  * Calibration VIs
 
- NI-Sync 18.1 supports only the following properties:
  * PFI Threshold
  * Serial Number
  
- NI-Sync 18.1 for Linux Desktop supports limited trigger routing. Visit ni.com/info
  and enter the Info Code NISyncLinuxRouting for available trigger routes.
  
- Measurement & Automation Explorer (MAX) is not supported on Linux Desktop. To read
  device aliases and device properties, use the lsni command.
  
------------
Known Issues
------------
You can access the software and documentation known issues list online. 
For a list of known issues with NI-Sync 18.1, visit ni.com/info
and enter the Info Code NISync181KnownIssues.

----------------------------------------------------------------
NI-Sync 18.1 Does Not Support 32-bit Linux Distributions
----------------------------------------------------------------
NI-Sync 18.1 for Linux Desktop does not support Intel 32-bit versions
of Linux distributions.

-----------------
Legal Information
-----------------

----
COPYRIGHT:
----
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

----
End-User License Agreements and Third-Party Legal Notices
----
Notices are located in the /usr/share/doc/<package-name>/NOTICES directory
EULAs are located in the /usr/share/ni/licenses directory

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

Members of the NI Alliance Partner Program are business
entities independent from NI and have no agency, partnership,
or joint-venture relationship with NI.

----
Patents
----
For patents covering the NI products/technology, refer to the
appropriate location: Help»Patents in your software, the patents.txt file on
your media, or the NI Patent Notice at ni.com/patents.

377612A-01