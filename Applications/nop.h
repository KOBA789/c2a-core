/**
 * @file
 * @brief NOP (No Operation) App
 * @note  C2A�� NOP �����邱�Ƃ�O��Ƃ��Ă���
 *        ���� App �͕K�� App �o�^���C Cmd_NOP ���K���R�}���h�o�^���Ă�������
 */
#ifndef NOP_H_
#define NOP_H_

#include "../System/ApplicationManager/app_info.h"
#include "../CmdTlm/common_tlm_cmd_packet.h"

AppInfo NOP_create_app(void);

CCP_EXEC_STS Cmd_NOP(const CTCP* packet);

#endif
