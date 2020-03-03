NI-DMM for Linux 64-bit, Version 18.2
July 2018

--------------------------------------------------------------------------------
This file contains late breaking news about NI-DMM 18.2 and supersedes any
information found in other documentation included in the distribution. Refer to
ni.com/linux for the most recent information about Linux support at National
Instruments.
--------------------------------------------------------------------------------

System Requirements
Supported Hardware
Installation
Examples
Linux Limitations
Known Issues
Legal Information

-------------------
System Requirements
-------------------
NI-DMM for Linux software for the Linux 64-bit architecture has been
tested on the following distributions:

    openSUSE Leap 42.2
    openSUSE Leap 42.3
    Red Hat Enterprise Linux Desktop + Workstation 6.x
    Red Hat Enterprise Linux Desktop + Workstation 7.x
    CentOS 7.x

NI-DMM for Linux requires a 64-bit distribution and does not support
32-bit applications.

For more information about supported Linux versions and distributions supported
by NI, refer to ni.com/linux.

Minimum System Requirements:

Processor—2.53 GHz Intel Core 2 Duo processor or equivalent
RAM—1 GB, 800 MHz DDR2 RAM
A screen resolution of 1,024 x 768

Recommended System: 

Processor—2.3 GHz quad-core Intel Core i7 or equivalent
RAM—4 GB, dual-channel 1600 MHz DDR3 RAM

Refer to the LabVIEW Readme for additional system requirements and
supported operating systems for LabVIEW.

-------------------
Supported Hardware
-------------------
NI-DMM 18.2 for Linux supports the following digital multimeters:

-PXI Digital Multimeter
*PXI-4065
*PXI-4070
*PXI-4071
*PXI-4072
*PXIe-4080
*PXIe-4081
*PXIe-4082

-Digital Multimeter Device
*PCI-4065
*PCI-4070
*PCIe-4065

-------------------
Installation
-------------------
National Instruments provides a repository add-on for all supported drivers
that you can install using your Linux distribution's native package manager.

1. Visit ni.com/info and enter the Info Code LinuxRepository.
2. Download the installer for the repository add-on for your Linux distribution.
3. Install the repository add-on using the rpm command.
4. Select and install the following NI-DMM driver packages through your Linux 
distribution's package manager:
   ni-dmm
   
   For DAQmx-based DMM support:
   ni-dmm-daqmx
   
   For LabVIEW development support and examples:
   ni-dmm-labview-20xy-support

-------------------
Examples
-------------------
This release includes examples showing the use of NI-DMM in C and LabVIEW.

C examples are located in /usr/share/nidmm-devel/examples. Make files
are provided that link to the necessary libraries.

The LabVIEW examples are located in the LabVIEW examples directory.

-------------------
Linux Limitations
-------------------
Some features discussed in the NI-DMM documentation are
unavailable on Linux.

- NI-DMM supports two application development environments:
  * LabVIEW 2015, 2016, 2017, 2018
  * C and C++ (gcc)

  NI-DMM does not support other ADEs, such as the LabVIEW Real-Time
  Module, LabWindows/CVI, Microsoft Visual C/C++, or Microsoft Visual Basic.

- While NI-DMM includes LabVIEW VIs, it does
  not provide other graphical utilities that are available for other operating
  systems:
  * LabVIEW Express VIs.
  * The Measurement and Automation Explorer (MAX) graphical configuration
    utility. 
  * Soft front panels.

- External calibration functions are not supported. Refer to your hardware
  documentation for information about external calibration, and refer to
  ni.com/calibration. If your NI-DMM requires external
  calibration, either perform the calibration on a Windows system, or hire
  NI or an independent calibration laboratory to perform the
  calibration.

-------------------
Known Issues
-------------------
You can access the software and documentation known issues list online. 

Visit ni.com/info and enter the Info Code dmm182ki for an up-to-date list 
of known issues in NI-DMM 18.2.

-------------------
Legal Information
-------------------

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
Notices are located in the /usr/share/doc/<package-name>/NOTICES directory.
EULAs are located in the /usr/share/in/licenses directory.

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
