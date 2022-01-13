#pragma section REPRO
/**
 * @file
 * @brief �n��ǂ��甭�s���ꂽ RTC (Real Time Cmd) �����s����
 */

#include "gs_command_dispatcher.h"
#include "../TlmCmd/packet_handler.h"

static CommandDispatcher gs_command_dispatcher_;
const CommandDispatcher* const gs_command_dispatcher = &gs_command_dispatcher_;

/**
 * @brief  GSCD App �������֐�
 * @param  void
 * @return void
 */
static void GSCD_init_(void);

/**
 * @brief  GSCD App ���s�֐�
 *
 *         PH_add_gs_cmd_ �ɂāC GS ����� RTC �� gs_command_dispatcher �ɕR�t����ꂽ�R�}���h�L���[ PH_gs_cmd_list �� push back �����D
 *         ���̃L���[����P�R�}���h�����o�����s����
 * @param  void
 * @return void
 */
static void GSCD_dispatch_(void);


AppInfo GSCD_create_app(void)
{
  return AI_create_app_info("gs_command_dispatcher", GSCD_init_, GSCD_dispatch_);
}

static void GSCD_init_(void)
{
  gs_command_dispatcher_ = CDIS_init(&PH_gs_cmd_list);
}

static void GSCD_dispatch_(void)
{
  CDIS_dispatch_command(&gs_command_dispatcher_);
}

CCP_EXEC_STS Cmd_GSCD_CLEAR_ERR_LOG(const CTCP* packet)
{
  (void)packet;

  // �L�^���ꂽ�G���[�����N���A
  CDIS_clear_error_status(&gs_command_dispatcher_);
  return CCP_EXEC_SUCCESS;
}

#pragma section
