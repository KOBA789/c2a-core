#pragma section REPRO
/**
 * @file
 * @brief UART と DriverSuper テスト用
 */
#include "di_uart_test.h"
#include <stddef.h> // for NULL
#include <src_core/Library/print.h>
#include <src_core/TlmCmd/common_cmd_packet_util.h>
#include "../../Settings/port_config.h"
#include "../../Settings/DriverSuper/driver_buffer_define.h"

// FIXME: 接頭辞を DI_UART_TEST に

static UART_TEST_Driver uart_test_instance_;
const UART_TEST_Driver* uart_test_instance;

// バッファ
static uint8_t UART_TEST_rx_buffer_allocation_0_[DS_STREAM_REC_BUFFER_SIZE_DEFAULT];
static uint8_t UART_TEST_rx_buffer_allocation_1_[DS_STREAM_REC_BUFFER_SIZE_DEFAULT];
static DS_StreamRecBuffer UART_TEST_rx_buffer_0_;
static DS_StreamRecBuffer UART_TEST_rx_buffer_1_;

static void UART_TEST_init_by_AM_(void);
static void UART_TEST_init_(void);
static void UART_TEST_update_(void);


// !!!!!!!!!! 注意 !!!!!!!!!!
// このDriverは，AM_initialize_app_ では初期化されない！！
// initコマンドで初期化すること
AppInfo UART_TEST_update(void)
{
  return AI_create_app_info("update_uart_test", UART_TEST_init_by_AM_, UART_TEST_update_);
}


static void UART_TEST_init_by_AM_(void)
{
  uart_test_instance = &uart_test_instance_;
}


static void UART_TEST_init_(void)
{
  DS_INIT_ERR_CODE ret;

  DS_init_stream_rec_buffer(&UART_TEST_rx_buffer_0_,
                            UART_TEST_rx_buffer_allocation_0_,
                            sizeof(UART_TEST_rx_buffer_allocation_0_));
  DS_init_stream_rec_buffer(&UART_TEST_rx_buffer_1_,
                            UART_TEST_rx_buffer_allocation_1_,
                            sizeof(UART_TEST_rx_buffer_allocation_1_));


  ret = UART_TEST_init(&uart_test_instance_, PORT_CH_UART_TEST, &UART_TEST_rx_buffer_0_, &UART_TEST_rx_buffer_1_);
  if (ret != DS_INIT_OK)
  {
    Printf("UART_TEST init Failed! Err:%d \n", ret);
  }

  Printf("######################### \n");
  Printf(" UART_TEST init !!!!!!!! \n");
  Printf("######################### \n");
}


static void UART_TEST_update_(void)
{
  DS_REC_ERR_CODE ret;

  ret = UART_TEST_rec(&uart_test_instance_);

  // TODO: エラー処理
  (void)ret;
}


CCP_CmdRet Cmd_UART_TEST_INIT_DI(const CommonCmdPacket* packet)
{
  (void)packet;

  UART_TEST_init_();

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_UART_TEST_UPDATE(const CommonCmdPacket* packet)
{
  (void)packet;

  UART_TEST_update_();

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_UART_TEST_SEND_TEST(const CommonCmdPacket* packet)
{
  const uint8_t* param = CCP_get_param_head(packet);
  uint8_t id;
  DS_CMD_ERR_CODE ret;

  id = param[0];

  ret = UART_TEST_send(&uart_test_instance_, id);
  return DS_conv_cmd_err_to_ccp_cmd_ret(ret);
}

#pragma section
