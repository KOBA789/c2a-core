#pragma section REPRO
/**
 * @file
 * @brief ���[�h�J�ڂ̐���
 */
#include "mode_manager.h"

#include <string.h>

#include "../TimeManager/time_manager.h"
#include "../TaskManager/task_dispatcher.h"
#include "../AnomalyLogger/anomaly_logger.h"
#include "../EventManager/event_logger.h"
#include "../../TlmCmd/block_command_executor.h"
#include "../../TlmCmd/common_tlm_cmd_packet_util.h"
#include <src_user/TlmCmd/command_definitions.h>
#include "../../TlmCmd/packet_handler.h"
#include "../../Applications/timeline_command_dispatcher.h"
#include "../../Library/endian_memcpy.h"

/**
 * @brief Cmd_MM_START_TRANSITION�̎���
 * @param[in] id: �ڍs��� Mode ID
 * @return MM_ACK
 */
static MM_ACK MM_start_transition_(MD_MODEID id);

/**
 * @brief Cmd_MM_FINISH_TRANSITION�̎���
 * @param void
 * @return MM_ACK
 */
static MM_ACK MM_finish_transition_(void);

/**
 * @brief (���[�h�J�ڂɑΉ�����)�u���b�N�R�}���h�̃^�C�����C���ւ̓W�J���s��
 * @param[in] index: �W�J���� BC ID
 * @return MM_ACK
 */
static void MM_deploy_block_cmd_(bct_id_t index);

/**
 * @brief ���[�h�J�ڃe�[�u�������ׂĖ���`�ɏ���������
 * @return void
 */
static void MM_clear_transition_table_(void);

static ModeManager mode_manager_;
const ModeManager* const mode_manager = &mode_manager_;

void MM_initialize(void)
{
  // �e���[�h�E���[�h�J�ڂɑΉ�����u���b�N�R�}���h�̓ǂݍ���
  MD_load_mode_list();

  MM_clear_transition_table_();
  MD_load_transition_table();

  // �N�����ォ���[�h�J�ڎ��s���ł͂Ȃ��Ƃ��ď�����
  mode_manager_.stat = MM_STATUS_FINISHED;
  mode_manager_.previous_id = MD_MODEID_START_UP;
  mode_manager_.current_id = MD_MODEID_START_UP;

  // �N������Ɉ�x�̂ݎ��s�������R�}���h�͂��̃��[�h�J�ڂŎ��s����
  MM_start_transition_(MD_MODEID_INITIAL);
}

void MM_clear_transition_table_(void)
{
  int from, to;

  // ������ �S�Ă𖢒�`�ɐݒ�B
  for (from = 0; from < MD_MODEID_MODE_MAX; ++from)
  {
    for (to = 0; to < MD_MODEID_MODE_MAX; ++to)
    {
      mode_manager_.transition_table[from][to] = MM_NOT_DEFINED;
    }
  }
}

/**
 * @brief
 * ���[�h�J�ڌ�Ƀ^�X�N���X�g�Ƃ��Ď��s����u���b�N�R�}���h��ݒ肷��R�}���h
 */
CCP_EXEC_STS Cmd_MM_SET_MODE_LIST(const CTCP* packet)
{
  MD_MODEID mode;
  bct_id_t  bc_index;
  const uint8_t* param = CCP_get_param_head(packet);

  if (CCP_get_param_len(packet) != (1 + SIZE_OF_BCT_ID_T))
  {
    // �p�����[�^�̓p�P�b�g�w�b�_��uint8_t 2�imode, index)�B
    return CCP_EXEC_ILLEGAL_LENGTH;
  }

  // �ǂ̃��[�h�ɂǂ̃u���b�N�R�}���h��o�^���邩����������ǂݏo��
  mode = (MD_MODEID)param[0];
  endian_memcpy(&bc_index, param + 1, SIZE_OF_BCT_ID_T);

  mode_manager_.mm_ack = MM_set_mode_list(mode, bc_index);
  if (mode_manager_.mm_ack != MM_SUCCESS)
  {
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }
  return CCP_EXEC_SUCCESS;
}

MM_ACK MM_set_mode_list(MD_MODEID mode, bct_id_t  bc_index)
{
  if (mode >= MD_MODEID_MODE_MAX)
  {
    // ��`����Ă��Ȃ����[�h�ԍ����w�肳�ꂽ�ꍇ
    return MM_BAD_ID;
  }
  else if (bc_index > MM_NOT_DEFINED)
  {
    // �͈͊O�̃u���b�N�R�}���h�ԍ����w�肳�ꂽ�ꍇ
    return MM_BAD_BC_INDEX;
  }
  else if (bc_index != MM_NOT_DEFINED && BCE_is_active(bc_index) != 1)
  {
    // ����������Ă���u���b�N�ԍ����w�肳�ꂽ�ꍇ
    // ��`�Ȃ��p�̒l����ʈ������Ă���B
    return MM_INACTIVE_BLOCK;
  }

  mode_manager_.mode_list[mode] = bc_index;

  return MM_SUCCESS;
}

/**
 * @brief
 * ���[�h�J�ڎ��Ɏ��s����u���b�N�R�}���h��ݒ肷��R�}���h
 */
CCP_EXEC_STS Cmd_MM_SET_TRANSITION_TABLE(const CTCP* packet)
{
  unsigned char from, to;
  bct_id_t bc_index;
  const uint8_t* param = CCP_get_param_head(packet);

  if (CCP_get_param_len(packet) != 1 + 1 + SIZE_OF_BCT_ID_T)
  {
    // �R�}���h�̓p�P�b�g�w�b�_��uint8_t 3�ifrom, to, index)�B
    return CCP_EXEC_ILLEGAL_LENGTH;
  }

  // �ǂ̃��[�h�J�ڂɂǂ̃u���b�N�R�}���h��o�^���邩����������ǂݏo��
  from = param[0];
  to   = param[1];
  endian_memcpy(&bc_index, param + 2, SIZE_OF_BCT_ID_T);

  mode_manager_.mm_ack = MM_set_transition_table((MD_MODEID)from, (MD_MODEID)to, bc_index);
  if (mode_manager_.mm_ack != MM_SUCCESS)
  {
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }
  return CCP_EXEC_SUCCESS;
}

MM_ACK MM_set_transition_table(MD_MODEID from,
    MD_MODEID to,
    bct_id_t  bc_index)
{
  if ((from >= MD_MODEID_MODE_MAX) || (to >= MD_MODEID_MODE_MAX))
  {
    // ��`����Ă��Ȃ����[�h�ԍ����w�肳�ꂽ�ꍇ
    return MM_BAD_ID;
  }
  else if (bc_index > MM_NOT_DEFINED)
  {
    // �͈͊O�̃u���b�N�R�}���h�ԍ����w�肳�ꂽ�ꍇ
    return MM_BAD_BC_INDEX;
  }
  else if (bc_index != MM_NOT_DEFINED && BCE_is_active(bc_index) != 1)
  {
    // ����������Ă���u���b�N�ԍ����w�肳�ꂽ�ꍇ
    // ��`�Ȃ��p�̒l����ʈ������Ă���B
    return MM_INACTIVE_BLOCK;
  }

  mode_manager_.transition_table[from][to] = bc_index;

  return MM_SUCCESS;
}

/**
 * @brief
 * ���[�h�J�ڂ��J�n����R�}���h
 */
CCP_EXEC_STS Cmd_MM_START_TRANSITION(const CTCP* packet)
{
  MD_MODEID id;

  // �ǂ̃��[�h�ɑJ�ڂ��邩����������ǂݏo��
  id = (MD_MODEID)CCP_get_param_head(packet)[0];

  // �G���[����ۑ�
  mode_manager_.mm_ack = MM_start_transition_(id);
  if (mode_manager_.mm_ack != MM_SUCCESS)
  {
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }
  return CCP_EXEC_SUCCESS;
}

static MM_ACK MM_start_transition_(MD_MODEID id)
{
  size_t bc_index = MM_NOT_DEFINED;

  if (id >= MD_MODEID_MODE_MAX)
  {
    // ��`����Ă��Ȃ����[�h�ԍ����w�肳�ꂽ�ꍇ
#ifndef AL_DISALBE_AT_C2A_CORE
    AL_add_anomaly(AL_CORE_GROUP_MODE_MANAGER, MM_BAD_ID);
#endif
    EL_record_event((EL_GROUP)EL_CORE_GROUP_MODE_MANAGER, MM_BAD_ID, EL_ERROR_LEVEL_LOW, (uint32_t)id);
    return MM_BAD_ID;
  }
  else if (mode_manager_.stat != MM_STATUS_FINISHED)
  {
    // �ʂ̃��[�h�J�ڂ����s���̏ꍇ
#ifndef AL_DISALBE_AT_C2A_CORE
    AL_add_anomaly(AL_CORE_GROUP_MODE_MANAGER, MM_OVERWRITE);
#endif
    EL_record_event((EL_GROUP)EL_CORE_GROUP_MODE_MANAGER, MM_OVERWRITE, EL_ERROR_LEVEL_LOW, (uint32_t)mode_manager_.current_id);
    return MM_OVERWRITE;
  }

  // ���s����u���b�N�R�}���h��ǂݍ���ł݂�
  bc_index = mode_manager_.transition_table[mode_manager_.current_id][id];

  if (bc_index == MM_NOT_DEFINED)
  {
    // ���s���������[�h�J�ڂɑΉ�����u���b�N�R�}���h���o�^����Ă��Ȃ��ꍇ
#ifndef AL_DISALBE_AT_C2A_CORE
    AL_add_anomaly(AL_CORE_GROUP_MODE_MANAGER, MM_ILLEGAL_MOVE);
#endif
    EL_record_event((EL_GROUP)EL_CORE_GROUP_MODE_MANAGER, MM_ILLEGAL_MOVE, EL_ERROR_LEVEL_LOW, (uint32_t)bc_index);
    return MM_ILLEGAL_MOVE;
  }

  // �����܂ł���΃��[�h�J�ڂ����s
  TMGR_clear_master_mode_cycle();
  MM_deploy_block_cmd_(mode_manager_.transition_table[mode_manager_.current_id][id]);
  mode_manager_.previous_id = mode_manager_.current_id;
  mode_manager_.current_id = id;
  mode_manager_.stat = MM_STATUS_IN_PROGRESS;

  return MM_SUCCESS;
}

/**
 * @brief
 * ���[�h�J�ڏI���������R�}���h
 * ���[�h�J�ڂ̃u���b�N�R�}���h�̍Ō�ɓ���Ďg��
 * �����Ă��Ȃ��ꍇ�A�^�X�N���X�g���J�ڐ�̃��[�h�ɒu�������Ȃ��̂Œ���
 */
CCP_EXEC_STS Cmd_MM_FINISH_TRANSITION(const CTCP* packet)
{
  (void)packet;

  mode_manager_.mm_ack = MM_finish_transition_();
  if (mode_manager_.mm_ack != MM_SUCCESS)
  {
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }
  return CCP_EXEC_SUCCESS;
}

static MM_ACK MM_finish_transition_(void)
{
  TDSP_ACK ack = TDSP_UNKNOWN;

  if (mode_manager_.stat != MM_STATUS_IN_PROGRESS)
  {
    // ���[�h�J�ڂ����s���łȂ��ꍇ
#ifndef AL_DISALBE_AT_C2A_CORE
    AL_add_anomaly(AL_CORE_GROUP_MODE_MANAGER, MM_NOT_IN_PROGRESS);
#endif
    EL_record_event((EL_GROUP)EL_CORE_GROUP_MODE_MANAGER, MM_NOT_IN_PROGRESS, EL_ERROR_LEVEL_HIGH, (uint32_t)mode_manager_.current_id);
    return MM_NOT_IN_PROGRESS;
  }

  TMGR_clear_master_mode_cycle(); // �����start/finish�����ł��Ă���̂͂Ȃ��H

  // �^�X�N���X�g���A�J�ڌ�̃��[�h�ɓo�^���ꂽ�u���b�N�R�}���h�֍X�V
  ack = TDSP_set_task_list_id(MM_get_tasklist_id_of_mode(mode_manager_.current_id));

  switch (ack)
  {
  case TDSP_SUCCESS:
    break;

  default:
#ifndef AL_DISALBE_AT_C2A_CORE
    AL_add_anomaly(AL_CORE_GROUP_MODE_MANAGER, MM_TL_LOAD_FAILED);
#endif
    EL_record_event((EL_GROUP)EL_CORE_GROUP_MODE_MANAGER, MM_TL_LOAD_FAILED, EL_ERROR_LEVEL_HIGH, (uint32_t)ack);
    break;
  }

  mode_manager_.stat = MM_STATUS_FINISHED;

  return MM_SUCCESS;
}

static void MM_deploy_block_cmd_(bct_id_t bc_index)
{
  CTCP packet;

  CCP_form_block_deploy_cmd(&packet, TL_ID_DEPLOY_BC, bc_index);

  PH_dispatch_command(&packet);
}

CCP_EXEC_STS Cmd_MM_UPDATE_TRANSITION_TABLE_FOR_TLM(const CTCP* packet)
{
  (void)packet;
  MM_update_transition_table_for_tlm();

  return CCP_EXEC_SUCCESS;
}

uint16_t MM_update_transition_table_for_tlm(void)
{
  uint8_t from, to;
  uint16_t idx = 0;

  memset(mode_manager_.transition_table_for_tlm, 0x00, sizeof(mode_manager_.transition_table_for_tlm));

  for (from = 0; from < MD_MODEID_MODE_MAX; ++from)
  {
    for (to = 0; to < MD_MODEID_MODE_MAX; ++to)
    {
      if (mode_manager_.transition_table[from][to] == MM_NOT_DEFINED) continue;

      mode_manager_.transition_table_for_tlm[idx].from = from;
      mode_manager_.transition_table_for_tlm[idx].to = to;
      mode_manager_.transition_table_for_tlm[idx].bc_index = mode_manager_.transition_table[from][to];
      ++idx;
    }
  }

  return idx;
}

bct_id_t MM_get_tasklist_id_of_mode(MD_MODEID mode)
{
  if (mode >= MD_MODEID_MODE_MAX) return 0;
  return mode_manager_.mode_list[mode];
}

#pragma section
