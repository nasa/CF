/************************************************************************
 *
 * NASA Docket No. GSC-18,447-1, and identified as “CFS CFDP (CF)
 * Application version 3.0.0”
 * Copyright © 2019 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ************************************************************************/

/**
 * @file
 *
 *  The CF Application Message IDs header file
 */

#ifndef CF_MSGIDS_H
#define CF_MSGIDS_H

/**************************
 * CF Command Message IDs
 **************************/

#define CF_CMD_MID     0x18B3
#define CF_SEND_HK_MID 0x18B4
#define CF_WAKE_UP_MID 0x18B5

/***************************
 * CF Telemetry Message IDs
 ***************************/

#define CF_HK_TLM_MID     0x08B0
#define CF_CONFIG_TLM_MID 0x08B2

#endif /* !CF_MSGIDS_H */
