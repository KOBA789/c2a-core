#pragma section REPRO
#include "task_dispatcher.h"

#include <stdlib.h>
#include <string.h> // for memcpy

#include "../../CmdTlm/packet_list.h"
#include "../../CmdTlm/block_command_executor.h"
#include "../ModeManager/mode_manager.h"
#include "../TimeManager/time_manager.h"
#include "../AnomalyLogger/anomaly_logger.h"
#include "../EventManager/event_logger.h"
#include <src_user/CmdTlm/block_command_definitions.h>
#include <src_user/CmdTlm/command_definitions.h>
#include "../../Library/print.h"
#include <src_user/Library/VT100.h>

static TDSP_Info TDSP_info_;
const TDSP_Info* const TDSP_info = &TDSP_info_;

// ���݂̃T�C�N���Ŏ��s���ׂ��^�X�N�ꗗ��ۑ���������
static PacketList task_list_;

/**
 * @brief `TDSP_info_.task_list_id` �ɓo�^���ꂽ BC �� TaskList�ɓW�J.
 * @note  ���ۂ� BC �̓��e��ǂݍ��݁A�W�J���Ă���̂� `PL_ACK PL_deploy_block_cmd(args)`
 */
static void TDSP_deploy_block_as_task_list_(void);

/**
 * @brief �f�o�b�O���\��
 */
static void print_tdsp_status_(void);

void TDSP_initialize(void)
{
  static PL_Node task_stock_[TDSP_TASK_MAX];
  PL_initialize(task_stock_, TDSP_TASK_MAX, &task_list_);

  // �^�X�N���X�g�����������AINITIAL���[�h�̃u���b�N�R�}���h��W�J����
  TDSP_info_.tskd = CDIS_init(&task_list_);
  TDSP_info_.task_list_id = MM_get_tasklist_id_of_mode(MD_MODEID_START_UP);
  TDSP_deploy_block_as_task_list_();
  TDSP_info_.activated_at = 0;
}

TDSP_ACK TDSP_set_task_list_id(bct_id_t id)
{
  if (id >= BCT_MAX_BLOCKS) return TDSP_INVAILD_BCT_ID;
  if (!BCE_is_active(id)) return TDSP_INACTIVE_BCT_ID;
  if (BCT_get_bc_length(id) == 0) return TDSP_EMPTY_BC;

  TDSP_info_.task_list_id = id;
  return TDSP_SUCCESS;
}

static void TDSP_deploy_block_as_task_list_(void)
{
  PL_ACK ack;

  // �{�֐����̏�������Master Cycle���ω������ꍇ�����o�ł���悤�A
  // �܂���Master Cycle�̏����X�V����B
  TDSP_info_.activated_at = TMGR_get_master_total_cycle() + 1;

  ack = PL_deploy_block_cmd(&task_list_, TDSP_info_.task_list_id, 0);

  if (ack != PL_SUCCESS)
  {
#ifndef AL_DISALBE_AT_C2A_CORE
    AL_add_anomaly(AL_CORE_GROUP_TASK_DISPATCHER, TDSP_DEPLOY_FAILED);
#endif
    EL_record_event((EL_GROUP)EL_CORE_GROUP_TASK_DISPATCHER,
                    TDSP_DEPLOY_FAILED,
                    EL_ERROR_LEVEL_HIGH,
                    (uint32_t)ack);
  }
}

void TDSP_execute_pl_as_task_list(void)
{
  // �܂��� (1)�u���b�N�R�}���h���^�X�N���X�g�ɓW�J���ꂽ���̃T�C�N���� �� (2)���݂̃T�C�N���� ���r
  // (1) = (2) �̎��͓W�J���� 1�T�C�N�� (100ms) �ȓ��Ȃ̂Ń^�X�N���X�g�ɓo�^���ꂽ�R�}���h������s����
  // (1) > (2) �̎��͍���̃T�C�N���Ŏ��s���ׂ��R�}���h�����ׂĎ��s�ς݂ł���
  // (1) < (2) �̎��͓W�J���� 1�T�C�N�� (100ms) �ȏ�o�߂��Ă��܂��Ă���̂ŋ����I������

  if (TDSP_info_.activated_at == TMGR_get_master_total_cycle())
  {
    // �^�X�N���X�g�̐擪�R�}���h���s�\�莞���ƌ��ݎ������r
    PL_ACK ack = PL_check_tl_cmd(&task_list_,
                                 (size_t)(TMGR_get_master_step()));

    switch (ack)
    {
    case PL_TLC_PAST_TIME:
      // ���s�������߂��Ă����ꍇ�͎��s�O�ɃA�m�}����o�^�B
#ifndef AL_DISALBE_AT_C2A_CORE
      AL_add_anomaly(AL_CORE_GROUP_TASK_DISPATCHER, TDSP_STEP_OVERRUN);
#endif
      EL_record_event((EL_GROUP)EL_CORE_GROUP_TASK_DISPATCHER,
                      TDSP_STEP_OVERRUN,
                      EL_ERROR_LEVEL_LOW,
                      (uint32_t)CCP_get_ti(&PL_get_head(&task_list_)->packet));

      // FALL THROUGH

    case PL_TLC_ON_TIME:
      // ���s�������߂��Ă���A�������͎��s�����s�b�^���̏ꍇ�̓R�}���h�����s
      CDIS_dispatch_command(&(TDSP_info_.tskd));

      if (TDSP_info_.tskd.prev.sts != CCP_EXEC_SUCCESS)
      {
        // �R�}���h���s���Ɉُ킪���������ꍇ�̓A�m�}����o�^�B
#ifndef AL_DISALBE_AT_C2A_CORE
        AL_add_anomaly(AL_CORE_GROUP_TASK_DISPATCHER, TDSP_TASK_EXEC_FAILED);
#endif
        EL_record_event((EL_GROUP)EL_CORE_GROUP_TASK_DISPATCHER,
                      TDSP_TASK_EXEC_FAILED,
                      EL_ERROR_LEVEL_HIGH,
                      (uint32_t)(TDSP_info_.tskd.prev.sts + 100));   // FIXME: CCP_EXEC_STS ���������܂ނ̂ŁD�D�D�Ȃ�Ƃ�������
      }

      break;

    case PL_TLC_NOT_YET:
      // case�����s�K�؂����A�����ɗ���͈̂ȉ��̓�̏ꍇ
      // �E�^�X�N���X�g����
      // �E���̃R�}���h�̎��s�������܂�

      if (PL_count_active_nodes(&task_list_) == 0)
      {
        // task_list����Ȃ�ēx�^�X�N���X�g��W�J
        // ������s����TDSP_info_.activated_at���C���N�������g�����̂ŁA"���̃T�C�N�����s�҂����"�ɂȂ�
        TDSP_deploy_block_as_task_list_();
      }

      break;

    default:
      // ��{�����ɂ͗��Ȃ�
#ifndef AL_DISALBE_AT_C2A_CORE
      AL_add_anomaly(AL_CORE_GROUP_TASK_DISPATCHER, TDSP_UNKNOWN);
#endif
      EL_record_event((EL_GROUP)EL_CORE_GROUP_TASK_DISPATCHER,
                      TDSP_UNKNOWN,
                      EL_ERROR_LEVEL_HIGH,
                      (uint32_t)ack);
    }
  }
  else if (TDSP_info_.activated_at > TMGR_get_master_total_cycle())
  {
    // ���̃T�C�N���̎��s�҂����
    return;
  }
  else if (TDSP_info_.activated_at < TMGR_get_master_total_cycle())
  {
    if ((TDSP_info_.activated_at == 0) && (TMGR_get_master_total_cycle() == OBCT_MAX_CYCLE - 1))
    {
      // ���T�C�N���̎��s�҂����(�T�C�N���I�[�o�[�t���[���O)
      // �{���� TDSP_info_.activated_at > TMGR_get_master_total_cycle() �ƂȂ�͂����A
      // TDSP_info_.activated_at�����ӂ��0�ɖ߂��Ă���ꍇ�����ɗ���
      return;
    }
    else
    {
      // 1�T�C�N���ȓ��ɑS�ẴR�}���h�����s���I���Ȃ������ꍇ�����ɗ���
#ifndef AL_DISALBE_AT_C2A_CORE
      AL_add_anomaly(AL_CORE_GROUP_TASK_DISPATCHER, TDSP_CYCLE_OVERRUN);
#endif
      EL_record_event((EL_GROUP)EL_CORE_GROUP_TASK_DISPATCHER,
                      TDSP_CYCLE_OVERRUN,
                      EL_ERROR_LEVEL_HIGH,
                      0);

      // ���X�g���N���A->�ēW�J�����T�C�N������Ď��s
      PL_clear_list(&task_list_);
      TDSP_deploy_block_as_task_list_();
    }
  }
}

void TDSP_resync_internal_counter(void)
{
  TDSP_info_.activated_at = TMGR_get_master_total_cycle();
}

CCP_EXEC_STS Cmd_TDSP_SET_TASK_LIST(const CTCP* packet)
{
  // FIXME: u8 �ł����̂��H �܂��C�����C������D
  TDSP_ACK ack = TDSP_set_task_list_id((bct_id_t)(CCP_get_param_head(packet)[0]));

  switch (ack)
  {
  case TDSP_SUCCESS:
    return CCP_EXEC_SUCCESS;
  case TDSP_INVAILD_BCT_ID:
    return CCP_EXEC_ILLEGAL_PARAMETER;
  case TDSP_INACTIVE_BCT_ID:
  case TDSP_EMPTY_BC:
  default:
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }
}

AppInfo print_tdsp_status(void)
{
  return AI_create_app_info("tstm", NULL, print_tdsp_status_);
}

void print_tdsp_status_(void)
{
  VT100_erase_line();
  Printf("TASK: BC %d, ERR (TOTAL, STEP, STS) = (%10u, %3u, %d)\n",
         TDSP_info->task_list_id,
         TDSP_info->tskd.prev_err.time.total_cycle,
         TDSP_info->tskd.prev_err.time.step,
         TDSP_info->tskd.prev_err.sts);
}

#pragma section
