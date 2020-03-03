NI-SWITCH for Linux 64-bit, Version 18.1
July 2018

--------------------------------------------------------------------------------
This file contains late breaking news about NI-SWITCH 18.1 and supercedes any
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
NI-SWITCH for Linux software for the Linux 64-bit architecture has been
tested on the following distributions:

    openSUSE Leap 42.2
    openSUSE Leap 42.3
    Red Hat Enterprise Linux Desktop + Workstation 6.x
    Red Hat Enterprise Linux Desktop + Workstation 7.x
    CentOS 7.x

NI-SWITCH for Linux requires a 64-bit distribution and does not support
32-bit applications.

For more information about supported Linux versions and distributions supported
by NI, refer to ni.com/linux.	
	
NI-SWITCH has the following requirements:

Processor—1 GHz or faster 64-bit processor
RAM—2 GB RAM
A screen resolution of 1,024 x 768

------------------
Supported Hardware
------------------
NI-SWITCH 18.1 for Linux supports the following hardware models:

-Matrix Modules for SwitchBlock
*SWB-2810
*SWB-2811
*SWB-2812
*SWB-2813
*SWB-2814
*SWB-2815
*SWB-2816
*SWB-2817
*SWB-2833
*SWB-2834
*SWB-2865A

-PXI Carrier Modules for SwitchBlock
*PXI-2800

-PXI Matrix Switch Modules
*PXI/PXIe-2529
*PXI-2530
*PXI-2530B
*PXI/PXIe-2531
*PXI/PXIe-2532
*PXI/PXIe-2532B
*PXI-2533
*PXI-2534
*PXI-2535
*PXI-2536
*PXIe-2737
*PXIe-2738
*PXIe-2739

-PXI Multiplexer Switch Modules
*PXI-2501
*PXI-2503
*PXIe-2524
*PXIe-2525
*PXIe-2526
*PXI/PXIe-2527
*PXI/PXIe-2575
*PXI-2576
*PXI-2584
*PXI-2585
*PXIe-2746
*PXIe-2747
*PXIe-2748

-PXI Programmable Resistor Modules
*PXI-2720
*PXI-2722
*PXIe-2725
*PXIe-2727

-PXI Relay Driver Modules
*PXI-2567

-PXI Relay Modules
*PXI-2520
*PXI-2521
*PXI-2522
*PXI-2523
*PXI-2564
*PXI-2565
*PXI-2566
*PXI-2568
*PXI/PXIe-2569
*PXI-2570
*PXI-2571
*PXI-2586

-PXI RF Matrix Switch Modules
*PXI/PXIe-2540
*PXI/PXIe-2541

-PXI RF Multiplexer Switch Modules
*PXI/PXIe-2543
*PXI/PXIe-2544
*PXI-2545
*PXI-2546
*PXI-2547
*PXI-2554
*PXI-2555
*PXI-2556
*PXI-2557
*PXI-2590
*PXI-2591
*PXI/PXIe-2593
*PXI-2594
*PXI-2595
*PXI-2596
*PXI-2597
*PXI-2796
*PXI-2797

-PXI RF Relay Modules
*PXI/PXIe-2542
*PXI-2548
*PXI-2549
*PXI-2558
*PXI-2559
*PXI-2599
*PXI-2799

-PXI Signal Insertion Switch Modules
*PXI-2510
*PXI/PXIe-2512
*PXI/PXIe-2514
*PXI/PXIe-2515

-PXI Transfer Switch Modules
*PXI-2598
*PXI-2798
*PXIe-2790

------------
Installation
------------
National Instruments provides a repository add-on for all supported drivers
that you can install using your Linux distribution's native package manager.

1. Visit ni.com/info and enter the Info Code LinuxRepository.
2. Download the installer for the repository add-on for your Linux distribution.
3. Install the repository add-on using the rpm command.
4. Select and install the following NI-SWITCH driver packages through your
Linux distribution's package manager:
   ni-switch
   
   For CA1, CA2, and CA3 switch support:
   ni-switch-daqmx

   For the switch block configuration utility:
   ni-switch-daqmx-config 

   For LabVIEW development support:
   ni-switch-labview-20xy-support

   For LabVIEW examples:
   ni-switch-labview-20xy-examples

--------
Examples
--------
This release includes examples showing the use of NI-SWITCH in C and LabVIEW.

C examples are located in /usr/share/niswitch-devel/examples. Make files
are provided that link to the necessary libraries.

The LabVIEW examples are located in the LabVIEW examples directory.

-----------------
Linux Limitations
-----------------
Some features discussed in the NI-SWITCH 18.1 documentation are
unavailable on Linux.

- NI-SWITCH supports two application development environments:
  * LabVIEW 2015, 2016, 2017, 2018
  * C and C++ (gcc)

  NI-SWITCH does not support other ADEs, such as the LabVIEW Real-Time
  Module, LabWindows/CVI, Microsoft Visual C/C++, or Microsoft Visual Basic.

- While NI-SWITCH includes LabVIEW VIs, it does not provide other graphical
  utilities that are available for other operating systems:
  * LabVIEW Express VIs
  * The Measurement and Automation Explorer (MAX) graphical configuration
    utility.
  * Soft front panels.

------------
Known Issues
------------

You can access the software and documentation known issues list online.

Visit ni.com/info and enter the Info Code exw24t for an up-to-date list
of known issues in NI-SWITCH 18.1.

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
