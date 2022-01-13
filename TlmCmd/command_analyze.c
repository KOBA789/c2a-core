#pragma section REPRO
/**
 * @file
 * @brief  �R�}���h�̎��s�E�o�^�C�R�}���h�֘A����
 */
#include "command_analyze.h"
#include "common_tlm_cmd_packet_util.h"
#include <src_user/TlmCmd/command_definitions.h>
#include "../Library/print.h"
 #include "../System/EventManager/event_logger.h"
#include <string.h>

/**
 * @enum   EH_EL_LOCAL_ID
 * @brief  EL_CORE_GROUP_COMMAND_ANALYZE �C�x���g�� local id
 * @note   uint8_t ��z��
 */
typedef enum
{
  CA_EL_LOCAL_ID_ILLEGAL_CMD_CODE = 0,     //!< �s���� CMD_CODE ���w�肳�ꂽ�ꍇ
  CA_EL_LOCAL_ID_NULL_CMD_CODE             //!< ���o�^�� CMD_CODE ���w�肳�ꂽ�ꍇ
} CA_EL_LOCAL_ID;

/**
 * @brief  �R�}���h�p�����^�̃T�C�Y�����擾
 * @param cmd_code: �R�}���hID
 * @param n: N�Ԗڂ̈��� �i0�N�Z�j
 * @return CA_PARAM_SIZE_TYPE
 * @note   �s���Ȉ����̏ꍇ�� CA_PARAM_SIZE_TYPE_NONE ��Ԃ�
 */
static CA_PARAM_SIZE_TYPE CA_get_param_size_type_(CMD_CODE cmd_code, uint8_t n);

static CommandAnalyze command_analyze_;
const CommandAnalyze* const command_analyze = &command_analyze_;


void CA_initialize(void)
{
  int i;

  if (CA_MAX_CMDS <= Cmd_CODE_MAX)
  {
    Printf("CA: init error!!!\n");
    return;
  }

  memset(&command_analyze_, 0x00, sizeof(CommandAnalyze));
  for (i = 0; i < CA_MAX_CMDS; ++i)
  {
    command_analyze_.cmd_table[i].cmd_func = NULL;
  }

  CA_load_cmd_table(command_analyze_.cmd_table);
}

CCP_EXEC_STS CA_execute_cmd(const CTCP* packet)
{
  CMD_CODE cmd_code = CCP_get_id(packet);
  CCP_EXEC_STS (*cmd_func)(const CTCP*) = NULL;

  if (cmd_code >= CA_MAX_CMDS)
  {
    EL_record_event((EL_GROUP)EL_CORE_GROUP_COMMAND_ANALYZE,
                    CA_EL_LOCAL_ID_ILLEGAL_CMD_CODE,
                    EL_ERROR_LEVEL_LOW,
                    (uint32_t)cmd_code);
    return CCP_EXEC_CMD_NOT_DEFINED;
  }

  cmd_func = command_analyze->cmd_table[cmd_code].cmd_func;

  if (cmd_func != NULL)
  {
    // �����ōŒ���̃p�����^���`�F�b�N�����邪�C bct_id_t �ȂǁC������`���g���Ă�����̂͊e�R�}���h�����ł��A�T�[�V�������邱��
    uint16_t param_len = CCP_get_param_len(packet);
    if (CA_ckeck_cmd_param_len(cmd_code, param_len) != CA_ACK_OK) return CCP_EXEC_ILLEGAL_LENGTH;

    return cmd_func(packet);
  }
  else
  {
    EL_record_event((EL_GROUP)EL_CORE_GROUP_COMMAND_ANALYZE,
                    CA_EL_LOCAL_ID_NULL_CMD_CODE,
                    EL_ERROR_LEVEL_LOW,
                    (uint32_t)cmd_code);
    return CCP_EXEC_CMD_NOT_DEFINED;
  }
}

uint8_t CA_get_cmd_param_num(CMD_CODE cmd_code)
{
  uint8_t num;
  if (cmd_code >= CA_MAX_CMDS) return 0;

  for (num = 0; num < CA_MAX_CMD_PARAM_NUM; ++num)
  {
    if (CA_get_param_size_type_(cmd_code, num) == CA_PARAM_SIZE_TYPE_NONE)
    {
      return num;
    }
  }
  return CA_MAX_CMD_PARAM_NUM;
}

uint8_t CA_get_cmd_param_size(CMD_CODE cmd_code, uint8_t n)
{
  switch (CA_get_param_size_type_(cmd_code, n))
  {
    case CA_PARAM_SIZE_TYPE_NONE:
    case CA_PARAM_SIZE_TYPE_RAW:
      return 0;
    case CA_PARAM_SIZE_TYPE_1BYTE:
      return 1;
      break;
    case CA_PARAM_SIZE_TYPE_2BYTE:
      return 2;
      break;
    case CA_PARAM_SIZE_TYPE_4BYTE:
      return 4;
      break;
    case CA_PARAM_SIZE_TYPE_8BYTE:
      return 8;
      break;
    default:
      return 0;     // �s��
  }
}

uint16_t CA_get_cmd_param_min_len(CMD_CODE cmd_code)
{
  uint8_t i;
  uint16_t min_len = 0;

  for (i = 0; i < CA_MAX_CMD_PARAM_NUM; ++i)
  {
    uint8_t param_size = CA_get_cmd_param_size(cmd_code, i);
    if (param_size == 0) return min_len;
    min_len += param_size;
  }
  return min_len;
}

CA_ACK CA_ckeck_cmd_param_len(CMD_CODE cmd_code, uint16_t param_len)
{
  if (cmd_code >= CA_MAX_CMDS) return CA_ACK_ERR;

  if (CA_has_raw_param(cmd_code))
  {
    if (param_len < CA_get_cmd_param_min_len(cmd_code) || param_len > CCP_get_max_param_len())
    {
      return CA_ACK_ERR;
    }
    return CA_ACK_OK;
  }
  else
  {
    return (CA_get_cmd_param_min_len(cmd_code) == param_len) ? CA_ACK_OK : CA_ACK_ERR;
  }
}

int CA_has_raw_param(CMD_CODE cmd_code)
{
  uint8_t param_num = CA_get_cmd_param_num(cmd_code);
  if (param_num == 0) return 0;

  return (CA_get_param_size_type_(cmd_code, (uint8_t)(param_num - 1)) == CA_PARAM_SIZE_TYPE_RAW) ? 1 : 0;
}

static CA_PARAM_SIZE_TYPE CA_get_param_size_type_(CMD_CODE cmd_code, uint8_t n)
{
  if (n >= CA_MAX_CMD_PARAM_NUM) return CA_PARAM_SIZE_TYPE_NONE;
  if (cmd_code >= CA_MAX_CMDS) return CA_PARAM_SIZE_TYPE_NONE;
  if (n % 2)
  {
    return (CA_PARAM_SIZE_TYPE)command_analyze->cmd_table[cmd_code].param_size_infos[n / 2].packed_info.bit.second;
  }
  else
  {
    return (CA_PARAM_SIZE_TYPE)command_analyze->cmd_table[cmd_code].param_size_infos[n / 2].packed_info.bit.first;
  }
}

CCP_EXEC_STS Cmd_CA_REGISTER_CMD(const CTCP* packet)
{
  uint8_t param_size_infos[(CA_MAX_CMD_PARAM_NUM + 1) / 2];
  CMD_CODE cmd_code = (CMD_CODE)CCP_get_param_from_packet(packet, 0, uint16_t);
  uint32_t cmd_func = CCP_get_param_from_packet(packet, 1, uint32_t);
  uint16_t ret;
  uint8_t i;

  // raw �p�����^�Ȃ̂ŁC�������`�F�b�N
  if (CCP_get_param_len(packet) != 6 + sizeof(param_size_infos)) return CCP_EXEC_ILLEGAL_LENGTH;

  ret = CCP_get_raw_param_from_packet(packet, param_size_infos, sizeof(param_size_infos));
  if (ret != sizeof(param_size_infos)) return CCP_EXEC_ILLEGAL_LENGTH;

  if (cmd_code >= CA_MAX_CMDS)
  {
    // �o�^�w��ʒu���R�}���h������𒴂��Ă���ꍇ�ُ͈픻��
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }

  // ���[���x���R�}���h�Ȃ̂ŁC�A�T�[�V�������Ȃ�
  command_analyze_.cmd_table[cmd_code].cmd_func = (CCP_EXEC_STS (*)(const CTCP*))cmd_func;
  for (i = 0; i < sizeof(param_size_infos); ++i)
  {
    command_analyze_.cmd_table[cmd_code].param_size_infos[i].packed_info.bit.first = ( param_size_infos[i] & 0xf0 ) >> 4;
    command_analyze_.cmd_table[cmd_code].param_size_infos[i].packed_info.bit.second = param_size_infos[i] & 0x0f;
  }

  return CCP_EXEC_SUCCESS;
}

CCP_EXEC_STS Cmd_CA_SET_PAGE_FOR_TLM(const CTCP* packet)
{
  uint8_t page = CCP_get_param_from_packet(packet, 0, uint8_t);

  if (page >= CA_TLM_PAGE_MAX)
  {
    // �y�[�W�ԍ����R�}���h�e�[�u���͈͊O
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }

  command_analyze_.tlm_page_no = page;
  return CCP_EXEC_SUCCESS;
}

#pragma section
