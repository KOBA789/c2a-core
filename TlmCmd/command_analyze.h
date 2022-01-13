/**
 * @file
 * @brief  �R�}���h�̎��s�E�o�^�C�R�}���h�֘A����
 */
#ifndef COMMAND_ANALYZE_H_
#define COMMAND_ANALYZE_H_

#include "common_tlm_cmd_packet.h"
#include <src_user/TlmCmd/command_definitions.h>

#define CA_TLM_PAGE_SIZE      (32)                                  //!< �R�}���h�e�[�u����1�e�����g���p�P�b�g(=1�y�[�W)�Ɋi�[�����R�}���h���i�y�[�W�l�[�V�����p�j
#define CA_TLM_PAGE_MAX       (48)                                  //!< �R�}���h�e�[�u���y�[�W���i�y�[�W�l�[�V�����p�j
#define CA_MAX_CMDS           (CA_TLM_PAGE_SIZE * CA_TLM_PAGE_MAX)  //!< �R�}���h�e�[�u���T�C�Y�D���Ȃ킿�o�^�ł���ő�R�}���h��
#define CA_MAX_CMD_PARAM_NUM  (6)                                   //!< �R�}���h�p�����^�i�����j�̍ő吔�D�����_�ł̓��[�U�[���ŉςɂ��邱�Ƃ�z�肵�Ă͂Ȃ�

#include <src_user/Settings/TlmCmd/command_analyze_params.h>


/**
 * @enum  AC_ACK
 * @brief CA �̔ėp�Ԃ�l
 * @note  uint8_t ��z��
 */
typedef enum
{
  CA_ACK_OK = 0,  //!< ����I��
  CA_ACK_ERR      //!< �G���[
} CA_ACK;

/**
 * @struct CA_PackedParamSizeInfo
 * @brief  �p�����^�̃T�C�Y�������k�����\����
 * @note   2�̃p�����^�T�C�Y���� uint8_t �ɉ������߂�
 * @note   �l�� CA_PARAM_SIZE_TYPE
 */
typedef struct
{
  union
  {
    uint8_t byte;
    struct
    {
      unsigned first: 4;
      unsigned second: 4;
    } bit;
  } packed_info;
} CA_PackedParamSizeInfo;

/**
 * @enum  CA_PARAM_SIZE_TYPE
 * @brief �R�}���h�p�����^�T�C�Y
 * @note  unsigned 4bit �ϐ���z��
 */
typedef enum
{
  CA_PARAM_SIZE_TYPE_NONE = 0,    //!< �p�����^�Ȃ�
  CA_PARAM_SIZE_TYPE_1BYTE,       //!< 1 byte
  CA_PARAM_SIZE_TYPE_2BYTE,       //!< 2 byte
  CA_PARAM_SIZE_TYPE_4BYTE,       //!< 4 byte
  CA_PARAM_SIZE_TYPE_8BYTE,       //!< 8 byte
  CA_PARAM_SIZE_TYPE_RAW = 0xf    //!< RAW �p�����^
} CA_PARAM_SIZE_TYPE;

/**
 * @struct CA_CmdInfo
 * @brief  �R�}���h�e�[�u���̗v�f�ƂȂ�\����
 */
typedef struct
{
  CCP_EXEC_STS (*cmd_func)(const CTCP*);                                    //!< �R�}���h�ƂȂ�֐�
  CA_PackedParamSizeInfo param_size_infos[(CA_MAX_CMD_PARAM_NUM + 1) / 2];  //!< �p�����^�T�C�Y���
} CA_CmdInfo;

/**
 * @struct CommandAnalyze
 * @brief  CommandAnalyze �� Info �\����
 */
typedef struct
{
  CA_CmdInfo cmd_table[CA_MAX_CMDS];  //!< �R�}���h�e�[�u��
  uint8_t tlm_page_no;                //!< �e�����Ŏg���y�[�W��
} CommandAnalyze;

extern const CommandAnalyze* const command_analyze;


/**
 * @brief  CA �̏�����
 * @param  void
 * @return void
 */
void CA_initialize(void);

/**
 * @brief  �R�}���h���s�̖{��
 * @param  packet: ���s����R�}���h
 * @return CCP_EXEC_STS
 */
CCP_EXEC_STS CA_execute_cmd(const CTCP* packet);

/**
 * @brief  �R�}���h�p�����^�����擾����
 * @param  cmd_code:  �`�F�b�N����R�}���h��ID
 * @return �R�}���h�p�����^��
 * @note   �s���Ȉ����̏ꍇ�� 0 ��Ԃ�
 */
uint8_t CA_get_cmd_param_num(CMD_CODE cmd_code);

/**
 * @brief  �R�}���h�p�����^�T�C�Y���擾����
 * @param  cmd_code:  �`�F�b�N����R�}���h��ID
 * @param  n: N�Ԗڂ̃p�����^ �i0�N�Z�j
 * @return �R�}���h�p�����^�T�C�Y
 * @note   �s���Ȉ����̏ꍇ�� 0 ��Ԃ�
 */
uint8_t CA_get_cmd_param_size(CMD_CODE cmd_code, uint8_t n);

/**
 * @brief  �ŏ��R�}���h�p�����^�����擾����
 * @param  cmd_code:  �`�F�b�N����R�}���h��ID
 * @return �R�}���h�p�����^��
 * @note   �s���Ȉ����̏ꍇ�� 0 ��Ԃ�
 * @note   RAW �p�����^�̓T�C�Y 0 �Ƃ��Čv�Z���邽�߁C�ŏ��R�}���h�p�����^���ƂȂ�
 */
uint16_t CA_get_cmd_param_min_len(CMD_CODE cmd_code);

/**
 * @brief  �R�}���h�p�����^�����`�F�b�N����
 * @param  cmd_code:  �`�F�b�N����R�}���h��ID
 * @param  param_len: �p�����^��
 * @return CA_ACK
 * @note   �s���Ȉ����̏ꍇ�� CA_ACK_ERR ��Ԃ�
 */
CA_ACK CA_ckeck_cmd_param_len(CMD_CODE cmd_code, uint16_t param_len);

/**
 * @brief  RAW �p�����^�������Ă��邩�H
 * @param  cmd_code:  �`�F�b�N����R�}���h��ID
 * @retval 1: RAW �p�����^����
 * @retval 0: RAW �p�����^�Ȃ� or �s���Ȉ���
 */
int CA_has_raw_param(CMD_CODE cmd_code);

/**
 * @brief  Cmd Table�̃��[�h
 * @note   ��`�� /src_user/TlmCmd/CommandDefinitions.c �ɂ���
 * @param  cmd_table: Cmd Table�̎���
 * @return void
 */
void CA_load_cmd_table(CA_CmdInfo cmd_table[CA_MAX_CMDS]);

CCP_EXEC_STS Cmd_CA_REGISTER_CMD(const CTCP* packet);

CCP_EXEC_STS Cmd_CA_SET_PAGE_FOR_TLM(const CTCP* packet);

#endif
