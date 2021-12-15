#pragma section REPRO
/**
 * @file  block_command_table.c
 * @brief BCT�̒�`, BCT����̊֐�
 */

#include "block_command_table.h"

#include "string.h" // for memcpy

#include "block_command_executor.h"

#include <src_user/CmdTlm/block_command_user_settings.h>
#include <src_user/CmdTlm/block_command_definitions.h>
#include "packet_handler.h"
#include "command_analyze.h"
#include <src_user/CmdTlm/command_definitions.h>
#include "../System/TimeManager/time_manager.h"
#include "../Library/endian_memcpy.h"
#include "../System/WatchdogTimer/watchdog_timer.h"
#include "common_tlm_cmd_packet_util.h"

static BlockCommandTable block_command_table_;
const BlockCommandTable* const block_command_table = &block_command_table_;

/**
 * @brief BCT_Func �Ƀf�t�H���g�̊֐��|�C���^������
 */
static void BCT_load_default_func_pointer_(void);

// �ȉ�4�� default (SRAM, �璷����) �� getter, setter
static BCT_CmdData* BC_get_bc_cmd_data_default_(const BCT_Pos* pos);
static uint8_t BC_get_bc_length_default_(const bct_id_t block);
static void BC_set_bc_cmd_data_default_(const BCT_Pos* pos, const BCT_CmdData* value);
static void BC_set_bc_length_default_(const bct_id_t block, uint8_t length);

// BCT_Func �����b�v���� static �� getter, setter. �Ȃ̂� �Ԃ�l�� const �����Ȃ�
static BCT_CmdData* BCT_get_bc_cmd_data_(const BCT_Pos* pos);
static void BCT_set_bc_cmd_data_(const BCT_Pos* pos, const BCT_CmdData* value);
static void BCT_set_bc_length_(const bct_id_t block, uint8_t length);

/**
 * @brief pos, pos_stored �� initialize
 */
static void BCT_clear_pos_(void);

static BCT_ACK BCT_set_position_(const BCT_Pos* pos);

/**
 * @brief pos �̎w���ꏊ�� packet �̓��e��ۑ� (TCP_PRM_HDR_LEN �̒������l��)
 */
static BCT_ACK BCT_save_cmd_(const BCT_Pos* pos, const CTCP* packet);

void BCT_initialize(void)
{
  bct_id_t i;

  BCT_load_default_func_pointer_();
  BCE_load_default_func_pointer();
  BCUS_load_user_settings(&block_command_table_, (BlockCommandExecutor*)block_command_executor); // const_cast

  for (i = 0; i < BCT_MAX_BLOCKS; ++i)
  {
    if (!BCUS_bc_should_clear(i)) continue;

    BCT_clear_block(i);
    WDT_clear_wdt();      // �O�ׁ̈H
  }

  BCT_clear_pos_();

  BC_load_defaults();
}

static void BCT_load_default_func_pointer_(void)
{
  bct_id_t block;
  for (block = 0; block < BCT_MAX_BLOCKS; ++block)
  {
    block_command_table_.func[block].get_bc_cmd_data_ = BC_get_bc_cmd_data_default_;
    block_command_table_.func[block].get_bc_length_ = BC_get_bc_length_default_;

    block_command_table_.func[block].set_bc_cmd_data_ = BC_set_bc_cmd_data_default_;
    block_command_table_.func[block].set_bc_length_ = BC_set_bc_length_default_;
  }
}

static BCT_CmdData* BC_get_bc_cmd_data_default_(const BCT_Pos* pos)
{
  return &block_command_table_.blocks[pos->block]->cmds[pos->cmd];
}

static void BC_set_bc_cmd_data_default_(const BCT_Pos* pos, const BCT_CmdData* value)
{
  BCT_CmdData* cmd_data = &block_command_table_.blocks[pos->block]->cmds[pos->cmd];
  if ((unsigned char*)cmd_data == (const unsigned char*)value) return;
  memcpy(cmd_data, value, sizeof(BCT_CmdData));
}

static uint8_t BC_get_bc_length_default_(const bct_id_t block)
{
  return block_command_table_.blocks[block]->length;
}

static void BC_set_bc_length_default_(const bct_id_t block, uint8_t length)
{
  block_command_table_.blocks[block]->length = length;
}

static BCT_CmdData* BCT_get_bc_cmd_data_(const BCT_Pos* pos)
{
  return block_command_table_.func[pos->block].get_bc_cmd_data_(pos);
}

static void BCT_set_bc_cmd_data_(const BCT_Pos* pos, const BCT_CmdData* value)
{
  block_command_table_.func[pos->block].set_bc_cmd_data_(pos, value);
}

static void BCT_set_bc_length_(const bct_id_t block, uint8_t length)
{
  block_command_table_.func[block].set_bc_length_(block, length);
}

const BCT_CmdData* BCT_get_bc_cmd_data(const BCT_Pos* pos)
{
  return block_command_table_.func[pos->block].get_bc_cmd_data_(pos);
}

uint8_t BCT_get_bc_length(const bct_id_t block)
{
  if (block >= BCT_MAX_BLOCKS) return 0;
  return block_command_table_.func[block].get_bc_length_(block);
}

static void BCT_clear_pos_(void)
{
  block_command_table_.pos.block = 0;
  block_command_table_.pos.cmd   = 0;
  block_command_table_.pos_stored.block = 0;
  block_command_table_.pos_stored.cmd   = 0;
}

static BCT_ACK BCT_set_position_(const BCT_Pos* pos)
{
  BCT_ACK ack = BCT_check_position(pos);

  if (ack != BCT_SUCCESS) return ack;

  block_command_table_.pos = *pos;

  return BCT_SUCCESS;
}

BCT_ACK BCT_check_position(const BCT_Pos* pos)
{
  if (pos->block >= BCT_MAX_BLOCKS)
  {
    return BCT_INVALID_BLOCK_NO;
  }
  if (pos->cmd >= BCT_MAX_CMD_NUM)
  {
    return BCT_INVALID_CMD_NO;
  }

  return BCT_SUCCESS;
}

BCT_ACK BCT_register_cmd(const CTCP* packet)
{
  BCT_ACK ack = BCT_check_position(&block_command_table_.pos);
  uint8_t length;

  if (ack != BCT_SUCCESS) return ack;

  length = BCT_get_bc_length(block_command_table_.pos.block);

  // 2018/06/20 �R�����g�ǋL
  // ���ݓo�^����Ă���R�}���h�������傫����΃G���[
  // �܂�C�o�^����Ă���e�[�u���̏㏑���͂ł��邪�C
  // ���o�^�̃Z�����΂��āC�s�A���ɓo�^���邱�Ƃ͂ł��Ȃ��C�͂��D
  if (block_command_table_.pos.cmd > length)
  {
    return BCT_ISORATED_CMD;
  }

  ack = BCT_save_cmd_(&block_command_table_.pos, packet);

  if (ack != BCT_SUCCESS)
  {
    return ack;
  }

  // �A���o�^�ɔ����A�|�C���^�ʒu��i�߂�B������߂͎���o�^�����W�F�N�g�����B
  ++(block_command_table_.pos.cmd);

  // �u���b�N�����ւ̃R�}���h�ǉ��̏ꍇ�̓u���b�N�����X�V����
  if (length < block_command_table_.pos.cmd)
  {
    length = block_command_table_.pos.cmd;
    BCT_set_bc_length_(block_command_table_.pos.block, length);
  }

  return BCT_SUCCESS;
}

BCT_ACK BCT_overwrite_cmd(const BCT_Pos* pos, const CTCP* packet)
{
  BCT_ACK ack = BCT_check_position(pos);

  if (ack != BCT_SUCCESS) return ack;
  if (BCT_get_bc_length(pos->block) <= pos->cmd) return BCT_ISORATED_CMD;

  return BCT_save_cmd_(pos, packet);
}

// FIXME: �ȉ������֐��ɂ킽���� BlockCmdCmdData �� CTCP�̕ϊ����C����cast�ɂ���čs���Ă��邪�CCTCP�̓��[�U�[��`�Ȃ��߁C����i=TCP�ȊO�j�ɑΉ��ł���悤�ɂ���
//        ���l�̗��R�� memcpy �Ȃǂ��Ή�����K�v������D
static BCT_ACK BCT_save_cmd_(const BCT_Pos* pos, const CTCP* packet)
{
  // FIXME: TCP�Ɉˑ����Ȃ��C�܂�CTCP�Ɉˑ�����R�[�h�ɂ���
  BCT_ACK ack = BCT_check_position(pos);
  if (ack != BCT_SUCCESS) return ack;

  // �p�P�b�g�S�����z��ő咷�𒴂��Ă���ꍇ�ُ͈픻��
  if (CCP_get_packet_len(packet) > BCT_CMD_MAX_LENGTH) return BCT_CMD_TOO_LONG;

  // �i�[�\�ȃp�P�b�g�Ȃ���e���R�s�[���ۑ�
  BCT_set_bc_cmd_data_(pos, (BCT_CmdData*)packet->packet); // const_cast

  return BCT_SUCCESS;
}

BCT_ACK BCT_load_cmd(const BCT_Pos* pos, CTCP* packet)
{
  BCT_ACK ack = BCT_check_position(pos);
  BCT_CmdData* bc_cmddata;

  if (ack != BCT_SUCCESS) return ack;
  bc_cmddata = BCT_get_bc_cmd_data_(pos);
  memcpy(&packet->packet, bc_cmddata, BCT_CMD_MAX_LENGTH);   // FIXME: CTCP��TCP�ˑ��Ȃ̂Œ���

  return BCT_SUCCESS;
}

BCT_ACK BCT_copy_bct(const bct_id_t dst_block, const bct_id_t src_block)
{
  uint8_t cmd;
  uint8_t length = BCT_get_bc_length(src_block);

  if (dst_block >= BCT_MAX_BLOCKS || src_block >= BCT_MAX_BLOCKS) return BCT_INVALID_BLOCK_NO;
  if (BCE_is_active(dst_block)) return BCT_INVALID_BLOCK_NO;

  BCT_clear_block(dst_block);
  BCT_set_bc_length_(dst_block, BCT_get_bc_length(src_block));
  for (cmd = 0; cmd < length; ++cmd)
  {
    BCT_Pos dst_pos, src_pos;
    BCT_make_pos(&dst_pos, dst_block, cmd);
    BCT_make_pos(&src_pos, src_block, cmd);
    BCT_set_bc_cmd_data_(&dst_pos, BCT_get_bc_cmd_data(&src_pos));
  }

  return BCT_SUCCESS;
}

CMD_CODE BCT_get_id(const bct_id_t block, const uint8_t cmd)
{
  BCT_Pos pos;
  BCT_make_pos(&pos, block, cmd);
  if (BCT_check_position(&pos) != BCT_SUCCESS) return Cmd_CODE_MAX;

  return CCP_get_id((CTCP*)BCT_get_bc_cmd_data_(&pos));
}

cycle_t BCT_get_ti(const bct_id_t block, const uint8_t cmd)
{
  BCT_Pos pos;
  BCT_make_pos(&pos, block, cmd);
  if (BCT_check_position(&pos) != BCT_SUCCESS) return 0;

  return CCP_get_ti((CTCP*)BCT_get_bc_cmd_data_(&pos));
}

const uint8_t* BCT_get_param_head(const bct_id_t block, const uint8_t cmd)
{
  BCT_Pos pos;
  BCT_make_pos(&pos, block, cmd);
  if (BCT_check_position(&pos) != BCT_SUCCESS)
  {
    BCT_make_pos(&pos, 0, 0);
    return CCP_get_param_head((CTCP*)BCT_get_bc_cmd_data_(&pos));
  }

  return CCP_get_param_head((CTCP*)BCT_get_bc_cmd_data_(&pos));
}

/* 2018/11/27
 * �n��ǂ�OBC������BC�̃|�C���^���������Ă��������Ȃ�Ȃ��悤�ɂ��邽�߂̉��C
 * �ڍׂ�C2A�h�L�������g �u�u���b�N�R�}���h�ǉ����@.md�v
 */
void BCT_store_pos(void)
{
  block_command_table_.pos_stored.block = block_command_table_.pos.block;
  block_command_table_.pos_stored.cmd   = block_command_table_.pos.cmd  ;
  return;
}

void BCT_restore_pos(void)
{
  block_command_table_.pos.block = block_command_table_.pos_stored.block;
  block_command_table_.pos.cmd   = block_command_table_.pos_stored.cmd;
  return;
}

BCT_ACK BCT_make_pos(BCT_Pos* pos, const bct_id_t block, const uint8_t cmd)
{
  pos->block = block;
  pos->cmd = cmd;

  return BCT_check_position(pos);
}

BCT_ACK BCT_swap_address(const bct_id_t block_a, const bct_id_t block_b)
{
  BCT_Table* tmp_bct_table;
  BCT_Func   tmp_bct_func;

  if (block_a >= BCT_MAX_BLOCKS || block_b >= BCT_MAX_BLOCKS) return BCT_INVALID_BLOCK_NO;
  if (BCE_is_active(block_a) || BCE_is_active(block_b)) return BCT_DEFECTIVE_BLOCK;

  tmp_bct_table = block_command_table_.blocks[block_a];
  block_command_table_.blocks[block_a] = block_command_table_.blocks[block_b];
  block_command_table_.blocks[block_b] = tmp_bct_table;

  memcpy(&tmp_bct_func, &block_command_table_.func[block_a], sizeof(BCT_Func));
  memcpy(&block_command_table_.func[block_a], &block_command_table_.func[block_b], sizeof(BCT_Func));
  memcpy(&block_command_table_.func[block_b], &tmp_bct_func, sizeof(BCT_Func));

  return BCT_SUCCESS;
}

BCT_ACK BCT_swap_contents(const bct_id_t block_a, const bct_id_t block_b)
{
  int i;

  uint8_t     tmp_length, length_a, length_b;
  BCT_CmdData tmp_bct_cmddata;

  if (block_a >= BCT_MAX_BLOCKS || block_b >= BCT_MAX_BLOCKS) return BCT_INVALID_BLOCK_NO;
  if (BCE_is_active(block_a) || BCE_is_active(block_b)) return BCT_DEFECTIVE_BLOCK;

  length_a = BCT_get_bc_length(block_a);
  length_b = BCT_get_bc_length(block_b);
  BCT_set_bc_length_(block_a, length_b);
  BCT_set_bc_length_(block_b, length_a);

  tmp_length = (uint8_t)((length_a >= length_b) ? length_a : length_b);

  for (i = 0; i < tmp_length; ++i)
  {
    BCT_Pos pos_a, pos_b;
    BCT_CmdData* cmd_a;
    BCT_CmdData* cmd_b;

    BCT_make_pos(&pos_a, block_a, (uint8_t)i);
    BCT_make_pos(&pos_b, block_b, (uint8_t)i);

    cmd_a = BCT_get_bc_cmd_data_(&pos_a);
    cmd_b = BCT_get_bc_cmd_data_(&pos_b);
    memcpy(&tmp_bct_cmddata, cmd_a, sizeof(BCT_CmdData));
    BCT_set_bc_cmd_data_(&pos_a, cmd_b);
    BCT_set_bc_cmd_data_(&pos_b, &tmp_bct_cmddata);

    WDT_clear_wdt(); // ���Ԃ�������̂�
  }

  return BCT_SUCCESS;
}

CCP_EXEC_STS BCT_convert_bct_ack_to_ctcp_exec_sts(BCT_ACK ack)
{
  switch (ack)
  {
  case BCT_SUCCESS:
    return CCP_EXEC_SUCCESS;

  case BCT_INVALID_BLOCK_NO:
    return CCP_EXEC_ILLEGAL_PARAMETER;

  case BCT_INVALID_CMD_NO:
    return CCP_EXEC_CMD_NOT_DEFINED;

  case BCT_DEFECTIVE_BLOCK:
    return CCP_EXEC_ILLEGAL_CONTEXT;

  case BCT_CMD_TOO_LONG:
    return CCP_EXEC_ILLEGAL_PARAMETER;

  case BCT_BC_FULL:
    return CCP_EXEC_ILLEGAL_CONTEXT;

  case BCT_ZERO_PERIOD:
    return CCP_EXEC_ILLEGAL_PARAMETER;

  default:
    return CCP_EXEC_UNKNOWN;
  }
}

CCP_EXEC_STS Cmd_BCT_CLEAR_BLOCK(const CTCP* packet)
{
  bct_id_t block;
  BCT_ACK  ack;

  if (CCP_get_param_len(packet) != SIZE_OF_BCT_ID_T)
  {
    // �p�����[�^�̓u���b�N�ԍ�
    return CCP_EXEC_ILLEGAL_LENGTH;
  }

  // �p�����[�^��ǂݏo���B
  endian_memcpy(&block, CCP_get_param_head(packet), SIZE_OF_BCT_ID_T);

  // �w�肳�ꂽ�u���b�N�ԍ��̃N���A�����s�B
  ack = BCT_clear_block(block);

  return BCT_convert_bct_ack_to_ctcp_exec_sts(ack);
}

BCT_ACK BCT_clear_block(const bct_id_t block)
{
  if (block >= BCT_MAX_BLOCKS) return BCT_INVALID_BLOCK_NO;

  block_command_table_.pos.block = block;
  block_command_table_.pos.cmd = 0;
  BCT_set_bc_length_(block, 0);

  BCE_clear_block(block);

  return BCT_SUCCESS;
}

CCP_EXEC_STS Cmd_BCT_SET_BLOCK_POSITION(const CTCP* packet)
{
  const unsigned char* param = CCP_get_param_head(packet);
  BCT_Pos pos;
  BCT_ACK ack;

  if (CCP_get_param_len(packet) != (SIZE_OF_BCT_ID_T + 1))
  {
    // �p�����[�^�̓u���b�N�ԍ��ƃR�}���h�ԍ�1Byte�B
    return CCP_EXEC_ILLEGAL_LENGTH;
  }

  // �p�����[�^��ǂݏo��
  endian_memcpy(&pos.block, param, SIZE_OF_BCT_ID_T);
  pos.cmd = param[SIZE_OF_BCT_ID_T];

  ack = BCT_set_position_(&pos);

  return BCT_convert_bct_ack_to_ctcp_exec_sts(ack);
}

CCP_EXEC_STS Cmd_BCT_COPY_BCT(const CTCP* packet)
{
  const unsigned char* param = CCP_get_param_head(packet);
  uint16_t dst_block, src_block;
  BCT_ACK ack;

  if (CCP_get_param_len(packet) != 2 * SIZE_OF_BCT_ID_T) return CCP_EXEC_ILLEGAL_LENGTH;
  endian_memcpy(&dst_block, param, SIZE_OF_BCT_ID_T);
  endian_memcpy(&src_block, param + SIZE_OF_BCT_ID_T, SIZE_OF_BCT_ID_T);

  ack = BCT_copy_bct(dst_block, src_block);
  return BCT_convert_bct_ack_to_ctcp_exec_sts(ack);
}

CCP_EXEC_STS Cmd_BCT_OVERWRITE_CMD(const CTCP* packet)
{
  CMD_CODE cmd_id = (CMD_CODE)CCP_get_param_from_packet(packet, 0, uint16_t);
  cycle_t  ti     = (cycle_t)CCP_get_param_from_packet(packet, 1, uint32_t);
  bct_id_t block  = (bct_id_t)CCP_get_param_from_packet(packet, 2, bct_id_t);
  uint8_t  cmd    = CCP_get_param_from_packet(packet, 3, uint8_t);

  BCT_Pos  pos;
  BCT_CmdData bct_cmddata; // FIXME BCT_CmdData <-> CTCP
  BCT_CmdData tmp_param;   // �������񂱂���param���R�s�[����
  uint16_t real_param_len = CCP_get_param_len(packet);
  uint16_t min_cmd_param_len = sizeof(CMD_CODE) + sizeof(cycle_t) + sizeof(bct_id_t) + sizeof(uint8_t);
  uint16_t max_cmd_param_len = min_cmd_param_len + sizeof(BCT_CmdData);
  uint16_t cmd_param_len;

  // raw �Ȃ̂ň������`�F�b�N
  if (real_param_len < min_cmd_param_len || real_param_len > max_cmd_param_len) return CCP_EXEC_ILLEGAL_LENGTH;

  cmd_param_len = real_param_len - min_cmd_param_len;
  CCP_get_raw_param_from_packet(packet, &tmp_param, sizeof(BCT_CmdData));

  BCT_make_pos(&pos, block, cmd);
  CCP_form_tlc((CTCP*)&bct_cmddata, ti, (CMD_CODE)cmd_id, (const uint8_t*)&tmp_param, cmd_param_len);
  BCT_overwrite_cmd(&pos, (CTCP*)&bct_cmddata);

  return CCP_EXEC_SUCCESS;
}

#pragma section
