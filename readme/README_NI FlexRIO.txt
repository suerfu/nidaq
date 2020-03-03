FlexRIO for Linux 64-bit, Version 18.1
July 2018

--------------------------------------------------------------------------------
This file contains late breaking news about FlexRIO 18.1 and supersedes 
any information found in other documentation included in the distribution. Refer
to ni.com/linux for the most recent information about Linux support at National
Instruments.
--------------------------------------------------------------------------------

System Requirements
Supported Hardware
Installation
Using the FlexRIO Driver and FPGA Interface C API
Linux Limitations
Known Issues
Legal Information

-------------------
System Requirements
-------------------
FlexRIO for Linux software for the Linux 64-bit architecture has been
tested on the following distributions:

    openSUSE Leap 42.2
    openSUSE Leap 42.3
    Red Hat Enterprise Linux Desktop + Workstation 6.x
    Red Hat Enterprise Linux Desktop + Workstation 7.x
    CentOS 7.x

FlexRIO for Linux requires a 64-bit distribution and does not support
32-bit applications.

For more information about supported Linux versions and distributions supported
by NI, refer to ni.com/linux.	
	
FlexRIO has the following requirements:

Processor—1 GHz or faster 64-bit (x64) processor
RAM—2 GB RAM

In order to use FlexRIO on Linux, you need a Windows system to develop your 
LabVIEW FPGA application. You must compile a bitfile, generate headers with the 
FPGA Interface C API Generator, and then move the application to your Linux 
system. Refer to the "Using the FlexRIO Driver and FPGA Interface C API" 
section for more information about developing LabVIEW FPGA applications with 
Windows systems.

------------------
Supported Hardware
------------------
FlexRIO 18.1 for Linux supports the following hardware models:

-FlexRIO FPGA Modules
*PXI-7951R
*PXI-7952R
*PXI-7953R
*PXI-7954R
*PXIe-7961R
*PXIe-7962R
*PXIe-7965R
*PXIe-7966R
*PXIe-7971R
*PXIe-7972R
*PXIe-7975R
*PXIe-7976R

-FlexRIO Adapter Modules
*NI 5731
*NI 5732
*NI 5733
*NI 5734
*NI 5741
*NI 5742
*NI 5751/5751B
*NI 5752/5752B
*NI 5753
*NI 5761
*NI 5762
*NI 5771
*NI 5772
*NI 5781
*NI 5782
*NI 5783
*NI 5791
*NI 5792
*NI 5793
*NI 6581/6581B
*NI 6583
*NI 6584
*NI 6585/6585B
*NI 6587
*NI 6589

------------
Installation
------------
National Instruments provides a repository add-on for all supported drivers
that you can install using your Linux distribution's native package manager.

1. Visit ni.com/info and enter the Info Code LinuxRepository.
2. Download the installer for the repository add-on for your Linux distribution.
3. Install the repository add-on using the rpm command.
4. Select and install the following FlexRIO driver packages through your
Linux distribution's package manager:

  For support for all FlexRIO devices:
  ni-flexrio
  
  For support for PXI-795x devices:
  ni-flexrio-795x
  
  For support for PXI-796x devices:
  ni-flexrio-796x
  
  For support for PXI-797x devices:
  ni-flexrio-797x

-------------------------------------------------
Using the FlexRIO Driver and FPGA Interface C API
-------------------------------------------------

You must begin development on a computer running Windows.
1. Use LabVIEW FPGA to develop and build an FPGA VI that runs on a FlexRIO 
   device.
2. Use the FPGA Interface C API Generator to create the C source files for 
   your application.
   Go to ni.com/info and enter the Info Code fpgainterfacecapi to get the
   latest version of the FPGA Interface C API Generator.
3. Copy the generated files to the Linux machine to begin development of
   C/C++ code for your application.
   For information about using the FPGA Interface C API, go to ni.com/info and
   enter the Info Code fpgainterfacecapihelp.

To find the FlexRIO device you are using in your application, use the command 
"lsni -v" to list information on all National Instruments devices in the system.

-----------------
Linux Limitations
-----------------
Some features discussed in the FlexRIO 18.1 documentation are
unavailable on Linux.

The following FlexRIO driver features supported for Windows are not supported 
for Linux.

  *Development of LabVIEW FPGA VIs and bitfiles (*.lvbitx)
  *Development of LabVIEW FPGA Interface host VIs

You must compile a bitfile using LabVIEW FPGA on a Windows machine. Refer to 
the "Using the FlexRIO Driver and FPGA Interface C API" section for more 
information.

------------
Known Issues
------------

You can access the software and documentation known issues list online.

Visit ni.com/info and enter the Info Code extai9 for an up-to-date list
of known issues in FlexRIO 18.1.

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

----
Patents
----
For patents covering the NI products/technology, refer to the
appropriate location: Help»Patents in your software, the patents.txt file on
your media, or the NI Patent Notice at ni.com/patents.
