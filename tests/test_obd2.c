// SPDX-License-Identifier: GPL-3.0-or-later
#include "audilink/obd2.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "FAIL: %s\n", #x); return 1; } } while (0)
int main(void)
{
    char command[16], dtc[LINK_OBD2_DTC_TEXT_LENGTH];
    char short_command[2] = { 'X', '\0' };
    uint16_t did = 0U;
    uint8_t rpm_payload[] = { 0x1aU, 0xf8U };
    AudilinkObd2DecodedPid decoded;
    CHECK(audilink_obd2_pid_definition_count() > 0U);
    CHECK(audilink_obd2_mode01_assigned_count() > 0U);
    CHECK(audilink_obd2_build_supported_pid_request(0x00U, command, sizeof(command)) == AUDILINK_OBD2_RESULT_OK);
    CHECK(strcmp(command, "0100") == 0);
    CHECK(audilink_obd2_build_live_pid_request(0x0cU, command, sizeof(command)) == AUDILINK_OBD2_RESULT_OK);
    CHECK(strcmp(command, "010C") == 0);
    /* The product inherits Link's safe rejection of invalid output buffers. */
    CHECK(audilink_obd2_build_live_pid_request(0x0cU, short_command, sizeof(short_command)) ==
          LINK_OBD2_RESULT_BUFFER_TOO_SMALL);
    CHECK(short_command[0] == '\0');
    CHECK(audilink_obd2_build_live_pid_request(0x0cU, NULL, sizeof(command)) ==
          LINK_OBD2_RESULT_INVALID_ARGUMENT);
    CHECK(audilink_obd2_build_vin_request(command, sizeof(command)) == AUDILINK_OBD2_RESULT_OK);
    CHECK(strcmp(command, "0902") == 0);
    CHECK(audilink_obd2_build_dtc_request(AUDILINK_OBD2_DTC_STORED, command, sizeof(command)) == AUDILINK_OBD2_RESULT_OK);
    CHECK(strcmp(command, "03") == 0);
    CHECK(audilink_obd2_decode_pid_payload(0x01U, 0x0cU, rpm_payload, sizeof(rpm_payload), &decoded) == AUDILINK_OBD2_RESULT_OK);
    CHECK(decoded.signal_count >= 1U);
    CHECK(decoded.signals[0].value > 1725.999 && decoded.signals[0].value < 1726.001);
    CHECK(audilink_obd2_decode_dtc_pair(0x01U, 0x33U, dtc) == AUDILINK_OBD2_RESULT_OK);
    CHECK(strcmp(dtc, "P0133") == 0);
    CHECK(audilink_obd2_obdonuds_pid_to_did(0x000cU, &did) == AUDILINK_OBD2_RESULT_OK);
    CHECK(did == 0xf40cU);
    puts("AUDILink inherited standard OBD-II baseline passed");
    return 0;
}
