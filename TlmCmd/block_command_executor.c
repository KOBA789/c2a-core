#pragma section REPRO
/**
 * @file
 * @brief BCT�̎��s����̊֐�, �p�����[�^�[
 * @note  Block Command ExeInfo��
 *        Block Command Table (BCT) �̎��s���̃p�����[�^�[�ł���.
 *        ���� Cmd �̃f�[�^�ۑ����Ǝ��s���͕�������Ă��炸, BCT �̂��̎��s��Ԃ͂����ɕۑ������.
 *        ����Ď��s�p�����[�^�[�� BCT �Ɠ������� (BCT_MAX_BLOCKS) �m�ۂ����.
 */

#include <string.h>

#include "block_command_executor.h"
#include "packet_handler.h"
#include "../Library/endian_memcpy.h"
#include "../System/TimeManager/time_manager.h"
#include "common_tlm_cmd_packet_util.h"

static CTCP packet_;

static BlockCommandExecutor block_command_executor_;
const BlockCommandExecutor* const block_command_executor = &block_command_executor_;

// default (SRAM, �璷����) �� getter, setter
static BCE_Params* BCE_get_bc_exe_params_default_(const bct_id_t block);
static void BCE_set_bc_exe_params_default_(const bct_id_t block, const BCE_Params* bc_exe_params);

// BlockCmdExeFunc �����b�v���� static getter, setter
// static�ł���̂ŁCconst �����Ă��Ȃ�
// user��������ł́C�O�d�璷�����ꂽ�s�������������BCT�������\�������邽�߁C
// �擾�����|�C���^����Ēl���X�V�����ꍇ�Csetter���Ăяo���D
// block �̈����A�T�[�V�����͂��Ă��Ȃ�
static BCE_Params* BCE_get_bc_exe_params_(const bct_id_t block);
static void BCE_set_bc_exe_params_(const bct_id_t block, const BCE_Params* bc_exe_params);

/**
 * @brief rotator �̎��s���
 * @param[in] block: BC �� idx
 * @return CCP_EXEC_STS
 * @note  rotator �͂Ђ����炻�� BC �Ɋ܂܂�� Cmd �����[�v�Ŏ��s��������
 *        interval[cycle] ���Ƃ� 1�� Cmd �����s�����.
 */
static CCP_EXEC_STS BCT_rotate_block_cmd_(bct_id_t block);

/**
 * @brief BC ���܂Ƃ߂Ĉꊇ�Ŏ��s����
 * @param[in] block: BC �� idx
 * @return CCP_EXEC_STS
 * @note  BC �̓����� BC �����s���鎞�Ȃ�
 */
static CCP_EXEC_STS BCT_combine_block_cmd_(bct_id_t block);

/**
 * @brief BC ���܂Ƃ߂Ĉꊇ�Ŏ��s����
 * @param[in] block: BC �� idx
 * @param[in] limit_step: ���s�������� [step]
 * @return CCP_EXEC_STS
 * @note ���Ԃ𐧌���݂���BC�����s���������Ȃ�
 */
static CCP_EXEC_STS BCT_timelimit_combine_block_cmd_(bct_id_t block, step_t limit_step);

/**
 * @brief ���Ԑ����t���� combiner
 * @param[in] block: BC �� idx
 * @return BCE_Params*
 */
static BCE_Params* BCE_get_bc_exe_params_default_(const bct_id_t block)
{
  return block_command_executor_.bc_exe_params[block];
}

static void BCE_set_bc_exe_params_default_(const bct_id_t block, const BCE_Params* bc_exe_params)
{
  if ((unsigned char*)block_command_executor_.bc_exe_params[block] == (const unsigned char*)bc_exe_params) return;
  memcpy(block_command_executor_.bc_exe_params[block], bc_exe_params, sizeof(BCE_Params));
}

static void BCE_set_bc_exe_params_(const bct_id_t block, const BCE_Params* bc_exe_params)
{
  block_command_executor_.bc_exe_func[block].set_bc_exe_params_(block, bc_exe_params);
}

static BCE_Params* BCE_get_bc_exe_params_(const bct_id_t block)
{
  return block_command_executor_.bc_exe_func[block].get_bc_exe_params_(block);
}

const BCE_Params* BCE_get_bc_exe_params(const bct_id_t block)
{
  return block_command_executor_.bc_exe_func[block].get_bc_exe_params_(block);
}

void BCE_load_default_func_pointer(void)
{
  bct_id_t block;
  for (block = 0; block < BCT_MAX_BLOCKS; ++block)
  {
    block_command_executor_.bc_exe_func[block].get_bc_exe_params_ = BCE_get_bc_exe_params_default_;
    block_command_executor_.bc_exe_func[block].set_bc_exe_params_ = BCE_set_bc_exe_params_default_;
  }
}

BCT_ACK BCE_clear_block(const bct_id_t block)
{
  BCE_Params* bc_exe_params;
  if (block >= BCT_MAX_BLOCKS) return BCT_INVALID_BLOCK_NO;

  bc_exe_params = BCE_get_bc_exe_params_(block);
  bc_exe_params->is_active = 0;
  BCE_set_bc_exe_params_(block, bc_exe_params);

  BCE_reset_rotator_info(block);
  BCE_reset_combiner_info(block);

  return BCT_SUCCESS;
}

CCP_EXEC_STS Cmd_BCT_ACTIVATE_BLOCK(const CTCP* packet)
{
  BCT_ACK ack;
  (void)packet;

  ack = BCE_activate_block();

  return BCT_convert_bct_ack_to_ctcp_exec_sts(ack);
}

BCT_ACK BCE_activate_block(void)
{
  bct_id_t block = block_command_table->pos.block;
  BCE_Params* bc_exe_params;

  if (block >= BCT_MAX_BLOCKS) return BCT_INVALID_BLOCK_NO;

  bc_exe_params = BCE_get_bc_exe_params_(block);

  if (BCT_get_bc_length(block) != block_command_table->pos.cmd)
  {
    return BCT_DEFECTIVE_BLOCK;
  }

  bc_exe_params->is_active = 1;

  BCE_set_bc_exe_params_(block, bc_exe_params);

  return BCT_SUCCESS;
}

CCP_EXEC_STS Cmd_BCT_ACTIVATE_BLOCK_BY_ID(const CTCP* packet)
{
  bct_id_t block;
  BCT_ACK ack;

  if (CCP_get_param_len(packet) != SIZE_OF_BCT_ID_T)
  {
    // �p�����[�^�̓u���b�N�ԍ�
    return CCP_EXEC_ILLEGAL_LENGTH;
  }

  endian_memcpy(&block, CCP_get_param_head(packet), SIZE_OF_BCT_ID_T);
  ack = BCE_activate_block_by_id(block);

  return BCT_convert_bct_ack_to_ctcp_exec_sts(ack);
}

CCP_EXEC_STS Cmd_BCT_INACTIVATE_BLOCK_BY_ID(const CTCP* packet)
{
  bct_id_t block;
  BCT_ACK ack;

  if (CCP_get_param_len(packet) != SIZE_OF_BCT_ID_T)
  {
    // �p�����[�^�̓u���b�N�ԍ�
    return CCP_EXEC_ILLEGAL_LENGTH;
  }

  endian_memcpy(&block, CCP_get_param_head(packet), SIZE_OF_BCT_ID_T);

  ack = BCE_inactivate_block_by_id(block);

  return BCT_convert_bct_ack_to_ctcp_exec_sts(ack);
}

BCT_ACK BCE_activate_block_by_id(bct_id_t block)
{
  BCE_Params* bc_exe_params;

  if (block < 0 || block >= BCT_MAX_BLOCKS) return BCT_INVALID_BLOCK_NO;

  bc_exe_params = BCE_get_bc_exe_params_(block);
  bc_exe_params->is_active = 1;
  BCE_set_bc_exe_params_(block, bc_exe_params);

  return BCT_SUCCESS;
}

BCT_ACK BCE_inactivate_block_by_id(bct_id_t block)
{
  BCE_Params* bc_exe_params;

  if (block < 0 || block >= BCT_MAX_BLOCKS) return BCT_INVALID_BLOCK_NO;

  bc_exe_params = BCE_get_bc_exe_params_(block);
  bc_exe_params->is_active = 0;
  BCE_set_bc_exe_params_(block, bc_exe_params);

  return BCT_SUCCESS;
}

CCP_EXEC_STS Cmd_BCT_ROTATE_BLOCK(const CTCP* packet)
{
  bct_id_t block;

  if (CCP_get_param_len(packet) != SIZE_OF_BCT_ID_T)
  {
    // �p�����[�^�̓u���b�N�ԍ�
    return CCP_EXEC_ILLEGAL_LENGTH;
  }

  // �p�����[�^��ǂݏo���B
  endian_memcpy(&block, CCP_get_param_head(packet), SIZE_OF_BCT_ID_T);

  return BCT_rotate_block_cmd_(block);
}

static CCP_EXEC_STS BCT_rotate_block_cmd_(bct_id_t block)
{
  CCP_EXEC_STS ack;
  BCE_Params* bc_exe_params;
  BCT_Pos pos;

  if (block >= BCT_MAX_BLOCKS) return BCT_convert_bct_ack_to_ctcp_exec_sts(BCT_INVALID_BLOCK_NO);

  bc_exe_params = BCE_get_bc_exe_params_(block);
  if (!bc_exe_params->is_active) return CCP_EXEC_ILLEGAL_CONTEXT;
  if (bc_exe_params->rotate.interval == 0) return CCP_EXEC_ILLEGAL_CONTEXT;

  ++bc_exe_params->rotate.counter;
  if (bc_exe_params->rotate.counter < bc_exe_params->rotate.interval)
  {
    BCE_set_bc_exe_params_(block, bc_exe_params);
    return CCP_EXEC_SUCCESS; // �X�L�b�v
  }

  bc_exe_params->rotate.counter = 0;

  ++bc_exe_params->rotate.next_cmd;
  bc_exe_params->rotate.next_cmd %= BCT_get_bc_length(block);

  BCE_set_bc_exe_params_(block, bc_exe_params);

  BCT_make_pos(&pos, block, bc_exe_params->rotate.next_cmd);
  BCT_load_cmd(&pos, &packet_);
  ack = PH_dispatch_command(&packet_);

  return ack;
}

CCP_EXEC_STS Cmd_BCT_COMBINE_BLOCK(const CTCP* packet)
{
  bct_id_t block;

  if (CCP_get_param_len(packet) != SIZE_OF_BCT_ID_T)
  {
    // �p�����[�^�̓u���b�N�ԍ�
    return CCP_EXEC_ILLEGAL_LENGTH;
  }

  // �p�����[�^��ǂݏo���B
  endian_memcpy(&block, CCP_get_param_head(packet), SIZE_OF_BCT_ID_T);

  return BCT_combine_block_cmd_(block);
}

static CCP_EXEC_STS BCT_combine_block_cmd_(bct_id_t block)
{
  uint8_t cmd;
  CCP_EXEC_STS ack;
  uint8_t length;

  if (block >= BCT_MAX_BLOCKS) return BCT_convert_bct_ack_to_ctcp_exec_sts(BCT_INVALID_BLOCK_NO);

  length = BCT_get_bc_length(block);

  if (!BCE_is_active(block)) return CCP_EXEC_ILLEGAL_CONTEXT;

  for (cmd = 0; cmd < length; ++cmd)
  {
    BCT_Pos pos;
    pos.block = block;
    pos.cmd = cmd;
    BCT_load_cmd(&pos, &packet_);
    ack = PH_dispatch_command(&packet_);

    if (ack != CCP_EXEC_SUCCESS) return ack;
  }

  return CCP_EXEC_SUCCESS;
}

// 2019/10/01 �ǉ�
// ���Ԑ����t���R���o�C�i
// �i���Ԃ�������ł��؂�D���������āC�K���ݒ莞�Ԃ͂�����j
CCP_EXEC_STS Cmd_BCT_TIMELIMIT_COMBINE_BLOCK(const CTCP* packet)
{
  const uint8_t* param = CCP_get_param_head(packet);
  bct_id_t block;
  step_t  limit_step;

  if (CCP_get_param_len(packet) != SIZE_OF_BCT_ID_T + 1)
  {
    // �p�����[�^�̓u���b�N�ԍ� + �������� [step]
    return CCP_EXEC_ILLEGAL_LENGTH;
  }

  // �p�����[�^��ǂݏo���B
  endian_memcpy(&block, param, SIZE_OF_BCT_ID_T);
  limit_step = param[SIZE_OF_BCT_ID_T];

  return BCT_timelimit_combine_block_cmd_(block, limit_step);
}

static CCP_EXEC_STS BCT_timelimit_combine_block_cmd_(bct_id_t block, step_t limit_step)
{
  uint8_t cmd;
  uint8_t length;
  CCP_EXEC_STS ack;
  BCE_Params* bc_exe_params;

  ObcTime start = TMGR_get_master_clock();
  ObcTime finish;
  step_t diff;

  if (block >= BCT_MAX_BLOCKS) return BCT_convert_bct_ack_to_ctcp_exec_sts(BCT_INVALID_BLOCK_NO);

  bc_exe_params = BCE_get_bc_exe_params_(block);
  if (!bc_exe_params->is_active) return CCP_EXEC_ILLEGAL_CONTEXT;

  ++bc_exe_params->timelimit_combine.call_num;
  length = BCT_get_bc_length(block);

  // ��������Ƃ����������...
  // �l�������Ȃ��悤�ɁC�������ς��Ȃ��悤�ɓK���ɉ����Ă銴��
  if (bc_exe_params->timelimit_combine.call_num >= 0xFFFF - 16)
  {
    bc_exe_params->timelimit_combine.call_num /= 8;
    bc_exe_params->timelimit_combine.timeover_num /= 8;
  }

  for (cmd = 0; cmd < length; ++cmd)
  {
    BCT_Pos pos;
    pos.block = block;
    pos.cmd = cmd;
    BCT_load_cmd(&pos, &packet_);
    ack = PH_dispatch_command(&packet_);
    if (ack != CCP_EXEC_SUCCESS)
    {
      BCE_set_bc_exe_params_(block, bc_exe_params);
      return ack;
    }

    // ���Ԕ���
    finish = TMGR_get_master_clock();
    diff = OBCT_diff_in_step(&start, &finish);
    if (diff >= limit_step)
    {
      // �r���Œ��f
      ++bc_exe_params->timelimit_combine.timeover_num;
      bc_exe_params->timelimit_combine.last_timeover_cmd_pos = cmd;
      if (bc_exe_params->timelimit_combine.last_timeover_cmd_pos < bc_exe_params->timelimit_combine.worst_cmd_pos)
      {
        bc_exe_params->timelimit_combine.worst_cmd_pos = bc_exe_params->timelimit_combine.last_timeover_cmd_pos;
      }

      BCE_set_bc_exe_params_(block, bc_exe_params);
      return CCP_EXEC_SUCCESS;    // �ُ�ł͂Ȃ��̂ł����Ԃ�
    }
  }

  BCE_set_bc_exe_params_(block, bc_exe_params);

  // �Ō�܂Ŏ��s�ł���
  return CCP_EXEC_SUCCESS;
}

BCT_ACK BCE_reset_rotator_info(const bct_id_t block)
{
  BCE_Params* bc_exe_params;
  if (block >= BCT_MAX_BLOCKS) return BCT_INVALID_BLOCK_NO;

  bc_exe_params = BCE_get_bc_exe_params_(block);

  bc_exe_params->rotate.counter = 0;
  bc_exe_params->rotate.interval = 1;
  bc_exe_params->rotate.next_cmd = 0;

  BCE_set_bc_exe_params_(block, bc_exe_params);

  return BCT_SUCCESS;
}

BCT_ACK BCE_reset_combiner_info(const bct_id_t block)
{
  BCE_Params* bc_exe_params;
  if (block >= BCT_MAX_BLOCKS) return BCT_INVALID_BLOCK_NO;

  bc_exe_params = BCE_get_bc_exe_params_(block);

  bc_exe_params->timelimit_combine.call_num = 0;
  bc_exe_params->timelimit_combine.timeover_num = 0;
  bc_exe_params->timelimit_combine.last_timeover_cmd_pos = 0xFF;
  bc_exe_params->timelimit_combine.worst_cmd_pos = 0xFF;

  BCE_set_bc_exe_params_(block, bc_exe_params);

  return BCT_SUCCESS;
}

uint8_t BCE_is_active(const bct_id_t block)
{
  if (block >= BCT_MAX_BLOCKS) return 0;
  return (uint8_t)BCE_get_bc_exe_params(block)->is_active;
}

BCT_ACK BCE_swap_address(const bct_id_t block_a, const bct_id_t block_b)
{
  BCE_Func tmp_bce_func;
  BCE_Params* tmp_params_pointer;

  if (block_a >= BCT_MAX_BLOCKS || block_b >= BCT_MAX_BLOCKS) return BCT_INVALID_BLOCK_NO;
  if (BCE_is_active(block_a) || BCE_is_active(block_b)) return BCT_DEFECTIVE_BLOCK;

  tmp_params_pointer = block_command_executor_.bc_exe_params[block_a];
  block_command_executor_.bc_exe_params[block_a] = block_command_executor_.bc_exe_params[block_b];
  block_command_executor_.bc_exe_params[block_b] = tmp_params_pointer;

  memcpy(&tmp_bce_func, &block_command_executor_.bc_exe_func[block_a], sizeof(BCE_Func));
  memcpy(&block_command_executor_.bc_exe_func[block_a], &block_command_executor_.bc_exe_func[block_b], sizeof(BCE_Func));
  memcpy(&block_command_executor_.bc_exe_func[block_b], &tmp_bce_func, sizeof(BCE_Func));

  return BCT_SUCCESS;
}

BCT_ACK BCE_swap_contents(const bct_id_t block_a, const bct_id_t block_b)
{
  BCE_Params* params_a;
  BCE_Params* params_b;
  BCE_Params  tmp_params;

  if (block_a >= BCT_MAX_BLOCKS || block_b >= BCT_MAX_BLOCKS) return BCT_INVALID_BLOCK_NO;
  if (BCE_is_active(block_a) || BCE_is_active(block_b)) return BCT_DEFECTIVE_BLOCK;

  params_a = BCE_get_bc_exe_params_(block_a);
  params_b = BCE_get_bc_exe_params_(block_b);

  memcpy(&tmp_params, params_a, sizeof(BCE_Params));
  BCE_set_bc_exe_params_(block_a, params_b);
  BCE_set_bc_exe_params_(block_b, &tmp_params);

  return BCT_SUCCESS;
}

CCP_EXEC_STS Cmd_BCT_RESET_ROTATOR_INFO(const CTCP* packet)
{
  bct_id_t block;

  if (CCP_get_param_len(packet) != SIZE_OF_BCT_ID_T)
  {
    // �p�����[�^�̓u���b�N�ԍ�
    return CCP_EXEC_ILLEGAL_LENGTH;
  }

  // �p�����[�^��ǂݏo���B
  endian_memcpy(&block, CCP_get_param_head(packet), SIZE_OF_BCT_ID_T);

  return BCT_convert_bct_ack_to_ctcp_exec_sts(BCE_reset_rotator_info(block));
}

CCP_EXEC_STS Cmd_BCT_RESET_COMBINER_INFO(const CTCP* packet)
{
  bct_id_t block;

  if (CCP_get_param_len(packet) != SIZE_OF_BCT_ID_T)
  {
    // �p�����[�^�̓u���b�N�ԍ�
    return CCP_EXEC_ILLEGAL_LENGTH;
  }

  // �p�����[�^��ǂݏo���B
  endian_memcpy(&block, CCP_get_param_head(packet), SIZE_OF_BCT_ID_T);

  return BCT_convert_bct_ack_to_ctcp_exec_sts(BCE_reset_combiner_info(block));
}

// ����10��BC��NOP��o�^����R�}���h. �g�p�O�񂪋������邩??
// �p�X�^�p���Ɏg�p����̂�, �ꉞ�����ɂ��Ă������ق��������C������.
CCP_EXEC_STS Cmd_BCT_FILL_NOP(const CTCP* packet)
{
  cycle_t num_nop;
  cycle_t ti;

  num_nop = (cycle_t)CCP_get_param_head(packet)[0];

  if (num_nop > 10 || num_nop < 1) return CCP_EXEC_ILLEGAL_PARAMETER;
  if (block_command_table->pos.cmd + num_nop != 10) return CCP_EXEC_ILLEGAL_CONTEXT;

  for (ti = 11 - num_nop; ti < 11; ++ti)
  {
    CCP_form_tlc(&packet_, ti, Cmd_CODE_NOP, NULL, 0);
    BCT_register_cmd(&packet_);
  }

  return CCP_EXEC_SUCCESS;
}

CCP_EXEC_STS Cmd_BCT_SET_ROTATE_INTERVAL(const CTCP* packet)
{
  const unsigned char* param = CCP_get_param_head(packet);
  bct_id_t block;
  uint16_t interval;
  BCE_Params* bc_exe_params;

  if (CCP_get_param_len(packet) != (SIZE_OF_BCT_ID_T + 2))
  {
    // �p�����[�^�̓u���b�N�ԍ�2Byte�{����2Byte = 4Bytes
    return CCP_EXEC_ILLEGAL_LENGTH;
  }

  // �p�����[�^��ǂݏo���B
  endian_memcpy(&block, param, SIZE_OF_BCT_ID_T);
  endian_memcpy(&interval, param + SIZE_OF_BCT_ID_T, 2);

  if (interval == 0 || block >= BCT_MAX_BLOCKS)
  {
    // 0�Ŋ���ɍs���̂ł����ł͂���
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }

  bc_exe_params = BCE_get_bc_exe_params_(block);
  bc_exe_params->rotate.interval = interval;
  BCE_set_bc_exe_params_(block, bc_exe_params);

  return CCP_EXEC_SUCCESS;
}

#pragma section
