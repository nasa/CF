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
 *  Define CF Performance IDs
 */

#ifndef CF_PERFIDS_H
#define CF_PERFIDS_H

#define CF_PERF_ID_APPMAIN    11
#define CF_PERF_ID_FSEEK      12
#define CF_PERF_ID_FOPEN      13
#define CF_PERF_ID_FCLOSE     14
#define CF_PERF_ID_FREAD      15
#define CF_PERF_ID_FWRITE     16
#define CF_PERF_ID_CYCLE_ENG  17
#define CF_PERF_ID_DIRREAD    18
#define CF_PERF_ID_CREAT      19
#define CF_PERF_ID_RENAME     20
#define CF_PERF_ID_PDURCVD(x) (30 + x)
#define CF_PERF_ID_PDUSENT(x) (40 + x)

#endif /* !CF_PERFIDS_H */

/************************/
/*  End of File Comment */
/************************/
