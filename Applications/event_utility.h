/**
 * @file
 * @brief �C�x���g�����̒�����s�̂��߂�App
 * @note  �����I�ɂ� event_handler �������s���邾��
 */
#ifndef EVENT_UTILITY_H_
#define EVENT_UTILITY_H_

#include "../System/ApplicationManager/app_info.h"
#include "../CmdTlm/common_tlm_cmd_packet.h"

AppInfo EVENT_UTIL_create_app(void);

/**
 * @struct EventUtility
 * @brief  EventUtility �� AppInfo �\����
 */
typedef struct
{
  uint8_t is_enabled_eh_execution;    //!< EH �̎��s���L�����H
} EventUtility;

extern const EventUtility* const event_utility;

CCP_EXEC_STS Cmd_EVENT_UTIL_ENABLE_EH_EXEC(const CTCP* packet);
CCP_EXEC_STS Cmd_EVENT_UTIL_DISABLE_EH_EXEC(const CTCP* packet);
CCP_EXEC_STS Cmd_EVENT_UTIL_EXEC_EH(const CTCP* packet);

#endif
