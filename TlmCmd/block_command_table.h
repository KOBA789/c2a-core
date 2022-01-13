/**
 * @file
 * @brief BCT�̒�`, BCT����̊֐�
 */
#ifndef BLOCK_COMMAND_TABLE_H_
#define BLOCK_COMMAND_TABLE_H_

#include <src_user/Library/stdint.h>

#define BCT_CMD_MAX_LENGTH (64)  /*!< BCT�ɕۑ�����p�P�b�g�̃p�P�b�g�T�C�Y�̏��
                                    TCP�̍ő咷�͂��Ȃ�傫���̂ŁC�����ł͂����菬�������ă�������ߖ񂷂�̂��ʗ�
                                    ���̒l���傫�ȃR�}���h��BCT�ɕۑ��ł��Ȃ��Ȃ邱�Ƃɒ��ӂ��邱��*/
#define BCT_MAX_CMD_NUM    (32)  //!< �P��BC�ɍő剽�̃R�}���h��}���ł��邩

#define BCT_MAX_BLOCKS    (383) //!< BC��ID�̍ő吔
// �����ς�����CCmd_RESET_(RE)STORE_BCT* ���ς���I
// 2019/07/19 �R�����g�ǉ�
// MM_NOT_DEFINED��BCT_MAX_BLOCKS ���g���Ă���̂ŁC-1����Ă�D
// ���ǁC�܂�-1����K�v�����܂�Ȃ������H


// ����CBCT�̃T�C�Y�ύX�����₷���悤�ɒ�`����
// �R�}���h�̈������`�F�b�N�������p����
#define SIZE_OF_BCT_ID_T    (2)    //!< bct_id_t �̌^�T�C�Y�D�������C���p�N�g������Ȃ�ɂ���̂ŁC�ς�

#include <src_user/Settings/CmdTlm/block_command_table_params.h>

#if SIZE_OF_BCT_ID_T == 1
typedef uint8_t bct_id_t;
#elif SIZE_OF_BCT_ID_T == 2
typedef uint16_t bct_id_t;
#elif SIZE_OF_BCT_ID_T == 4
typedef uint32_t bct_id_t;
#else
#error Illegal value for SIZE_OF_BCT_ID_T
#endif

#include "common_tlm_cmd_packet.h" // bct_id_t �̒�`��肠�Ƃ�include

/*
Block Command Table ��
BCT_MAX_BLOCKS x BCT_MAX_CMD_NUM
�̃e�[�u���ł���C
BCT_Pos.block ���u���b�N�ԍ��C
BCT_Pos.cmd   �����̃u���b�N���ł̈ʒu
��ێ����Ă���D
�R�}���h�̎��f�[�^��ۑ����Ă���{�̂�

[�\���̒�`]                        [�T�C�Y Byte]
BlockCommandTable                        7668               at 383
  BCT_Pos                               8 : 4 * 2
    block                                   2
    cmd                                     1
  BCT_Func[BCT_MAX_BLOCKS]           6128 : 4 * 4 * 383 at 383
  BCT_Table*[BCT_MAX_BLOCKS]         1532 : 4 * 383     at 383

BCT_Table                          784767               at 383
  length                                    1
  BCT_CmdData[BCT_MAX_CMD_NUM]               2048 : 64 * 32
    data[BCT_CMD_MAX_LENGTH]                        64

Sum                                792435 : 784767 + 7668
�Ȃ��C�\���̂̃T�C�Y�́C���̃����o�̃T�C�Y�̘a�ɂȂ�Ƃ͌���Ȃ����Ƃɒ��ӁD
���̒l�͖ڈ��ł���D(�f�[�^�̌������̂��߂ɁC�p�f�B���O����邱�Ƃ����邽��.)
*/

/**
 * @struct BCT_Pos
 * @brief  BCT���ł̍�ƒ��̈ʒu���
 */
typedef struct
{
  bct_id_t block; //!< �u���b�N�̈ʒu���
  uint8_t  cmd;   //!< ���̃u���b�N���łǂ̃R�}���h���w����
} BCT_Pos;

/**
 * @struct BCT_CmdData
 * @brief  Cmd �̕ۑ��ꏊ
 * @note   TCP_MAX_LENGTH (444) �Ɣ�ׂĂ��Ȃ�Z�� BCT_CMD_MAX_LENGTH (64)
 *         CTCP�̒�`�����[�U�[�ˑ��Ȃ̂Ō��� uint8_t �̔z��ɂȂ��Ă��� (FIXME: ����)
 */
typedef struct
{
  uint8_t data[BCT_CMD_MAX_LENGTH];
} BCT_CmdData;

/**
 * @struct BCT_Table
 * @brief  BCT �̖{��. ���ۂ� BC �� cmd ���m�ۂ���Ă���
 */
typedef struct
{
  uint8_t     length;                   //!< cmds �ɕۑ�����Ă���R�}���h��
  BCT_CmdData cmds[BCT_MAX_CMD_NUM];     //!< �e cmd ���ۑ������z��
} BCT_Table;

/**
 * @struct BCT_Func
 * @brief  �esetter, getter ������
 * @note   ���J����Ă��邪 private ����. getter �̓��b�v����Ă���̂ł�������g������
 *         ���݂��闝�R�͒��g�� MRAM �ȂǎO�d�璷������Ă���ꏊ�ɂ��邱�Ƃ����邽��
 */
typedef struct
{
  BCT_CmdData* (*get_bc_cmd_data_)(const BCT_Pos* pos);                           //!< cmd_data �� getter
  uint8_t      (*get_bc_length_)(const bct_id_t block);                           //!< length �� getter
  void         (*set_bc_cmd_data_)(const BCT_Pos* pos, const BCT_CmdData* value); //!< cmd_data �� setter
  void         (*set_bc_length_)(const bct_id_t block, uint8_t length);           //!< length �� setter
} BCT_Func;

/**
 * @struct BlockCommandTable
 * @brief  BCT �S�̂̍\����
 */
typedef struct
{
  BCT_Pos pos;                            //!< �o�^�Ȃǂ̍�ƒ��̈ʒu���
  BCT_Pos pos_stored;                     /*!< OBC�����̏����Ń|�C���^���ς��Ȃ��悤�ɂ��邽�߂̈ꎞ�ޔ��ꏊ�D
                                               �ڍׂ�C2A�h�L�������g �u�u���b�N�R�}���h�ǉ����@.md�v  */
  BCT_Table* blocks[BCT_MAX_BLOCKS];      //!< BCT�̊eBC�D�D���ȏꏊ�Ŋm�ۂł���l�Ƀ|�C���^�ɂ��Ă���
  BCT_Func   func[BCT_MAX_BLOCKS];        //!< BCT�֌W�̊֐�
} BlockCommandTable;

/**
 * @enum  BCT_ACK
 * @brief BCT �֌W�̊֐��̃G���[�X�e�[�^�X
 * @note  uint8_t ��z��
 */
typedef enum
{
  BCT_SUCCESS,
  BCT_INVALID_BLOCK_NO,
  BCT_INVALID_CMD_NO,
  BCT_ISORATED_CMD,
  BCT_DEFECTIVE_BLOCK,
  BCT_CMD_TOO_LONG,
  BCT_BC_FULL,
  BCT_ZERO_PERIOD,
  BCT_UNKNOWN
} BCT_ACK;

extern const BlockCommandTable* const block_command_table;

/**
 * @brief BCT �̏������֐�
 * @note  BCT �Ə����Ă��邪�����ł� BCE_init ���Ă΂�邽�� BC ���֌W����L�ۖ��ۂ������������.
 */
void BCT_initialize(void);

/**
 * @brief BCT_CmdData �� const pointer getter
 * @note �A�T�[�V�����͂���Ȃ�
 * @param[in] pos: const BCT_Pos*
 * @return const BCT_Pos*
 */
const BCT_CmdData* BCT_get_bc_cmd_data(const BCT_Pos* pos);

/**
 * @brief BCT �� length �� getter
 * @note �A�T�[�V�����͂���Ȃ��D�L���łȂ� block �̏ꍇ�C 0 ��Ԃ�
 * @param[in] block: BC �� idx
 * @return length
 */
uint8_t BCT_get_bc_length(const bct_id_t block);

/**
 * @brief �w�肳�ꂽ block �� BCT ������������
 * @param[in] block: BC �� idx
 * @return BCT_ACK
 */
BCT_ACK BCT_clear_block(const bct_id_t block);

/**
 * @brief pos �� pos_stored �ɕۑ�
 * @note �n��ǂ�OBC������BC�̃|�C���^���������Ă��������Ȃ�Ȃ��悤�ɂ��邽�߂̉��C. �ڍׂ�C2A�h�L�������g �u�u���b�N�R�}���h�ǉ����@.md�v
 * @param  void
 * @return void
 */
void BCT_store_pos(void);

/**
 * @brief pos �� pos_stored ���畜��
 * @param  void
 * @return void
 */
void BCT_restore_pos(void);

/**
 * @brief BCT_Pos* �� validate
 * @param  pos: BCT�̈ʒu
 * @return BCT_ACK
 */
BCT_ACK BCT_check_position(const BCT_Pos* pos);

/**
 * @brief BCT_Pos* ������������
 * @param[out] pos: �����ɍ�� pointer
 * @param[in] block: BC �� idx
 * @param[in] cmd: Cmds_data ���ł� idx
 * @return BCT_ACK
 */
BCT_ACK BCT_make_pos(BCT_Pos* pos, const bct_id_t block, const uint8_t cmd);

/**
 * @brief �ۑ��f�[�^��CTCP�ɃR�s�[����.
 * @param[in]  pos: �R�s�[����packet�̈ʒu
 * @param[out] packet: �R�s�[����� CTCP*
 * @return BCT_ACK
 * @note  �R�}���h���͋C�ɂ����ő咷��ǂݏo�����ƂƂ���.
 *        ���ۂ̒������̓R�s�[����Ă���w�b�_�Ɋi�[����Ă���.
 */
BCT_ACK BCT_load_cmd(const BCT_Pos* pos, CTCP* packet);

/**
 * @brief Cmd �� block_command_table->pos �ɓo�^����
 * @param[in] packet: �o�^���� Cmd �̓��e
 * @return BCT_ACK
 */
BCT_ACK BCT_register_cmd(const CTCP* packet);

/**
 * @brief ���ɓo�^����Ă��� BC �� Cmd �̒��g���O������㏑������
 * @param[in] pos: �㏑������ʒu
 * @param[in] packet: �㏑������� packet
 * @return BCT_ACK
 */
BCT_ACK BCT_overwrite_cmd(const BCT_Pos* pos, const CTCP* packet);

/**
 * @brief BCT �̒��g�� ���� BCT �� COPY ����
 * @param[in] dst_block �R�s�[��� bct_id
 * @param[in] src_block �R�s�[���� bct_id
 * @return BCT_ACK
 */
BCT_ACK BCT_copy_bct(const bct_id_t dst_block, const bct_id_t src_block);

// �ȉ�3�� BCT �ɓo�^����Ă��� Cmd ���炻�̓��e�����o��getter
/**
 * @brief BCT ���� Cmd �� CMD_CODE �� getter
 * @param[in] block: BC �� idx
 * @param[in] cmd: Cmds_data ���ł� idx
 * @return CMD_CODE
 * @note (block, cmd) ���ُ�l�������ꍇ�� Cmd_CODE_MAX ��Ԃ�
 */
CMD_CODE BCT_get_id(const bct_id_t block, const uint8_t cmd);

/**
 * @brief BCT ���� Cmd �� TI �� getter
 * @param[in] block: BC �� idx
 * @param[in] cmd: Cmds_data ���ł� idx
 * @return TI
 * @note (block, cmd) ���ُ�l�������ꍇ�� 0 ��Ԃ�
 */
cycle_t BCT_get_ti(const bct_id_t block, const uint8_t cmd);

/**
 * @brief BCT ���� Cmd �� param_head �� getter
 * @param[in] block: BC �� idx
 * @param[in] cmd: Cmds_data ���ł� idx
 * @return const uint8_t* �|�C���^
 * @note (block, cmd) ���ُ�l�������ꍇ�� (0, 0) �̂��̂�Ԃ�
 */
const uint8_t* BCT_get_param_head(const bct_id_t block, const uint8_t cmd);

/**
 * @brief BCT �� block �̎w���|�C���^ & func �����ւ���
 * @param[in] block_a: ����ւ��� BC
 * @param[in] block_b: ����ւ��� BC
 * @return BCT_ACK ���s����
 * @note ID1:HK:SRAM, ID2:AH:MRAM ������������ ID1:AH:MRAM, ID2:HK:SRAM ���o����
 *       `BCT_swap_contents` �Ƒg�ݍ��킹�邱�ƂŔԍ� - ���g�͂��̂܂܂� RAM ������؂�ւ��邱�Ƃ��o����
 *       �K�� `BCE_swap_address` �Ƌ��Ɏg�p���邱��
 */
BCT_ACK BCT_swap_address(const bct_id_t block_a, const bct_id_t block_b);

/**
 * @brief BCT �� block �̎w�����g�����ւ���
 * @param[in] block_a: ����ւ��� BC
 * @param[in] block_b: ����ւ��� BC
 * @return BCT_ACK ���s����
 * @note ID1:AH:MRAM, ID2:HK:SRAM ������������ ID1:HK:MRAM, ID2:AH:SRAM ���o����
 *       `BCT_swap_ram` �Ƒg�ݍ��킹�邱�ƂŔԍ� - ���g�͂��̂܂܂� RAM ������؂�ւ��邱�Ƃ��o����
 *       �K�� `BCE_swap_contents` �Ƌ��Ɏg�p���邱��
 */
BCT_ACK BCT_swap_contents(const bct_id_t block_a, const bct_id_t block_b);

/**
 * @brief enum�ϊ��p�֐�
 * @param  ack: BCT_ACK
 * @return CCP_EXEC_STS
 */
CCP_EXEC_STS BCT_convert_bct_ack_to_ctcp_exec_sts(BCT_ACK ack);

CCP_EXEC_STS Cmd_BCT_CLEAR_BLOCK(const CTCP* packet);
CCP_EXEC_STS Cmd_BCT_SET_BLOCK_POSITION(const CTCP* packet);
CCP_EXEC_STS Cmd_BCT_COPY_BCT(const CTCP* packet);
CCP_EXEC_STS Cmd_BCT_OVERWRITE_CMD(const CTCP* packet);

#endif
