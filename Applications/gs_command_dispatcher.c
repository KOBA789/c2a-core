#pragma section REPRO
// �n��ǂ���̃R�}���h���s�p�A�v��

#include "gs_command_dispatcher.h"

#include "../CmdTlm/packet_handler.h"

static CommandDispatcher gs_command_dispatcher_;
const CommandDispatcher* const gs_command_dispatcher = &gs_command_dispatcher_;

static void GSCD_init_(void);
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
  // ����gs_command_dispatcher_�͊O���ōX�V����Ă���
  // 2018/06/26 �̎��_�ɂ����鏈���̗������̓I�ɕ`���ƁA
  // GSTOS_cmd_packet_handler_ > PH_analyze_packet > analyze_cmd_ > add_gs_cmd_
  //
  // AR_DI_GSTOS_CMD_PH �Ƃ���ID�̃A�v�����N�_�ɃR�}���h�̉�͂��i�݁A
  // �n��ǂ���̃R�}���h�Ɣ��肳����add_gs_cmd_�֐�����gs_command_dispatcher_�ɔ��f�����B

  CDIS_dispatch_command(&gs_command_dispatcher_);
}

CCP_EXEC_STS Cmd_GSCD_CLEAR_ERR_LOG(const CTCP* packet)
{
  (void)packet;

  // �L�^���ꂽ�G���[���������B
  CDIS_clear_error_status(&gs_command_dispatcher_);
  return CCP_EXEC_SUCCESS;
}
#pragma section
