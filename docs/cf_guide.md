# CF Version 3.0


## Overview

CF 3.0 is a cFS application for providing CFDP (CCSDS File Delivery Protocol) services. Its primary function is to provide file receive and transmit functionality to this protocol. It works by mapping CFDP PDUs on and off cFS's software bus.

#### Handling of Outgoing Messages

The priority of CF's use of available outgoing messages on each wakeup is:

1. Send any pending message required for RX transaction

2. Re-send any message required for TX transaction if timer expired (for example,
   send EOF, or re-send EOF if ack timer expired)

3. Send a filedata PDU in response to a NAK request. Per wakeup, one will be sent
   per each transaction in the TX wait state.

4. Once all TX wait transactions have processed their needs to send, the currently
   active TX transfer will send new filedata PDUs. It will keep sending them until
   there are no more messages on that wakeup (or the throttling semaphore stops it.)

#### Key Differences From CF v2.x

* There's no more text-based ground commands where strings are sent for commands.
* There's no more memory pool. Everything is set up with limits both in platform
  config for static memory limits and the configuration table for dynamic timing and functionality limits.
* Much smaller code footprint. CF 3.0 is light-weight flight-only app. It does not provide any ground engine support.

## Configuration

CF uses two sets of configuration parameters: compile-time configurable parameters
in the cf_platform_cfg.h file and run-time configurable parameters in the cf_def_cfg.c file.  Most parameters are included in the cf_def_cfg.c file for maximum flexibility.

CF expects to receive a CF_WAKEUP_MID message from the SCH (scheduler) app at a fixed rate. The number of wakeups per second is reflected in the configuration table. This drives CF's timing.

#### Channels

CF version 3.0 has a concept of "channels" which have their own
configuration. The channel configuration is done in the cf_def_cfg and allows
apids for incoming and outgoing PDUs to be unique per channel. Each channel can be
configured with polling directories as well.

In CF 3.0 each channel has an upper-bound on its state, and this is part of the
static memory allocation requirement.

#### Flow Control
By default, CF assumes that a per-channel semaphore is provided by the Telemetry Output (TO)
application or an equivalent application.  The semaphore name is defined in the
CF configuration table and must match the name of the semaphore created by TO. If
TO does not create a semaphore, the semaphore name can be left as an empty string
in the configuration table indicating that a semaphore should not be used.  If a
semaphore is expected and not found, the application will terminate during
initialization.  If no semaphore is used, the outgoing messages per wakeup may
need to be more limited. It's a valid configuration to have both the semaphore and maximum
outgoing messages per wakeup as well.

## Integration

#### Software Bus

Each channel has an input APID that is subscribed on, and an APID to publish its messages on.

#### Scheduler

CF as a whole expects the CF_WAKEUP_MID message as described earlier at a fixed rate. If the number of wakeups per second
is changed in SCH, then the ticks_per_second configuration parameter in the CF configuration table must also be updated.

#### Endianness

In order to compile, CF must have the endianness of the system specified during
compile time.  This can be done by adding one of the following settings to
the toolchain file in the the cFS *_defs directory:

* Little endian: -D_EL DENDIAN=_EL
* Big Endian: -D_EB DENDIAN=_EB

NOTE: cFS intends to standardize a way for compile-time endian awareness. This will be replaced by that someday.

#### Integration with TO

* TO's pipe needs to be able to receive packets of CF_PDU_MAX_SIZE

#### Received messages

Operationally, the flow of input packets from ground into CF should be throttled at some rate appropriate for ingest. CF
has a per-channel configuration item for max number of RX messages processed per wakeup, and both CF and the ingest app
need to be able to handle this.

