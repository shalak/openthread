/*
 *  Copyright (c) 2017, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>

#include <openthread/platform/alarm-micro.h>
#include <openthread/platform/alarm-milli.h>
#include <openthread/platform/diag.h>
#include <openthread/platform/logging.h>
#include <openthread/platform/misc.h>
#include <openthread/platform/radio.h>
#include <openthread/platform/settings.h>
#include <openthread/platform/uart.h>

typedef struct AlarmState
{
    uint32_t fire;
    bool     isRunning;
} AlarmState;

static uint32_t     sAlarmNow;
static AlarmState   sAlarmMilli;
static AlarmState   sAlarmMicro;
static uint32_t     sRandomState = 1;
static uint8_t      sRadioTransmitPsdu[OT_RADIO_FRAME_MAX_SIZE];
static otRadioFrame sRadioTransmitFrame = {.mPsdu = sRadioTransmitPsdu};

void FuzzerPlatformInit(void)
{
    sRandomState = 1;
    sAlarmNow    = 0;
    memset(&sAlarmMilli, 0, sizeof(sAlarmMilli));
    memset(&sAlarmMicro, 0, sizeof(sAlarmMicro));
}

void FuzzerPlatformProcess(otInstance *aInstance)
{
    if (sAlarmMilli.isRunning || sAlarmMicro.isRunning)
    {
        uint32_t fire = UINT32_MAX;

        if (sAlarmMilli.isRunning && fire > sAlarmMilli.fire)
        {
            fire = sAlarmMilli.fire;
        }

        if (sAlarmMicro.isRunning && fire > sAlarmMicro.fire)
        {
            fire = sAlarmMicro.fire;
        }

        sAlarmNow = fire;

        if (sAlarmMilli.isRunning && sAlarmNow >= sAlarmMilli.fire)
        {
            sAlarmMilli.isRunning = false;
            otPlatAlarmMilliFired(aInstance);
        }

#if OPENTHREAD_CONFIG_ENABLE_PLATFORM_USEC_TIMER
        if (sAlarmMicro.isRunning && sAlarmNow >= sAlarmMicro.fire)
        {
            sAlarmMicro.isRunning = false;
            otPlatAlarmMicroFired(aInstance);
        }
#endif
    }
}

uint32_t otPlatAlarmMilliGetNow(void)
{
    return sAlarmNow / 1000;
}

void otPlatAlarmMilliStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    OT_UNUSED_VARIABLE(aInstance);

    sAlarmMilli.fire      = (aT0 + aDt) * 1000;
    sAlarmMilli.isRunning = true;
}

void otPlatAlarmMilliStop(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    sAlarmMilli.isRunning = false;
}

uint32_t otPlatAlarmMicroGetNow(void)
{
    return sAlarmNow;
}

void otPlatAlarmMicroStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    OT_UNUSED_VARIABLE(aInstance);

    sAlarmMicro.fire      = aT0 + aDt;
    sAlarmMicro.isRunning = true;
}

void otPlatAlarmMicroStop(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    sAlarmMicro.isRunning = false;
}

bool otDiagIsEnabled(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return false;
}

void otDiagProcessCmd(otInstance *aInstance, int aArgCount, char *aArgVector[], char *aOutput, size_t aOutputMaxLen)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aArgCount);
    OT_UNUSED_VARIABLE(aArgVector);
    OT_UNUSED_VARIABLE(aOutput);
    OT_UNUSED_VARIABLE(aOutputMaxLen);
}

void otDiagProcessCmdLine(otInstance *aInstance, const char *aString, char *aOutput, size_t aOutputMaxLen)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aString);
    OT_UNUSED_VARIABLE(aOutput);
    OT_UNUSED_VARIABLE(aOutputMaxLen);
}

void otPlatReset(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
}

otPlatResetReason otPlatGetResetReason(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return OT_PLAT_RESET_REASON_POWER_ON;
}

void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, ...)
{
    OT_UNUSED_VARIABLE(aLogLevel);
    OT_UNUSED_VARIABLE(aLogRegion);
    OT_UNUSED_VARIABLE(aFormat);
}

void otPlatWakeHost(void)
{
}

void otPlatRadioGetIeeeEui64(otInstance *aInstance, uint8_t *aIeeeEui64)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aIeeeEui64);
}

void otPlatRadioSetPanId(otInstance *aInstance, uint16_t aPanId)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aPanId);
}

void otPlatRadioSetExtendedAddress(otInstance *aInstance, const otExtAddress *aExtAddr)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aExtAddr);
}

void otPlatRadioSetShortAddress(otInstance *aInstance, uint16_t aShortAddr)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aShortAddr);
}

void otPlatRadioSetPromiscuous(otInstance *aInstance, bool aEnabled)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aEnabled);
}

bool otPlatRadioIsEnabled(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return true;
}

otError otPlatRadioEnable(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return OT_ERROR_NONE;
}

otError otPlatRadioDisable(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return OT_ERROR_NONE;
}

otError otPlatRadioSleep(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return OT_ERROR_NONE;
}

otError otPlatRadioReceive(otInstance *aInstance, uint8_t aChannel)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aChannel);
    return OT_ERROR_NONE;
}

otError otPlatRadioTransmit(otInstance *aInstance, otRadioFrame *aFrame)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aFrame);
    return OT_ERROR_NONE;
}

otError otPlatRadioGetTransmitPower(otInstance *aInstance, int8_t *aPower)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aPower);
    return OT_ERROR_NONE;
}

otRadioFrame *otPlatRadioGetTransmitBuffer(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return &sRadioTransmitFrame;
}

int8_t otPlatRadioGetRssi(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return 0;
}

otRadioCaps otPlatRadioGetCaps(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return OT_RADIO_CAPS_NONE;
}

bool otPlatRadioGetPromiscuous(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return false;
}

void otPlatRadioEnableSrcMatch(otInstance *aInstance, bool aEnable)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aEnable);
}

otError otPlatRadioAddSrcMatchShortEntry(otInstance *aInstance, const uint16_t aShortAddress)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aShortAddress);
    return OT_ERROR_NONE;
}

otError otPlatRadioAddSrcMatchExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aExtAddress);
    return OT_ERROR_NONE;
}

otError otPlatRadioClearSrcMatchShortEntry(otInstance *aInstance, const uint16_t aShortAddress)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aShortAddress);
    return OT_ERROR_NONE;
}

otError otPlatRadioClearSrcMatchExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aExtAddress);
    return OT_ERROR_NONE;
}

void otPlatRadioClearSrcMatchShortEntries(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
}

void otPlatRadioClearSrcMatchExtEntries(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
}

otError otPlatRadioEnergyScan(otInstance *aInstance, uint8_t aScanChannel, uint16_t aScanDuration)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aScanChannel);
    OT_UNUSED_VARIABLE(aScanDuration);
    return OT_ERROR_NOT_IMPLEMENTED;
}

otError otPlatRadioSetTransmitPower(otInstance *aInstance, int8_t aPower)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aPower);
    return OT_ERROR_NOT_IMPLEMENTED;
}

int8_t otPlatRadioGetReceiveSensitivity(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return 0;
}

otError otPlatEntropyGet(uint8_t *aOutput, uint16_t aOutputLength)
{
    for (uint16_t length = 0; length < aOutputLength; length++)
    {
        aOutput[length] = (uint8_t)rand();
    }

    return OT_ERROR_NONE;
}

void otPlatSettingsInit(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
}

void otPlatSettingsDeinit(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
}

otError otPlatSettingsGet(otInstance *aInstance, uint16_t aKey, int aIndex, uint8_t *aValue, uint16_t *aValueLength)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aKey);
    OT_UNUSED_VARIABLE(aIndex);
    OT_UNUSED_VARIABLE(aValue);
    OT_UNUSED_VARIABLE(aValueLength);
    return OT_ERROR_NOT_FOUND;
}

otError otPlatSettingsSet(otInstance *aInstance, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aKey);
    OT_UNUSED_VARIABLE(aValue);
    OT_UNUSED_VARIABLE(aValueLength);
    return OT_ERROR_NONE;
}

otError otPlatSettingsAdd(otInstance *aInstance, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aKey);
    OT_UNUSED_VARIABLE(aValue);
    OT_UNUSED_VARIABLE(aValueLength);
    return OT_ERROR_NONE;
}

otError otPlatSettingsDelete(otInstance *aInstance, uint16_t aKey, int aIndex)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aKey);
    OT_UNUSED_VARIABLE(aIndex);
    return OT_ERROR_NONE;
}

void otPlatSettingsWipe(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
}

otError otPlatUartEnable(void)
{
    return OT_ERROR_NONE;
}

otError otPlatUartDisable(void)
{
    return OT_ERROR_NONE;
}

otError otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength)
{
    OT_UNUSED_VARIABLE(aBuf);
    OT_UNUSED_VARIABLE(aBufLength);
    return OT_ERROR_NONE;
}

void otPlatDiagProcess(otInstance *aInstance, int argc, char *argv[], char *aOutput, size_t aOutputMaxLen)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);
    OT_UNUSED_VARIABLE(aOutput);
    OT_UNUSED_VARIABLE(aOutputMaxLen);
}

void otPlatDiagModeSet(bool aMode)
{
    OT_UNUSED_VARIABLE(aMode);
}

bool otPlatDiagModeGet(void)
{
    return false;
}

void otPlatDiagChannelSet(uint8_t aChannel)
{
    OT_UNUSED_VARIABLE(aChannel);
}

void otPlatDiagTxPowerSet(int8_t aTxPower)
{
    OT_UNUSED_VARIABLE(aTxPower);
}

void otPlatDiagRadioReceived(otInstance *aInstance, otRadioFrame *aFrame, otError aError)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aFrame);
    OT_UNUSED_VARIABLE(aError);
}

void otPlatDiagAlarmCallback(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
}
