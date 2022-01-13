#pragma section REPRO
#include "command_dispatcher.h"

#include <src_user/TlmCmd/command_definitions.h>
#include "../System/TimeManager/time_manager.h"
#include "packet_handler.h"

static CDIS_EXEC_INFO CEI_init_(void);

CommandDispatcher CDIS_init(PacketList* pli)
{
  CommandDispatcher cdis;

  // �R�}���h���s�����������B
  cdis.prev = CEI_init_();
  cdis.prev_err = CEI_init_();

  // ���s�G���[�J�E���^��0�ɏ������B
  cdis.error_counter = 0;

  // ���s���f�t���O�𖳌��ɐݒ�
  cdis.lockout = 0;

  // �ُ펞���s���f�t���O�𖳌��ɐݒ�B
  cdis.stop_on_error = 0;

  // �����ΏۂƂ���PacketList���N���A���ēo�^�B
  PL_clear_list(pli);
  cdis.pli = pli;

  return cdis;
}

static CDIS_EXEC_INFO CEI_init_(void)
{
  CDIS_EXEC_INFO cei;

  OBCT_clear(&cei.time);
  cei.time.step = 0;            // �����ŏd������Clear���Ă��闝�R�͂Ȃ񂾁D�D�D
  cei.code = (CMD_CODE)0;
  cei.sts = CCP_EXEC_SUCCESS;

  return cei;
}

void CDIS_dispatch_command(CommandDispatcher* cdis)
{
  // �p�P�b�g�R�s�[�p�B�T�C�Y���傫�����ߐÓI�ϐ��Ƃ��Đ錾�B
  static CTCP packet_;

  if (cdis->lockout)
  {
    // ���s�L���t���O������������Ă���ꍇ�͏����ł��؂�B
    return;
  }

  if (PL_is_empty(cdis->pli))
  {
    // ���s���ׂ��R�}���h�������ꍇ�͏����I���B
    return;
  }

  if (cdis->prev.sts != CCP_EXEC_SUCCESS)
  {
    // ���O�R�}���h���ُ�I�������ꍇ�͎��s�O�ɏ���ۑ��B
    // ���s�O�ɃR�s�[���邱�ƂŎ��R�}���h���ُ�I�������ꍇ��
    // prev��prev_err��2�R�}���h���ُ̈����ێ��ł���B
    cdis->prev_err = cdis->prev;
  }

  // ���s���ׂ��R�}���h�p�P�b�g���擾�B
  packet_ = PL_get_head(cdis->pli)->packet;

  // �����Ŏ��s��ʂ�ύX����̂���߂��D
  // - MOBC����z���������OBC�ɂ��CGS cmd��TL cmd�𑗐M����������
  // - user_packet_handler �ł� PH_user_cmd_router �� dispatcher (AOBC_dispatch_command �Ȃ�) �ɂ�
  //   ���OBC���󂯂����R�}���h��ʂւƕϊ�������D
  /*
  // ���s���͑S�ẴR�}���h�̎��s��ʂ�RealTime�ɐݒ肷��B
  // �^�C�����C���͂����Ń��A���^�C���ɕϊ������B
  // ���̏����͓��ɕ����@��Ńp�P�b�g���[�e�B���O���s���ꍇ�d�v�B
  // ���ʂ̓��[�e�B���O��̓��[�e�B���O���̃^�C�����C�����󂯕t���Ȃ��͂��B
  CCP_set_exec_type(&packet_, CCP_EXEC_TYPE_RT);
  */

  // ���s�������L�^���R�}���h�����s�B
  cdis->prev.time = TMGR_get_master_clock();
  cdis->prev.code = CCP_get_id(&packet_);
  cdis->prev.sts  = PH_dispatch_command(&packet_);

  // ���s�����R�}���h�����X�g����j��
  PL_drop_executed(cdis->pli);

  if (cdis->prev.sts != CCP_EXEC_SUCCESS)
  {
    // ���s�����R�}���h�����s�ُ�X�e�[�^�X��Ԃ����ꍇ�B
    // �G���[�����J�E���^���J�E���g�A�b�v�B
    ++(cdis->error_counter);

    if (cdis->stop_on_error == 1)
    {
      // �ُ펞���s���f�t���O���L���ȏꍇ�B
      // ���s���t���O�𖳌������ȍ~�̎��s�𒆒f�B
      cdis->lockout = 1;
    }
  }
}

void CDIS_clear_command_list(CommandDispatcher* cdis)
{
  // �ێ����Ă��郊�X�g�̓��e���N���A
  PL_clear_list(cdis->pli);
}

void CDIS_clear_error_status(CommandDispatcher* cdis)
{
  // ���s�G���[��Ԃ�������Ԃɕ���
  cdis->prev_err = CEI_init_();

  // �ώZ�G���[�񐔂�0�N���A
  cdis->error_counter = 0;
}
#pragma section
