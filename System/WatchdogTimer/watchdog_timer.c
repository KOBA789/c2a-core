/**
 * @file  watchdog_timer.c
 * @brief watchdog timer
 * @note  ������BootLoader�ɔz�u����̂ŁC���v���\�̈�̂��̂�include���Ȃ����ƁI�I
 *        �Ȃ��CCMD�̓��v���Ώۓ��I�I
 */
#include "watchdog_timer.h"
#include "../../Library/print.h"

static WDT_Config wdt_config_;
const WDT_Config* const wdt_config = &wdt_config_;

static void wdt_init_(void);

// WDT�������̌��J�֐�
// BootLoader����͂��ꂪ�Ă΂��
void WDT_init(void)
{
  wdt_init_();
}

static void wdt_init_(void)
{
  int ret;
  ret = WDT_initialize((void*)&wdt_config_);
  if (ret != 0)
  {
    Printf("WDT init Error (%d)\n", ret);
  }
}

void WDT_clear_wdt(void)
{
  // IF wrapper �̂��̂����
  WDT_clear((void*)&wdt_config_);
}


// ###########################################################
// ###########################################################
// ###########################################################
#pragma section REPRO
// �������烊�v���Ώۓ��I�I


CCP_EXEC_STS Cmd_WDT_INIT(const CTCP* packet)
{
  (void)packet;

  wdt_init_();
  return CCP_EXEC_SUCCESS;
}

CCP_EXEC_STS Cmd_WDT_ENABLE(const CTCP* packet)
{
  int ret;
  (void)packet;

  ret = WDT_enable((void*)&wdt_config_);
  wdt_config_.is_wdt_enable = 1;

  if (ret != 0)
  {
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }

  return CCP_EXEC_SUCCESS;
}

CCP_EXEC_STS Cmd_WDT_DISABLE(const CTCP* packet)
{
  int ret;
  (void)packet;

  ret =  WDT_disable((void*)&wdt_config_);
  wdt_config_.is_wdt_enable = 0;

  if (ret != 0)
  {
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }

  return CCP_EXEC_SUCCESS;
}

CCP_EXEC_STS Cmd_WDT_STOP_CLEAR(const CTCP* packet)
{
  (void)packet;

  wdt_config_.is_clear_enable = 0;

  return CCP_EXEC_SUCCESS;
}

CCP_EXEC_STS Cmd_WDT_START_CLEAR(const CTCP* packet)
{
  (void)packet;

  wdt_config_.is_clear_enable = 1;

  return CCP_EXEC_SUCCESS;
}

#pragma section
// ###
// !!!!! this line should not delete!!!
// ###
