/**
 * @file  block_command_executor.h
 * @brief BCT�̎��s����̊֐�, �p�����[�^�[
 * @note  Block Command ExeInfo��
 *        Block Command Table (BCT) �̎��s���̃p�����[�^�[�ł���.
 *        ���� Cmd �̃f�[�^�ۑ����Ǝ��s���͕�������Ă��炸, BCT �̂��̎��s��Ԃ͂����ɕۑ������.
 *        ����Ď��s�p�����[�^�[�� BCT �Ɠ������� (BCT_MAX_BLOCKS) �m�ۂ����.
 */
#ifndef BLOCK_COMMAND_EXECUTOR_H_
#define BLOCK_COMMAND_EXECUTOR_H_

#include "common_tlm_cmd_packet.h"
#include "block_command_table.h"

/**
 * @struct BCE_Params
 * @brief  BCT �̎��s�p�����[�^�[��ۑ�����\����
 */
typedef struct
{
  uint8_t is_active; //!< �L�����ǂ����D1: �L��, 0: ����
  struct
  {
    uint16_t counter;  //!< interval �̌v�Z�̂��߂� counter[cycle]
    uint16_t interval; //!< interval[cycle] ���ƂɎ��s����� (���1)
    uint8_t  next_cmd; //!< ���Ɏ��s����� cmd
  } rotate;
  struct
  {
    uint16_t call_num;              //!< Cmd_BCT_TIMELIMIT_COMBINE_BLOCK���Ă΂�邽�тɃC���N�������g
    uint16_t timeover_num;          //!< ���Ԑ����Ɉ�������������C���N�������g
    uint8_t  last_timeover_cmd_pos; //!< �O�񎞊Ԑ����Ɉ������������Ƃ���cmd pos
    uint8_t  worst_cmd_pos;         //!< �ň��P�[�X��cmd pos
  } timelimit_combine;
} BCE_Params;

/**
 * @struct BCE_Func
 * @brief  BCE_Params �� getter, setter (private)
 * @note   ���J����Ă��邪 private ����. getter �̓��b�v���ꂽ���̂����J����Ă���̂ł�������g������
 *         ���݂��闝�R�͒��g�� MRAM �ȂǎO�d�璷������Ă���ꏊ�ɂ��邱�Ƃ����邽��
 */
typedef struct
{
  BCE_Params* (*get_bc_exe_params_)(const bct_id_t block);
  void        (*set_bc_exe_params_)(const bct_id_t block, const BCE_Params* bc_params);
} BCE_Func;

/**
 * @struct BlockCommandExecutor
 * @brief  ���s����S�̂� struct
 */
typedef struct
{
  BCE_Params* bc_exe_params[BCT_MAX_BLOCKS];    //!< �p�����^�D�s�������o����悤�Ƀ|�C���^�Ŋm��
  BCE_Func    bc_exe_func[BCT_MAX_BLOCKS];      //!< BCE_Params �ɑ΂��鑀��֐��D�e BC ���ƂɊm��
} BlockCommandExecutor;

extern const BlockCommandExecutor* const block_command_executor;

/**
 * @brief BCE_Params �� const getter
 * @param  block: BC �� idx
 * @return BCE_Params
 */
const BCE_Params* BCE_get_bc_exe_params(const bct_id_t block);

/**
 * @brief BCE_Params �Ƀf�t�H���g�̊֐��|�C���^������
 * @param  void
 * @return void
 */
void BCE_load_default_func_pointer(void);

/**
 * @brief �w�肳�ꂽ block �� BCE ��������
 * @param[in] block: BC �� idx
 * @return BCT_ACK
 */
BCT_ACK BCE_clear_block(const bct_id_t block);

/**
 * @brief block_command_table->pos �� BC �� activate ����
 * @param  void
 * @return BCT_ACK
 */
BCT_ACK BCE_activate_block(void);

// �w�肳�ꂽ BC �� activate, inactivate
BCT_ACK BCE_activate_block_by_id(bct_id_t block);
BCT_ACK BCE_inactivate_block_by_id(bct_id_t block);

// �w�肳�ꂽ BCE �̂��� rotator, combiner �� initialize
BCT_ACK BCE_reset_rotator_info(const bct_id_t block);
BCT_ACK BCE_reset_combiner_info(const bct_id_t block);

/**
 * @brief BCT �� length �� getter
 * @note �A�T�[�V�����͂���Ȃ��D�L���łȂ� block �̏ꍇ�C 0: ���� ��Ԃ�
 * @param[in] block: BC �� idx
 * @return is_active
 */
uint8_t BCE_is_active(const bct_id_t block);

/**
 * @brief BCE �� params �̎w���|�C���^ & func �����ւ���
 * @param[in] block_a: ����ւ��� BC
 * @param[in] block_b: ����ւ��� BC
 * @return BCT_ACK ���s����
 * @note ID1:HK:SRAM, ID2:AH:MRAM ������������ ID1:AH:MRAM, ID2:HK:SRAM ���o���� (���������̂�param)
 *       `BCE_swap_contents` �Ƒg�ݍ��킹�邱�ƂŔԍ� - ���g�͂��̂܂܂� RAM ������؂�ւ��邱�Ƃ��o����
 *       �K�� `BCT_swap_address` �Ƌ��Ɏg�p���邱��
 */
BCT_ACK BCE_swap_address(const bct_id_t block_a, const bct_id_t block_b);

/**
 * @brief BCE �� params �̎w�����g�����ւ���
 * @param[in] block_a: ����ւ��� BC
 * @param[in] block_b: ����ւ��� BC
 * @return BCT_ACK ���s����
 * @note ID1:AH:MRAM, ID2:HK:SRAM ������������ ID1:HK:MRAM, ID2:AH:SRAM ���o���� (���������̂�param)
 *       `BCE_swap_address` �Ƒg�ݍ��킹�邱�ƂŔԍ� - ���g�͂��̂܂܂� RAM ������؂�ւ��邱�Ƃ��o����
 *       �K�� `BCT_swap_contents` �Ƌ��Ɏg�p���邱��
 */
BCT_ACK BCE_swap_contents(const bct_id_t block_a, const bct_id_t block_b);

// �ȉ� �쐬���ꂽ�e�� CMDFILE �Ɛ���������邽�߉�������Ă��Ȃ�
// FIXME: BCT -> BCE
CCP_EXEC_STS Cmd_BCT_ACTIVATE_BLOCK(const CTCP* packet);
CCP_EXEC_STS Cmd_BCT_ACTIVATE_BLOCK_BY_ID(const CTCP* packet);
CCP_EXEC_STS Cmd_BCT_INACTIVATE_BLOCK_BY_ID(const CTCP* packet);
CCP_EXEC_STS Cmd_BCT_ROTATE_BLOCK(const CTCP* packet);
CCP_EXEC_STS Cmd_BCT_COMBINE_BLOCK(const CTCP* packet);
CCP_EXEC_STS Cmd_BCT_TIMELIMIT_COMBINE_BLOCK(const CTCP* packet);
CCP_EXEC_STS Cmd_BCT_RESET_ROTATOR_INFO(const CTCP* packet);
CCP_EXEC_STS Cmd_BCT_RESET_COMBINER_INFO(const CTCP* packet);
CCP_EXEC_STS Cmd_BCT_FILL_NOP(const CTCP* packet);
CCP_EXEC_STS Cmd_BCT_SET_ROTATE_INTERVAL(const CTCP* packet);

#endif
