#pragma section REPRO
/**
 * @file
 * @brief NOP (No Operation) App
 * @note  C2A�� NOP �����邱�Ƃ�O��Ƃ��Ă���
 *        ���� App �͕K�� App �o�^���C Cmd_NOP ���K���R�}���h�o�^���Ă�������
 */
#include "nop.h"
#include <stddef.h>

static void NOP_nop_(void);

AppInfo NOP_create_app(void)
{
  return AI_create_app_info("nop", NULL, NOP_nop_);
}

static void NOP_nop_() {
  // no operation
}

CCP_EXEC_STS Cmd_NOP(const CTCP* packet)
{
  (void)packet;
  return CCP_EXEC_SUCCESS;
}

#pragma section
