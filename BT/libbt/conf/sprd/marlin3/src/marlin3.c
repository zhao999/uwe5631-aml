/******************************************************************************
 *
 *  Copyright (C) 2016 Spreadtrum Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#define LOG_TAG "bt_chip_vendor"

#include <log/log.h>
#include <string.h>
#include <cutils/properties.h>
#include "marlin3.h"
#include "bt_hci_bdroid.h"
#include "bt_vendor_sprd_hci.h"
#include "conf.h"
#include "upio.h"
#include <stdio.h>
#include <unistd.h>

// pskey file structure default value
static pskey_config_t marlin3_pskey;

static const conf_entry_t marlin3_pksey_table[] = {
    CONF_ITEM_TABLE(device_class, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(feature_set, 0, marlin3_pskey, 16),
    CONF_ITEM_TABLE(device_addr, 0, marlin3_pskey, 6),
    CONF_ITEM_TABLE(comp_id, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(g_sys_uart0_communication_supported, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(cp2_log_mode, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(LogLevel, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(g_central_or_perpheral, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(Log_BitMask, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(super_ssp_enable, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(common_rfu_b3, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(common_rfu_w, 0, marlin3_pskey, 2),
    CONF_ITEM_TABLE(le_rfu_w, 0, marlin3_pskey, 2),
    CONF_ITEM_TABLE(lmp_rfu_w, 0, marlin3_pskey, 2),
    CONF_ITEM_TABLE(lc_rfu_w, 0, marlin3_pskey, 2),
    CONF_ITEM_TABLE(g_wbs_nv_117, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(g_wbs_nv_118, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(g_nbv_nv_117, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(g_nbv_nv_118, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(g_sys_sco_transmit_mode, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(audio_rfu_b1, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(audio_rfu_b2, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(audio_rfu_b3, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(audio_rfu_w, 0, marlin3_pskey, 2),
    CONF_ITEM_TABLE(g_sys_sleep_in_standby_supported, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(g_sys_sleep_master_supported, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(g_sys_sleep_slave_supported, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(power_rfu_b1, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(power_rfu_w, 0, marlin3_pskey, 2),
    CONF_ITEM_TABLE(win_ext, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(edr_tx_edr_delay, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(edr_rx_edr_delay, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(tx_delay, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(rx_delay, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(bb_rfu_w, 0, marlin3_pskey, 2),
    CONF_ITEM_TABLE(agc_mode, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(diff_or_eq, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(ramp_mode, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(modem_rfu_b1, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(modem_rfu_w, 0, marlin3_pskey, 2),
    CONF_ITEM_TABLE(BQB_BitMask_1, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(BQB_BitMask_2, 0, marlin3_pskey, 1),
    CONF_ITEM_TABLE(bt_coex_threshold, 0, marlin3_pskey, 8),
    CONF_ITEM_TABLE(other_rfu_w, 0, marlin3_pskey, 6),

    {0, 0, 0, 0, 0}
};

static rf_config_t marlin3_rf_config;

static const conf_entry_t marlin3_rf_table[] = {
    CONF_ITEM_TABLE(g_GainValue_A, 0, marlin3_rf_config, 6),
    CONF_ITEM_TABLE(g_ClassicPowerValue_A, 0, marlin3_rf_config, 10),
    CONF_ITEM_TABLE(g_LEPowerValue_A, 0, marlin3_rf_config, 16),
    CONF_ITEM_TABLE(g_BRChannelpwrvalue_A, 0, marlin3_rf_config, 8),
    CONF_ITEM_TABLE(g_EDRChannelpwrvalue_A, 0, marlin3_rf_config, 8),
    CONF_ITEM_TABLE(g_LEChannelpwrvalue_A, 0, marlin3_rf_config, 8),
    CONF_ITEM_TABLE(g_GainValue_B, 0, marlin3_rf_config, 6),
    CONF_ITEM_TABLE(g_ClassicPowerValue_B, 0, marlin3_rf_config, 10),
    CONF_ITEM_TABLE(g_LEPowerValue_B, 0, marlin3_rf_config, 16),
    CONF_ITEM_TABLE(g_BRChannelpwrvalue_B, 0, marlin3_rf_config, 8),
    CONF_ITEM_TABLE(g_EDRChannelpwrvalue_B, 0, marlin3_rf_config, 8),
    CONF_ITEM_TABLE(g_LEChannelpwrvalue_B, 0, marlin3_rf_config, 8),
    CONF_ITEM_TABLE(LE_fix_powerword, 0, marlin3_rf_config, 1),
    CONF_ITEM_TABLE(Classic_pc_by_channel, 0, marlin3_rf_config, 1),
    CONF_ITEM_TABLE(LE_pc_by_channel, 0, marlin3_rf_config, 1),
    CONF_ITEM_TABLE(RF_switch_mode, 0, marlin3_rf_config, 1),
    CONF_ITEM_TABLE(Data_Capture_Mode, 0, marlin3_rf_config, 1),
    CONF_ITEM_TABLE(Analog_IQ_Debug_Mode, 0, marlin3_rf_config, 1),
    CONF_ITEM_TABLE(RF_common_rfu_b3, 0, marlin3_rf_config, 1),
    CONF_ITEM_TABLE(RF_common_rfu_w, 0, marlin3_rf_config, 5),

    {0, 0, 0, 0, 0}

};

static void hw_core_cback(void *p_mem)
{
    HC_BT_HDR *p_evt_buf = (HC_BT_HDR *) p_mem;
    char        *p_name, *p_tmp;
    uint8_t     *p, status;
    uint16_t    opcode, mode;

    p = (uint8_t *)(p_evt_buf + 1) + HCI_EVT_CMD_CMPL_OPCODE;
    STREAM_TO_UINT16(opcode,p);
    STREAM_TO_UINT16(mode,p);
    STREAM_TO_UINT8(status,p);
    ALOGI("%s hw_core_cback response: [0x%04X, 0x%04X, 0x%02X]", __func__, opcode, mode, status);

    if (bt_vendor_cbacks)
    {
        /* Must free the RX event buffer */
        bt_vendor_cbacks->dealloc(p_evt_buf);
    }

    if (status) {
        if(1 == marlin3_pskey.super_ssp_enable) {
            sprd_vnd_ssp_init();
        } else {
            bt_vendor_cbacks->fwcfg_cb(BT_VND_OP_RESULT_SUCCESS);
        }
    } else if (status == 0){
        bt_vendor_cbacks->epilog_cb(BT_VND_OP_RESULT_SUCCESS);
        bt_sprd_spec_cb(BT_VND_OP_RESULT_SUCCESS);
    }

    //upio_set(UPIO_BT_WAKE, UPIO_DEASSERT, 0);

}

void hw_core_enable(unsigned char enable)
{
    HC_BT_HDR *p_buf = NULL;
    uint8_t *p;
    int i = 0;

    ALOGI("%s", __func__);

    /* Sending a HCI_RESET */
    if (bt_vendor_cbacks) {
        /* Must allocate command buffer via HC's alloc API */
        p_buf = (HC_BT_HDR *)bt_vendor_cbacks->alloc(
        BT_HC_HDR_SIZE + HCI_CMD_PREAMBLE_SIZE + START_STOP_CMD_SIZE);
    }

    if (p_buf) {
        ALOGI("hw_core_enable : %d", enable);
        p_buf->event = MSG_STACK_TO_HC_HCI_CMD;
        p_buf->offset = 0;
        p_buf->layer_specific = 0;
        p_buf->len = START_STOP_CMD_SIZE + 3;

        p = (uint8_t *)(p_buf + 1);
        UINT16_TO_STREAM(p, HCI_VSC_ENABLE_COMMMAND);
        *p++ = START_STOP_CMD_SIZE; /* parameter length */
        UINT16_TO_STREAM(p, DUAL_MODE);
        *p = enable ? ENABLE_BT : DISABLE_BT;
        upio_set(UPIO_BT_WAKE, UPIO_ASSERT, 0);
        /* Send command via HC's xmit_cb API */
        bt_vendor_cbacks->xmit_cb(HCI_VSC_ENABLE_COMMMAND, p_buf, hw_core_cback);
    } else {
        ALOGI("hw_pskey_send dont send pskey");
        if (bt_vendor_cbacks) {
            ALOGE("vendor lib hw_pskey_send aborted [no buffer]");
        }
    }
}
static void marlin3_pskey_dump(void *arg)
{
    UNUSED(arg);
}


static void hw_rf_cback(void *p_mem)
{
    HC_BT_HDR *p_evt_buf = (HC_BT_HDR *) p_mem;
    char        *p_name, *p_tmp;
    uint8_t     *p, status;
    uint16_t    opcode, mode;

    p = (uint8_t *)(p_evt_buf + 1) + HCI_EVT_CMD_CMPL_OPCODE;
    STREAM_TO_UINT16(opcode,p);
    STREAM_TO_UINT16(mode,p);
    STREAM_TO_UINT8(status,p);

    ALOGI("%s hw_core_cback response: [0x%04X, 0x%04X, 0x%02X]", __func__, opcode, mode, status);
    if (bt_vendor_cbacks)
    {
        /* Must free the RX event buffer */
        bt_vendor_cbacks->dealloc(p_evt_buf);
    }

    hw_core_enable(1);

}


static int marlin3_rf_preload()
{
    uint8_t *p, msg_req[HCI_CMD_MAX_LEN], tmp = 0;
    int i;

    ALOGI("%s", __FUNCTION__);
    p = msg_req;

    for (i = 0; i < 6; i++) {
        UINT16_TO_STREAM(p, marlin3_rf_config.g_GainValue_A[i]);
    }

    for (i = 0; i < 10; i++) {
        UINT16_TO_STREAM(p, marlin3_rf_config.g_ClassicPowerValue_A[i]);
    }

    for (i = 0; i < 16; i++) {
        UINT16_TO_STREAM(p, marlin3_rf_config.g_LEPowerValue_A[i]);
    }

    for (i = 0; i < 8; i++) {
        UINT16_TO_STREAM(p, marlin3_rf_config.g_BRChannelpwrvalue_A[i]);
    }

    for (i = 0; i < 8; i++) {
        UINT16_TO_STREAM(p, marlin3_rf_config.g_EDRChannelpwrvalue_A[i]);
    }

    for (i = 0; i < 8; i++) {
        UINT16_TO_STREAM(p, marlin3_rf_config.g_LEChannelpwrvalue_A[i]);
    }

    for (i = 0; i < 6; i++) {
        UINT16_TO_STREAM(p, marlin3_rf_config.g_GainValue_B[i]);
    }

    for (i = 0; i < 10; i++) {
        UINT16_TO_STREAM(p, marlin3_rf_config.g_ClassicPowerValue_B[i]);
    }


    for (i = 0; i < 16; i++) {
        UINT16_TO_STREAM(p, marlin3_rf_config.g_LEPowerValue_B[i]);
    }

    for (i = 0; i < 8; i++) {
        UINT16_TO_STREAM(p, marlin3_rf_config.g_BRChannelpwrvalue_B[i]);
    }

    for (i = 0; i < 8; i++) {
        UINT16_TO_STREAM(p, marlin3_rf_config.g_EDRChannelpwrvalue_B[i]);
    }

    for (i = 0; i < 8; i++) {
        UINT16_TO_STREAM(p, marlin3_rf_config.g_LEChannelpwrvalue_B[i]);
    }

    UINT16_TO_STREAM(p, marlin3_rf_config.LE_fix_powerword);

    UINT8_TO_STREAM(p, marlin3_rf_config.Classic_pc_by_channel);
    UINT8_TO_STREAM(p, marlin3_rf_config.LE_pc_by_channel);
    UINT8_TO_STREAM(p, marlin3_rf_config.RF_switch_mode);
    UINT8_TO_STREAM(p, marlin3_rf_config.Data_Capture_Mode);
    UINT8_TO_STREAM(p, marlin3_rf_config.Analog_IQ_Debug_Mode);
    UINT8_TO_STREAM(p, marlin3_rf_config.RF_common_rfu_b3);

    for (i = 0; i < 5; i++) {
        UINT32_TO_STREAM(p, marlin3_rf_config.RF_common_rfu_w[i]);
    }

    sprd_vnd_send_hci_vsc(HCI_RF_PARA, msg_req, (uint8_t)(p - msg_req), hw_rf_cback);
    return 0;
}


static void marlin3_pskey_cback(void *p_mem)
{
    HC_BT_HDR *p_evt_buf = (HC_BT_HDR *)p_mem;
    char buf[] = FW_DEFAULT_PROP;

    uint16_t opcode, node, year;
    uint8_t *p, status, month, day;

    status = *((uint8_t *)(p_evt_buf + 1) + HCI_EVT_CMD_CMPL_STATUS_RET_BYTE);
    p = (uint8_t *)(p_evt_buf + 1) + HCI_EVT_CMD_CMPL_OPCODE;
    STREAM_TO_UINT16(opcode, p);

    p = (uint8_t *)(p_evt_buf + 1) + FW_NODE_BYTE;
    STREAM_TO_UINT16(node, p);
    p = (uint8_t *)(p_evt_buf + 1) + FW_DATE_Y_BYTE;
    STREAM_TO_UINT16(year, p);
    p = (uint8_t *)(p_evt_buf + 1) + FW_DATE_M_BYTE;
    STREAM_TO_UINT8(month, p);
    p = (uint8_t *)(p_evt_buf + 1) + FW_DATE_D_BYTE;
    STREAM_TO_UINT8(day, p);

    ALOGI("Bluetooth Firmware Node: %04X Date: %04x-%02x-%02x", node, year, month,
          day);
    snprintf(buf, sizeof(buf), "%04x.%04x.%02x.%02x", node, year, month, day);
    property_set(FW_PROP_NAME, buf);

    if (bt_vendor_cbacks) {
        /* Must free the RX event buffer */
        bt_vendor_cbacks->dealloc(p_evt_buf);
    }
    marlin3_rf_preload();
}


static int marlin3_pskey_preload(void *arg)
{
    uint8_t *p, msg_req[HCI_CMD_MAX_LEN], tmp = 0;
    int i;

    UNUSED(arg);

    ALOGI("%s", __FUNCTION__);
    p = msg_req;
    UINT32_TO_STREAM(p, marlin3_pskey.device_class);

    for (i = 0; i < 16; i++) {
        UINT8_TO_STREAM(p, marlin3_pskey.feature_set[i]);
    }

    for (i = 0; i < 6; i++) {
        UINT8_TO_STREAM(p, marlin3_pskey.device_addr[i]);
    }

    UINT16_TO_STREAM(p, marlin3_pskey.comp_id);
    UINT8_TO_STREAM(p, marlin3_pskey.g_sys_uart0_communication_supported);
    UINT8_TO_STREAM(p, marlin3_pskey.cp2_log_mode);
    UINT8_TO_STREAM(p, marlin3_pskey.LogLevel);
    UINT8_TO_STREAM(p, marlin3_pskey.g_central_or_perpheral);

    UINT16_TO_STREAM(p, marlin3_pskey.Log_BitMask);
    UINT8_TO_STREAM(p, marlin3_pskey.super_ssp_enable);
    UINT8_TO_STREAM(p, marlin3_pskey.common_rfu_b3);

    for (i = 0; i < 2; i++) {
        UINT32_TO_STREAM(p, marlin3_pskey.common_rfu_w[i]);
    }

    for (i = 0; i < 2; i++) {
        UINT32_TO_STREAM(p, marlin3_pskey.le_rfu_w[i]);
    }

    for (i = 0; i < 2; i++) {
        UINT32_TO_STREAM(p, marlin3_pskey.lmp_rfu_w[i]);
    }

    for (i = 0; i < 2; i++) {
        UINT32_TO_STREAM(p, marlin3_pskey.lc_rfu_w[i]);
    }

    UINT16_TO_STREAM(p, marlin3_pskey.g_wbs_nv_117);
    UINT16_TO_STREAM(p, marlin3_pskey.g_wbs_nv_118);
    UINT16_TO_STREAM(p, marlin3_pskey.g_nbv_nv_117);
    UINT16_TO_STREAM(p, marlin3_pskey.g_nbv_nv_118);

    UINT8_TO_STREAM(p, marlin3_pskey.g_sys_sco_transmit_mode);
    UINT8_TO_STREAM(p, marlin3_pskey.audio_rfu_b1);
    UINT8_TO_STREAM(p, marlin3_pskey.audio_rfu_b2);
    UINT8_TO_STREAM(p, marlin3_pskey.audio_rfu_b3);

    for (i = 0; i < 2; i++) {
        UINT32_TO_STREAM(p, marlin3_pskey.audio_rfu_w[i]);
    }

    UINT8_TO_STREAM(p, marlin3_pskey.g_sys_sleep_in_standby_supported);
    UINT8_TO_STREAM(p, marlin3_pskey.g_sys_sleep_master_supported);
    UINT8_TO_STREAM(p, marlin3_pskey.g_sys_sleep_slave_supported);
    UINT8_TO_STREAM(p, marlin3_pskey.power_rfu_b1);

    for (i = 0; i < 2; i++) {
        UINT32_TO_STREAM(p, marlin3_pskey.power_rfu_w[i]);
    }

    UINT32_TO_STREAM(p, marlin3_pskey.win_ext);

    UINT8_TO_STREAM(p, marlin3_pskey.edr_tx_edr_delay);
    UINT8_TO_STREAM(p, marlin3_pskey.edr_rx_edr_delay);
    UINT8_TO_STREAM(p, marlin3_pskey.tx_delay);
    UINT8_TO_STREAM(p, marlin3_pskey.rx_delay);

    for (i = 0; i < 2; i++) {
        UINT32_TO_STREAM(p, marlin3_pskey.bb_rfu_w[i]);
    }

    UINT8_TO_STREAM(p, marlin3_pskey.agc_mode);
    UINT8_TO_STREAM(p, marlin3_pskey.diff_or_eq);
    UINT8_TO_STREAM(p, marlin3_pskey.ramp_mode);
    UINT8_TO_STREAM(p, marlin3_pskey.modem_rfu_b1);

    for (i = 0; i < 2; i++) {
        UINT32_TO_STREAM(p, marlin3_pskey.modem_rfu_w[i]);
    }

    UINT32_TO_STREAM(p, marlin3_pskey.BQB_BitMask_1);
    UINT32_TO_STREAM(p, marlin3_pskey.BQB_BitMask_2);
    for (i = 0; i < 8; i++) {
        UINT16_TO_STREAM(p, marlin3_pskey.bt_coex_threshold[i]);
    }

    for (i = 0; i < 6; i++) {
        UINT32_TO_STREAM(p, marlin3_pskey.other_rfu_w[i]);
    }

    sprd_vnd_send_hci_vsc(HCI_PSKEY, msg_req, (uint8_t)(p - msg_req), marlin3_pskey_cback);
    return 0;
}

static void marlin3_epilog_process() {
    hw_core_enable(0);
}

/******/
static int rfkill_id_test = -1;
static char rfkill_state_path_test[128] = {0};

static int init_rfkill_test()
{
    char path[64] = {0};
    char buf[32] = {0};
    int fd, sz, id;

    for (id = 0; id < 5 ; id++)
    {
        snprintf(path, sizeof(path), "/sys/class/rfkill/rfkill%d/name", id);
        ALOGE("%s:path==%s\n", __func__,path);
        fd = open(path, O_RDONLY);

        if (fd < 0)
        {
            ALOGE("%s : open(%s) failed: %s (%d)", __func__, path, strerror(errno), errno);
            continue;
        }
        memset(buf, 0, sizeof(buf));
        sz = read(fd, buf, sizeof(buf));
        close(fd);
        if (sz >= 9 && memcmp(buf, "bluetooth", 9) == 0)
        {
            rfkill_id_test = id;
            ALOGE("%s : select path %s\n", __func__, path);
            break;
        }
        else
        {
            buf[sizeof(buf) - 1] = 0;
            ALOGE("%s: %s is not match %s\n", __func__, path, buf);
        }
    }


    snprintf(rfkill_state_path_test, sizeof(rfkill_state_path_test), "/sys/class/rfkill/rfkill%d/state", rfkill_id_test);
    ALOGE("rfkill_state_path=%s\n",rfkill_state_path_test);

    if (rfkill_id_test < 0)
    {
        ALOGE("init_rfkill_test : not found\n");
        return -1;
    }

    return 0;
}



static int upio_set_bluetooth_power_test(void)
{

    int sz;
    int fd = -1;
    int ret = -1;
    char buffer = '1';
    if (rfkill_id_test == -1)
    {
        if (init_rfkill_test())
        {
            return ret;
        }
    }
    fd = open(rfkill_state_path_test, O_WRONLY);

    if (fd < 0)
    {
        ALOGE("set_bluetooth_power : open(%s) for write failed: %s (%d)", rfkill_state_path_test, strerror(errno), errno);
        return ret;
    }

    sz = write(fd, &buffer, 1);
    if (sz < 0)
    {
        ALOGE("set_bluetooth_power : write(%s) failed: %s (%d)", rfkill_state_path_test, strerror(errno), errno);
        close(fd);
        return sz;
    }
    else
    {
        close(fd);
    }

    return 0;
}


/*****/
#define UNISOC_BT_INI_PATH "/vendor/etc"
#define UNISOC_BT_NODE_DIR "/sys/devices/platform/mtty.0/"
#define UNISOC_BT_NODE_DIR_OLD "/sys/devices/platform/mtty/"
typedef enum wcn_chip_model_for_kernel
{
    WCN_CHIP_INVALID,
    WCN_CHIP_MARLIN3,
    WCN_CHIP_MARLIN3L,
    WCN_CHIP_MARLIN3E,
}WCN_CHIP_MODEL_FOR_KERNEL;

typedef struct
{
    WCN_CHIP_MODEL_FOR_KERNEL model;
    char file[64];
}WCN_CHIP_INI_MAP;

static int marlin3_init(void) {
    int ret;
    char ssp_property[128] = {0};

    int ant_num = 0, fd = -1;
    int chipid = WCN_CHIP_INVALID;
    char node_buf[32] = {0};
    char etc_path_final[128] = {0};
    char driver_node_file[64] = {0}, *driver_node_path = NULL;

    const WCN_CHIP_INI_MAP ini_map[] =
    {
        {WCN_CHIP_INVALID, "invaild"},
        {WCN_CHIP_MARLIN3, "marlin3"},
        {WCN_CHIP_MARLIN3L, "marlin3lite"},
        {WCN_CHIP_MARLIN3E, "marlin3e"},
    };

    ALOGI("%s", __func__);
    memset(&marlin3_pskey, 0, sizeof(marlin3_pskey));
    memset(&marlin3_rf_config, 0, sizeof(marlin3_rf_config));

    //vnd_load_configure("/vendor/etc/bt_configure_pskey.ini", &marlin3_pksey_table[0]);
    //vnd_load_configure("/vendor/etc/bt_configure_rf.ini", &marlin3_rf_table[0]);
    vnd_load_configure(UNISOC_BT_INI_PATH "/bt_configure_pskey.ini", &marlin3_pksey_table[0]);
    upio_set_bluetooth_power_test();  //rfkill power
    driver_node_path = UNISOC_BT_NODE_DIR;
    if (access(driver_node_path, X_OK) != 0)
    {
      //BTE("%s access err %s %d %s", __func__, driver_node_path, errno, strerror(errno));
       ALOGE("file_path_changed_%s", __func__);
       driver_node_path = UNISOC_BT_NODE_DIR_OLD;

       if (access(driver_node_path, X_OK) != 0)
       {

           ALOGE("%s UNISOC_BT_NODE_DIR_old", __func__);
           return -1;
       }
    }

    ALOGE("%s select %s", __func__, driver_node_path);

    memset(driver_node_file, 0, sizeof(driver_node_file));
    strcpy(driver_node_file, driver_node_path);
    strcat(driver_node_file, "ant_num");
    fd = open(driver_node_file, O_RDONLY);
    if(fd < 0)
    {
          ALOGE("%s open ant_num err", __func__);
         // BTE("%s open %s err %d %s", __func__, driver_node_file, errno, strerror(errno));
          return -1;
    }
    else
    {
        ret = read(fd, node_buf, sizeof(node_buf));
        if(ret <= 0)
        {
            ALOGE("%s read ant_num err %d %d", __func__, ret, errno);
            return -1;
        }
        else
        {
            ant_num = atoi(node_buf);
           //ÌìÏßÊýÊ¶±ð
            ALOGI("%s read ant_num %s int %d", __func__, node_buf, ant_num);
        }
        close(fd);
    }

     memset(driver_node_file, 0, sizeof(driver_node_file));
     strcpy(driver_node_file, driver_node_path);
     strcat(driver_node_file, "chipid");
     ALOGE("unisoc_%s open chipid", __func__);
     fd = open(driver_node_file, O_RDONLY);
     if(fd < 0)
     {
        ALOGE("%s open chipid err %d", __func__, errno);
        return -1;
     }
     else
     {
        ret = read(fd, node_buf, sizeof(node_buf));
        if(ret <= 0)
        {
            ALOGE("%s read chipid err %d %d", __func__, ret, errno);
            return -1;
        }
        else
        {
            //chipid Ê¶±ð

            chipid = atoi(node_buf);

            ALOGI("unisoc_%s read chipid %s int %d", __func__, node_buf, chipid);
         }
         close(fd);

     }


    sprintf(etc_path_final, "%s/bt_configure_rf_%s_%d.ini", UNISOC_BT_INI_PATH, ini_map[chipid].file, ant_num);
    //sprintf(etc_path_final, "%s/bt_configure_rf_%s_%d.ini", UNISOC_BT_INI_PATH, "marlin3", ant_num);

    ALOGE("gy_rf_path-%s__path=%s", __func__, etc_path_final);
    vnd_load_configure(etc_path_final, &marlin3_rf_table[0]);



    set_mac_address(marlin3_pskey.device_addr);

    ret = property_get("persist.vendor.sys.bt.non.ssp", ssp_property, "close");
    if (ret >= 0 && !strcmp(ssp_property, "open")) {
        ALOGI("### disable BT SSP function due to property setting ###");
        ALOGI("SSP setting pskey from : 0x%02x to: 0x%02x", marlin3_pskey.feature_set[6],
              (marlin3_pskey.feature_set[6] & 0xF7));
        marlin3_pskey.feature_set[6] &= 0xF7;
    }

    return 0;
}

static bt_adapter_module_t marlin_module = {
    .name = "marlin3",
    .pskey = &marlin3_pskey,
    .init = marlin3_init,
    .tab = (conf_entry_t *)&marlin3_pksey_table,
    .pskey_preload = marlin3_pskey_preload,
    .epilog_process = marlin3_epilog_process,
    .pskey_dump = marlin3_pskey_dump,
    .get_conf_file = NULL
};

const bt_adapter_module_t *get_adapter_module(void)
{
    return &marlin_module;
}
