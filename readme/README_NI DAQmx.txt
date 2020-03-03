NI-DAQmx for Linux/x86 64-bit, Version 18.1
May 2018

--------------------------------------------------------------------------------
This file contains late breaking news about NI-DAQmx and supersedes any
information found in other documentation included in the distribution. Refer to
ni.com/linux for the most recent information about Linux support at National
Instruments.
--------------------------------------------------------------------------------

System Requirements
Installation
Examples
Documentation and Help
Linux Limitations
Known Issues
NI-DAQmx Does Not Support 32-bit Linux Distributions
Legal Information

-------------------
System Requirements
-------------------
NI-DAQmx for Linux software for the Linux/x86 64-bit architecture has been
tested on the following distributions:

    openSUSE Leap 42.2
    openSUSE Leap 42.3
    Red Hat Enterprise Linux Desktop + Workstation 6.x
    Red Hat Enterprise Linux Desktop + Workstation 7.x
    CentOS 7.x

For more information about supported Linux versions and distributions supported
by NI, refer to ni.com/linux.

------------
Installation
------------
National Instruments provides a repository add-on for all supported drivers
that you can install using your Linux distribution's native package manager.

NOTE: Upgrading NI-DAQmx from version 8.0 is not supported.

1. Visit ni.com/info and enter the Info Code LinuxRepository.
2. Download the installer for the repository add-on for your Linux distribution.
3. Install the repository add-on using the rpm command.
4. Select and install the following NI-DAQmx driver packages through your
Linux distribution's package manager:
   - ni-daqmx
   - ni-daqmx-labview-<ver>-support

--------
Examples
--------
For examples, refer to ni.com/info. Enter theinfo code "DAQ181LinuxExamples" 
and click "Submit."

-------------
Documentation
-------------
Use a web browser to view getting started information at:
http://www.ni.com/product-documentation/5434/en/

NI-DAQmx Help:
http://digital.ni.com/express.nsf/bycode/exknn4

NI-DAQmx VI Reference:
http://digital.ni.com/express.nsf/bycode/exiv9b

NI-DAQmx C Function Reference:
http://digital.ni.com/express.nsf/bycode/exxy7f

-----------------
Linux Limitations
-----------------
Some features discussed in the NI-DAQmx documentation are
unavailable on Linux.

- NI-DAQmx supports two application development environments:
  * LabVIEW 2016 or later
  * C and C++ (gcc)

  NI-DAQmx does not support other ADEs, such as the LabVIEW Real-Time
  Module, LabWindows/CVI, Microsoft Visual C/C++, or Microsoft Visual Basic.

- While NI-DAQmx includes LabVIEW VIs for LabVIEW 2016 and later, it does
  not provide other graphical utilities that are available for other operating
  systems:
  * LabVIEW Express VIs
  * The Measurement and Automation Explorer (MAX) graphical configuration
    utility.
  * Soft front panels.

- External calibration functions are not supported.  Refer to your hardware
  documentation for information about external calibration, and refer to
  ni.com/calibration.  If your product requires external
  calibration, either perform the calibration on a Windows system, or hire
  NI or an independent calibration laboratory to perform the
  calibration.
  
For more information on Linux limitations, refer to ni.com/info. Enter the
info code "DAQLinuxExperience" and click "Submit."

For information on Linux tools you can use to work around these limitations,
enter the info code "DAQLinuxTools" and click "Submit."

------------
Known Issues
------------
1. If your system supports SMAP (Supervisor Mode Access Prevention) and it is 
   enabled, you will encounter an error during installation, as NI-DAQmx does 
   not support SMAP. To disable SMAP, add "nosmap" to the kernel parameters 
   as part of your boot configuration. Consult your boot manager on how to 
   update the kernel parameters. For more information, refer to ni.com/info. 
   Enter the info code "DAQ_SMAP_Error" and click "Submit."

----------------------------------------------------
NI-DAQmx Does Not Support 32-bit Linux Distributions
----------------------------------------------------
NI-DAQmx dropped support for Intel 32-bit versions of
Linux distributions on July 1, 2016. Versions of NI-DAQmx that ship after
July 1, 2016 may install and execute on Intel 32-bit distributions, but
official support is not provided.

-----------------
Supported Devices
-----------------
The following lists detail the devices supported in NI-DAQmx.

In the device support tables that follow, "X" indicates that the device is 
supported, and "-" indicates that the device either does not exist or is not 
supported in NI-DAQmx.

----
X Series DAQ
----

Device		PCIe	PXIe
----------------------------
NI 6320		X	-
NI 6321		X	-
NI 6323		X	-
NI 6341		X	X
NI 6343		X	-
NI 6345		-	X
NI 6346		X	-
NI 6349		-	X
NI 6351		X	-
NI 6353		X	-
NI 6355		-	X
NI 6356		-	X
NI 6358		-	X
NI 6361		X	X
NI 6363		X	X
NI 6365		-	X
NI 6366		-	X
NI 6368		-	X
NI 6374		X	-
NI 6375		-	X
NI 6376		X	X
NI 6378		-	X

----
M Series DAQ
----

Device	PCI	PCIe	PXI	PXIe
------------------------------------
NI 6220	X	-	X	-
NI 6221	X	-	X	-
NI 6224	X	-	X	-
NI 6225	X	-	X	-
NI 6229	X	-	X	-
NI 6230	X	-	X	-
NI 6232	X	-	X	-
NI 6233	X	-	X	-
NI 6236	X	-	X	-
NI 6238	X	-	X	-
NI 6239	X	-	X	-
NI 6250	X	-	X	-
NI 6251	X	X	X	X
NI 6254	X	-	X	-
NI 6255	X	-	X	-
NI 6259	X	X	X	X
NI 6280	X	-	X	-
NI 6281	X	-	X	-
NI 6284	X	-	X	-
NI 6289	X	-	X	-

----
S Series DAQ
----

Device	PCI	PXI	PXIe
----------------------------
NI 6110	X	-	-
NI 6111	X	-	-
NI 6115	X	X	-
NI 6120	X	X	-
NI 6122	X	X	-
NI 6123	X	X	-
NI 6124	-	-	X
NI 6132	X	X	-
NI 6133	X	X	-
NI 6143	X	X	-
NI 6154	X	-	-

----
SC Express
----

Device	PXIe
------------
NI 4300	X
NI 4302	X
NI 4303	X
NI 4304	X
NI 4305	X
NI 4309	X
NI 4310	X
NI 4322	X
NI 4330	X
NI 4331	X
NI 4339	X
NI 4340	X
NI 4353	X
NI 4357	X

----
SC Express Accessories
----

Device		Supported Accessory
-------------------------------
NI 4300		NI TB-4300, NI TB-4300B, NI TB-4300C, NI CAL-4300B
NI 4302		NI RM-4302, NI TB-4302, NI TB-4302C
NI 4303		NI RM-4302, NI TB-4302, NI TB-4302C
NI 4304		NI RM-4304, NI TB-4304
NI 4305		NI RM-4304, NI TB-4304
NI 4309		NI CAL-4309, NI TB-4309 (ST), NI TB-4309 (MT)
NI 4310		NI TB-4310 (10V), NI TB-4310 (600V)
NI 4322		NI TB-4322
NI 4330		NI TB-4330, RM-24999
NI 4331		NI TB-4330, RM-24999
NI 4339		NI TB-4339, NI TB-4339B, NI TB-4339C, NI RM-4339
NI 4340		NI TB-4340
NI 4353		NI TB-4353, NI TC-4353, NI CAL-4353
NI 4357		NI TB-4357

----
AO Series
----

Device	PCI	PCIe	PXI	PXIe
------------------------------------
NI 6703	X	-	-	-
NI 6704	X	-	X	-
NI 6711	X	-	X	-
NI 6713	X	-	X	-
NI 6722	X	-	X	-
NI 6723	X	-	X	-
NI 6731	X	-	-	-
NI 6733	X	-	X	-
NI 6738	-	X	-	X
NI 6739	-	-	-	X

----
Digital I/O
----

Device		PCI	PCIe	PXI
-----------------------------------
NI 6503		X	-	-	
NI 6508		-	-	X	
NI 6509		X	X	X	
NI 6510		X	-	-	
NI 6511		X	-	X	
NI 6512		X	-	X	
NI 6513		X	-	X	
NI 6514		X	-	X	
NI 6515		X	-	X	
NI 6516		X	-	-	
NI 6517		X	-	-	
NI 6518		X	-	-	
NI 6519		X	-	-	
NI 6520		X	-	-	
NI 6521		X	-	X	
NI 6527		X	-	X	
NI 6528		X	-	X	
NI 6529		-	-	X	
NI DIO-96	X	-	-	

----
TIO Series
----

Device	PCI	PCIe	PXI	PXIe
------------------------------------
NI 6601	X	-	-	-
NI 6602	X	-	X	-
NI 6608	-	-	X	-
NI 6612	-	X	-	X
NI 6614	-	-	-	X
NI 6624	X	-	X	-

----
Dynamic Signal Acquisition (DSA)
----

Device		PCI	PXI	PXIe
------------------------------------
NI 4461		X	X	-
NI 4462		X	-	-
NI 4463		-	-	X
NI 4464		-	-	X
NI 4472/B	X	-	-
NI 4474		X	-	-
NI 4480		-	-	X
NI 4481		-	-	X
NI 4492		-	-	X
NI 4495		-	X	-
NI 4496		-	X	X
NI 4497		-	-	X
NI 4498		-	X	X
NI 4499		-	-	X
NI 4610		-	-	X

-----------------
Legal Information
-----------------

----
COPYRIGHT:
----
(c) 2003-2018 National Instruments.
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
Notices are located in the /usr/share/doc/ni-daqmx/NOTICES directory
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

----
Patents
----
For patents covering the NI products/technology, refer to the
appropriate location: Help»Patents in your software, the patents.txt file on
your media, or the NI Patent Notice at ni.com/patents.
