core Flight System (cFS) CFDP Application (CF)
======================================================

Open Source Release Readme
==========================

CF Release 3.0.0

Date: 09/28/2021

Introduction
-------------
  The CFDP application (CF) is a core Flight System (cFS) application
  that is a plug in to the Core Flight Executive (cFE) component of the cFS.

  CF 3.0 is a cFS application for providing CFDP (CCSDS File Delivery Protocol) 
  services. Its primary function is to provide file receive and transmit 
  functionality to this protocol. It works by mapping CFDP PDUs on and off cFS's 
  software bus.

  The CF application is written in C and depends on the cFS Operating System
  Abstraction Layer (OSAL) and cFE components.  There is additional CF application
  specific configuration information contained in the application user's guide.
  
  Developer's guide information can be generated using Doxygen:
  doxygen cf_doxygen_config.txt

 

Software Included
------------------

  CFDP application (CF) 3.0.0


Software Required
------------------

 cFS Caelum

 Note: An integrated bundle including the cFE, OSAL, and PSP can
 be obtained at https://github.com/nasa/cfs

About cFS
----------
  The cFS is a platform and project independent reusable software framework and
  set of reusable applications developed by NASA Goddard Space Flight Center.
  This framework is used as the basis for the flight software for satellite data
  systems and instruments, but can be used on other embedded systems.  More
  information on the cFS can be found at http://cfs.gsfc.nasa.gov

EOF
