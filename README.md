# core Flight System (cFS) CFDP Application (CF)

## Introduction

The CFDP application (CF) is a core Flight System (cFS) application
that is a plug in to the Core Flight Executive (cFE) component of the cFS.

CF is a cFS application for providing CFDP (CCSDS File Delivery Protocol) CCSDS 727.0-B-5 compliant
services. Its primary function is to provide file receive and transmit 
functionality to this protocol. It works by mapping CFDP PDUs on and off cFS's 
software bus.

The CF application is written in C and depends on the cFS Operating System
Abstraction Layer (OSAL) and cFE components.  There is additional CF application
specific configuration information contained in the application user's guide.

User's guide information can be generated using Doxygen (from top mission directory):
```
  make prep
  make -C build/docs/cf-usersguide cf-usersguide
```

## Software Required

cFS Framework (cFE, OSAL, PSP)

A demonstration bundle of the Core Flight System including the cFE, OSAL, and PSP can be obtained at https://github.com/nasa/cfs

For information about a mission ready cFS bundle, see: https://github.com/nasa/cFS#cfs-gov-mission-ready-version

## Known issues

See all [open issues](https://github.com/nasa/CF/issues) and closed to milestones later than this version.

## Getting Help

For best results, submit issues:questions or issues:help wanted requests at <https://github.com/nasa/cFS>.

Official cFS page: <http://cfs.gsfc.nasa.gov>

## About cFS

The cFS is a platform and project independent reusable software framework and
set of reusable applications developed by NASA Goddard Space Flight Center.
This framework is used as the basis for the flight software for satellite data
systems and instruments, but can be used on other embedded systems.  More
information on the cFS can be found at http://cfs.gsfc.nasa.gov
