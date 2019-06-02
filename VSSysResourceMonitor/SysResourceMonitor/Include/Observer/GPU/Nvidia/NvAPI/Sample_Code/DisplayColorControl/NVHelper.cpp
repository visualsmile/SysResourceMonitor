/************************************************************************************************************************************\
|*                                                                                                                                    *|
|*     Copyright © 2012 NVIDIA Corporation.  All rights reserved.                                                                     *|
|*                                                                                                                                    *|
|*  NOTICE TO USER:                                                                                                                   *|
|*                                                                                                                                    *|
|*  This software is subject to NVIDIA ownership rights under U.S. and international Copyright laws.                                  *|
|*                                                                                                                                    *|
|*  This software and the information contained herein are PROPRIETARY and CONFIDENTIAL to NVIDIA                                     *|
|*  and are being provided solely under the terms and conditions of an NVIDIA software license agreement                              *|
|*  and / or non-disclosure agreement.  Otherwise, you have no rights to use or access this software in any manner.                   *|
|*                                                                                                                                    *|
|*  If not covered by the applicable NVIDIA software license agreement:                                                               *|
|*  NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOFTWARE FOR ANY PURPOSE.                                            *|
|*  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.                                                           *|
|*  NVIDIA DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,                                                                     *|
|*  INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.                       *|
|*  IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,                               *|
|*  OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT,                         *|
|*  NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.            *|
|*                                                                                                                                    *|
|*  U.S. Government End Users.                                                                                                        *|
|*  This software is a "commercial item" as that term is defined at 48 C.F.R. 2.101 (OCT 1995),                                       *|
|*  consisting  of "commercial computer  software"  and "commercial computer software documentation"                                  *|
|*  as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995) and is provided to the U.S. Government only as a commercial end item.     *|
|*  Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through 227.7202-4 (JUNE 1995),                                          *|
|*  all U.S. Government End Users acquire the software with only those rights set forth herein.                                       *|
|*                                                                                                                                    *|
|*  Any use of this software in individual and commercial software must include,                                                      *|
|*  in the user documentation and internal comments to the code,                                                                      *|
|*  the above Disclaimer (as applicable) and U.S. Government End Users Notice.                                                        *|
|*                                                                                                                                    *|
\************************************************************************************************************************************/
//////////////////////////////////////////////////////////////////////////////////////////
// @brief:	This sample code retrieves information about the current blanking state
//			of the display as well as set or unset the blanking state of the display.
// 
// @driver support: R304+
//////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nvapi.h"
#include <windows.h>

extern int dumpToFile;

void Log(char * szDebugStr, ...)
{
    va_list vaList;
    static char szDebug[512] = { 0 };
    va_start(vaList, szDebugStr);
    _vsnprintf_s(szDebug, sizeof(szDebug), szDebugStr, vaList);
    printf("%s", szDebug);

    if (dumpToFile)
    {
        FILE* out;
        // Print to file.
        fopen_s(&out, "ColorControlToolLog.txt", "at");
        if (out)
        {
            fprintf(out, "%s", szDebug);
            fclose(out);
        }
    }
}

void LogError(char *str, NvAPI_Status nvapiErrorStatus)
{
    NvAPI_ShortString errorDescStr;
    NvAPI_GetErrorMessage(nvapiErrorStatus, errorDescStr);

    Log(str, (char *)errorDescStr);
}

NvAPI_Status Initialize_NVAPI()
{
    NvAPI_Status nvapiReturnStatus = NVAPI_ERROR;

    nvapiReturnStatus = NvAPI_Initialize();
    if (nvapiReturnStatus != NVAPI_OK)
    {
        LogError("NvAPI_Initialize() failed.\nReturn Error : %s\n", nvapiReturnStatus);
    }

    return nvapiReturnStatus;
}

NvAPI_Status GetGPUs(NvPhysicalGpuHandle gpuHandleArray[NVAPI_MAX_PHYSICAL_GPUS], NvU32 &gpuCount)
{
    NvAPI_Status nvapiReturnStatus = NVAPI_ERROR;

    // Get all gpu handles.
    nvapiReturnStatus = NvAPI_EnumPhysicalGPUs(gpuHandleArray, &gpuCount);
    if (nvapiReturnStatus != NVAPI_OK)
    {
        LogError("\nNvAPI_EnumPhysicalGPUs() failed.\nReturn Error : %s", nvapiReturnStatus);
        return nvapiReturnStatus;
    }

    return nvapiReturnStatus;
}

NvAPI_Status GetConnectedDisplays(NvPhysicalGpuHandle gpuHandle, NV_GPU_DISPLAYIDS **pDisplayID, NvU32 &displayIdCount)
{
    NvAPI_Status nvapiReturnStatus = NVAPI_ERROR;

    /////////////////////////////// Obtain the connected display ID's array
    nvapiReturnStatus = NvAPI_GPU_GetConnectedDisplayIds(gpuHandle, NULL, &displayIdCount, NV_GPU_CONNECTED_IDS_FLAG_UNCACHED);
    if (nvapiReturnStatus != NVAPI_OK)
    {
        LogError("\nNvAPI_GPU_GetConnectedDisplayIds() failed.\nReturn Error : %s", nvapiReturnStatus);
        return nvapiReturnStatus;
    }

    NV_GPU_DISPLAYIDS *pTempDisplayID = new NV_GPU_DISPLAYIDS[displayIdCount];
    memset(pTempDisplayID, 0, displayIdCount * sizeof(NV_GPU_DISPLAYIDS));

    pTempDisplayID->version = NV_GPU_DISPLAYIDS_VER;
    nvapiReturnStatus = NvAPI_GPU_GetConnectedDisplayIds(gpuHandle, pTempDisplayID, &displayIdCount, NV_GPU_CONNECTED_IDS_FLAG_UNCACHED);
    if (nvapiReturnStatus != NVAPI_OK)
    {
        LogError("\nNvAPI_GPU_GetConnectedDisplayIds() failed.\nReturn Error : %s", nvapiReturnStatus);
        return nvapiReturnStatus;
    }

    *pDisplayID = pTempDisplayID;

    return nvapiReturnStatus;
}

void GetColorControl()
{
    NvAPI_Status nvapiReturnStatus = NVAPI_ERROR;
    
    NV_GPU_DISPLAYIDS* pDisplayID = NULL;
    NvU32 displayIdCount = 0;

    NvPhysicalGpuHandle gpuHandleArray[NVAPI_MAX_PHYSICAL_GPUS] = { 0 };
    NvU32 gpuCount = 0;

    nvapiReturnStatus = GetGPUs(gpuHandleArray, gpuCount);
    if (nvapiReturnStatus != NVAPI_OK)
    {
        LogError("\n GetConnectedGPUs failed with error code : %s ", nvapiReturnStatus);
        return;
    }

    // Get all active outputs info for all gpu's
    for (NvU32 i = 0; i < gpuCount; ++i)
    {

        nvapiReturnStatus = GetConnectedDisplays(gpuHandleArray[i], &pDisplayID, displayIdCount);
        if (nvapiReturnStatus != NVAPI_OK)
        {
            LogError("\n GetConnectedDisplays failed with error code : %s ", nvapiReturnStatus);
            goto cleanup;
        }

        NV_COLOR_DATA colorData;
        memset(&colorData, 0, sizeof(NV_COLOR_DATA));

        //Common for Set and Get
        colorData.version = NV_COLOR_DATA_VER;
        colorData.size = sizeof(NV_COLOR_DATA);
        colorData.cmd = NV_COLOR_CMD_GET;

        for (NvU32 j = 0; j < displayIdCount; j++) 
        {
            nvapiReturnStatus = NvAPI_Disp_ColorControl(pDisplayID[j].displayId, &colorData);
            if (nvapiReturnStatus != NVAPI_OK)
            {
                LogError("\nNvAPI_Disp_ColorControl Get failed with error code : %s ", nvapiReturnStatus);
                goto cleanup;
            }
        }
    }

cleanup:
    if(pDisplayID)
    {
        delete[] pDisplayID;
        pDisplayID = 0;
    }

    Log("\nNvAPI_Disp_ColorControl Get successfully run");
    return;
}

void SetColorControl()
{
    NvAPI_Status nvapiReturnStatus = NVAPI_ERROR;

    NV_GPU_DISPLAYIDS* pDisplayID = NULL;
    NvU32 displayIdCount = 0;

    NvPhysicalGpuHandle gpuHandleArray[NVAPI_MAX_PHYSICAL_GPUS] = { 0 };
    NvU32 gpuCount = 0;

    nvapiReturnStatus = GetGPUs(gpuHandleArray, gpuCount);
    if (nvapiReturnStatus != NVAPI_OK)
    {
        LogError("\n GetConnectedGPUs failed with error code : %s ", nvapiReturnStatus);
        return;
    }

    // Get all active outputs info for all gpu's
    for (NvU32 i = 0; i < gpuCount; ++i)
    {
        nvapiReturnStatus = GetConnectedDisplays(gpuHandleArray[i], &pDisplayID, displayIdCount);
        if (nvapiReturnStatus != NVAPI_OK)
        {
            LogError("\n GetConnectedDisplays failed with error code : %s ", nvapiReturnStatus);
            goto cleanup;
        }

        NV_COLOR_DATA colorData;
        memset(&colorData, 0, sizeof(NV_COLOR_DATA));

        //Common for Set and Get
        colorData.version = NV_COLOR_DATA_VER;
        colorData.size = sizeof(NV_COLOR_DATA);

        //Set parameters
        colorData.data.bpc = NV_BPC_10;
        colorData.data.colorFormat = NV_COLOR_FORMAT_DEFAULT;
        colorData.data.colorimetry = NV_COLOR_COLORIMETRY_DEFAULT;
        colorData.data.colorSelectionPolicy = NV_COLOR_SELECTION_POLICY_USER;
        colorData.data.dynamicRange = NV_DYNAMIC_RANGE_AUTO;
        colorData.cmd = NV_COLOR_CMD_SET;                      //command for Set

        for (NvU32 j = 0; j < displayIdCount; j++) {
            nvapiReturnStatus = NvAPI_Disp_ColorControl(pDisplayID[j].displayId, &colorData);
            if (nvapiReturnStatus != NVAPI_OK)
            {
                LogError("\nNvAPI_Disp_ColorControl Set failed with error code : %s ", nvapiReturnStatus);
                goto cleanup;
            }
        }
    }

cleanup:
    if (pDisplayID)
    {
        delete[] pDisplayID;
        pDisplayID = 0;
    }

    Log("\nNvAPI_Disp_ColorControl Set successfully run");
    return;
}