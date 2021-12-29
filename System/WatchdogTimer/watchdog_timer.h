/**
 * @file
 * @brief watchdog timer
 * @note  ������BootLoader�ɔz�u����̂ŁC���v���\�̈�̂��̂�include���Ȃ����ƁI�I
 *        �Ȃ��CCMD�̓��v���Ώۓ��I�I
 */
#ifndef WATCHDOG_TIMER_H_
#define WATCHDOG_TIMER_H_

#include "../../IfWrapper/wdt.h"
#include "../../CmdTlm/common_tlm_cmd_packet.h"

extern const WDT_Config* const wdt_config;

/**
 * @brief  WDT�̏�����
 * @param  void
 * @return void
 * @note   IF_wapper�ł͂Ȃ��C���̊֐���global�Ɍ��J�����
 */
void WDT_init(void);

/**
 * @brief  WDT�̃N���A
 * @param  void
 * @return void
 * @note   IF_wapper�ł͂Ȃ��C���̊֐���global�Ɍ��J�����
 */
void WDT_clear_wdt(void);

// ���S�ăv���ΏۊO
// ���S�ăv���Ώۓ�

CCP_EXEC_STS Cmd_WDT_INIT(const CTCP* packet);
CCP_EXEC_STS Cmd_WDT_ENABLE(const CTCP* packet);
CCP_EXEC_STS Cmd_WDT_DISABLE(const CTCP* packet);
CCP_EXEC_STS Cmd_WDT_STOP_CLEAR(const CTCP* packet);
CCP_EXEC_STS Cmd_WDT_START_CLEAR(const CTCP* packet);

#endif
