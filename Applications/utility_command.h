#ifndef UTILITY_COMMAND_H_
#define UTILITY_COMMAND_H_
// FIXME: ����App�͌��݊Ǘ�����Ă��Ȃ��I
//        �g�p����O�Ɋm�F���邱��

#include "../CmdTlm/common_tlm_cmd_packet.h"
#include "../IfWrapper/uart.h"
#include "../System/ApplicationManager/app_info.h"

#define UTIL_CMD_SIZE_MAX (256) // 256Bytes�ȏ�R�}���h�Ƃ��đ��邱�Ƃ͂Ȃ��͂��D�D�DICOUPS_PARAM_BL��蒷����

// �ėp�R�}���h�p�\����
typedef struct
{
  UART_Config uart_config_dummy;
  unsigned char util_cmd_buffer[UTIL_CMD_SIZE_MAX];
  unsigned int cmd_size; // ���M�R�}���h�T�C�Y
  unsigned int pointer;
  int uart_err_code; // UTIL_CMD_send_�ł̃G���[���ێ��p
} UtilityCommand;

extern const UtilityCommand* const utility_command;

AppInfo UTIL_CMD_create_app(void);

// �R�}���h
CCP_EXEC_STS Cmd_UTIL_CMD_ADD(const CTCP* packet);
CCP_EXEC_STS Cmd_UTIL_CMD_SEND(const CTCP* packet);
CCP_EXEC_STS Cmd_UTIL_CMD_RESET(const CTCP* packet);

#endif
