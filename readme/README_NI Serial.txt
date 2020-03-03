NI-Serial for Linux/x86 64-bit, Version 18.1
August 2018

--------------------------------------------------------------------------------
This file contains late breaking news about NI-Serial and supersedes any
information found in other documentation included in the distribution. Refer to
ni.com/linux for the most recent information about Linux support at National
Instruments.
--------------------------------------------------------------------------------

-----------------
Table of Contents
-----------------
System Requirements
Supported Hardware
Installation
Documentation
Linux Limitations
NI-Serial Does Not Support 32-bit Linux Distributions
Legal Information

-------------------
System Requirements
-------------------
NI-Serial for Linux software for the Linux/x86 64-bit architecture has been
tested on the following distributions:

    openSUSE Leap 42.3
    Red Hat Enterprise Linux Desktop + Workstation 6.x
    Red Hat Enterprise Linux Desktop + Workstation 7.x
    CentOS 7.x

For more information about supported Linux versions and distributions supported
by NI, refer to ni.com/linux.

--------------------
Supported Hardware
--------------------

--------------------------------------------------------------------------------------------------------
PXI Interfaces              Standard               # Ports          Isolated            Max Baud (kbaud)
--------------------------------------------------------------------------------------------------------
PXI-8431/2                  RS-485/RS-422           2                No                     3000.0
PXI-8431/4                  RS-485/RS-422           4                No                     3000.0
PXI-8431/8                  RS-485/RS-422           8                No                     3000.0
PXI-8433/2                  RS-485/RS-422           2                Yes                    3000.0
PXI-8433/4                  RS-485/RS-422           4                Yes                    3000.0


--------------------------------------------------------------------------------------------------------
PXI Express Interfaces      Standard               # Ports          Isolated            Max Baud (kbaud)
--------------------------------------------------------------------------------------------------------
PXIe-8430/8                 RS-232                  8                No                     1000.0
PXIe-8430/16                RS-232                  16               No                     1000.0
PXIe-8431/8                 RS-485/RS-422           8                No                     3000.0
PXIe-8431/16                RS-485/RS-422           16               No                     3000.0

1. The two-wire auto control mode for RS-485 transceiver control has a maximum baud rate of 2 Mbaud.
2. For possible use with higher baud rates, refer to ni.com/kb and search for KnowledgeBase 58KEI82F.
3. This port supplies connections only for transmit, receive, and ground.

------------
Installation
------------
National Instruments provides a repository add-on for all supported drivers
that you can install using your Linux distribution's native package manager.

1. Visit ni.com/info and enter the Info Code LinuxRepository.
2. Download the installer for the repository add-on for your Linux distribution.
3. Install the repository add-on using the rpm command.
4. Select and install the following NI-Serial driver packages through your
Linux distribution's package manager:
   - ni-serial

-------------
Documentation
-------------
Use a web browser to view the NI-Serial documentation at http://ni.com/manuals/.

-----------------
Linux Limitations
-----------------
Some features discussed in the NI-Serial documentation are
unavailable on Linux.

- While NI-Serial includes support for NI RS-485 serial hardware, it does
  not support wire mode configuration on all supported Linux distributions:
  * Red Hat Enterprise Linux Desktop + Workstation 6.x
  * Red Hat Enterprise Linux Desktop + Workstation 7.x
  * CentOS 7.x
  A default transceiver mode of 4 wire-auto will be configured upon install on
  these distributions.

----------------------------------------------------------------
NI-Serial Does Not Support 32-bit Linux Distributions
----------------------------------------------------------------
NI-Serial dropped support for Intel 32-bit versions of
Linux distributions on July 1, 2016. Versions of NI-Serial that ship after
July 1, 2016 may install and execute on Intel 32-bit distributions, but
official support is not provided.


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
EULAs and notices are located in the /usr/share/doc/ni-serial directory

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