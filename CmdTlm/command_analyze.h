/**
 * @file   command_analyze.h
 * @brief  �R�}���h�̎��s�E�o�^�C�R�}���h�֘A����
 */
#ifndef COMMAND_ANALYZE_H_
#define COMMAND_ANALYZE_H_

#include "common_tlm_cmd_packet.h"
#include <src_user/CmdTlm/command_definitions.h>

#define CA_TLM_PAGE_SIZE  (64)                                   //!< �R�}���h�e�[�u����1�e�����g���p�P�b�g(=1�y�[�W)�Ɋi�[�����R�}���h���i�y�[�W�l�[�V�����p�j
#define CA_TLM_PAGE_MAX   (24)                                   //!< �R�}���h�e�[�u���y�[�W���i�y�[�W�l�[�V�����p�j
#define CA_MAX_CMDS       (CA_TLM_PAGE_SIZE * CA_TLM_PAGE_MAX)   //!< �R�}���h�e�[�u���T�C�Y�D���Ȃ킿�o�^�ł���ő�R�}���h��

#include <src_user/Settings/CmdTlm/command_analyze_params.h>

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
 * @enum  CA_CMD_PARAM_LEN_TYPE
 * @brief �R�}���h�̃p�����^���f�[�^�̎��
 * @note  uint8_t��z��
 */
typedef enum
{
  CA_CMD_PARAM_LEN_TYPE_FIXED,        //!< �p�����^�Œ蒷
  CA_CMD_PARAM_LEN_TYPE_LOWER_LIMIT,  //!< �p�����^���̍ŏ��l�w��iraw param���܂܂�鎞�j
  CA_CMD_PARAM_LEN_TYPE_UNKNOWN       //!< �s��
} CA_CMD_PARAM_LEN_TYPE;

/**
 * @struct CA_CmdInfo
 * @brief  �R�}���h�e�[�u���̗v�f�ƂȂ�\����
 */
typedef struct
{
  CCP_EXEC_STS (*cmd_func)(const CTCP*);  //!< �R�}���h�ƂȂ�֐�
  CA_CMD_PARAM_LEN_TYPE param_len_type;   //!< �p�����^���̎��
  uint16_t param_len;                     //!< �p�����^��
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
 * @brief  �R�}���h�p�����^�����`�F�b�N����
 * @param  cmd_code:  �`�F�b�N����R�}���h��ID
 * @param  param_len: �p�����^��
 * @return CA_ACK
 */
CA_ACK CA_ckeck_cmd_param_len(CMD_CODE cmd_code, uint16_t param_len);

/**
 * @brief  Cmd Table�̃��[�h
 * @note   ��`�� /src_user/CmdTlm/CommandDefinitions.c �ɂ���
 * @param  cmd_table: Cmd Table�̎���
 * @return void
 */
void CA_load_cmd_table(CA_CmdInfo cmd_table[CA_MAX_CMDS]);

CCP_EXEC_STS Cmd_CA_REGISTER_CMD(const CTCP* packet);

CCP_EXEC_STS Cmd_CA_SET_PAGE_FOR_TLM(const CTCP* packet);

#endif
