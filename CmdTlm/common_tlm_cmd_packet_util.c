#pragma section REPRO
/**
 * @file   common_tlm_cmd_packet_util.c
 * @brief  CTCP �̔ėpUtil
 */

#include "common_tlm_cmd_packet_util.h"
#include "../Applications/timeline_command_dispatcher.h" // for TL_ID_MAX
#include "command_analyze.h"
#include "block_command_table.h" // for BCT_MAX_BLOCKS
#include <src_user/CmdTlm/command_definitions.h>  // for CMD_CODE
#include "../Library/endian_memcpy.h"
#include <stddef.h>     // for NULL


void CCP_form_app_cmd(CTCP* packet, cycle_t ti, AR_APP_ID id)
{
  // FIXME: ����4�͊��ˑ��Ȃ̂ŁC�ˑ����Ȃ��悤�ɒ���
  //        �K�؂ɒ������ƂŁC CCP_form_tlc �̕Ԃ�l���݂Ȃ��ėǂ��Ȃ�͂��D
  uint8_t param[4];
  size_t  id_temp = id;
  endian_memcpy(param, &id_temp, 4);

  CCP_form_tlc(packet, ti, Cmd_CODE_AM_EXECUTE_APP, param, 4);
}

CTCP_UTIL_ACK CCP_form_rtc(CTCP* packet, CMD_CODE cmd_id, const uint8_t* param, uint16_t len)
{
  if (param == NULL)
  {
    if (len != 0) return CTCP_UTIL_ACK_PARAM_ERR;
  }

  if (CA_ckeck_cmd_param_len(cmd_id, len) != CA_ACK_OK)
  {
    return CTCP_UTIL_ACK_PARAM_ERR;
  }

  CCP_set_common_hdr(packet);
  CCP_set_id(packet, cmd_id);
  CCP_set_exec_type(packet, CCP_EXEC_TYPE_RT);
  CCP_set_dest_type(packet, TCP_CMD_DEST_TYPE_TO_ME);
  CCP_set_ti(packet, 0); // RT�̏ꍇ�ATI��0�Œ�B
  CCP_set_param(packet, param, len);

  return CTCP_UTIL_ACK_OK;
}

CTCP_UTIL_ACK CCP_form_tlc(CTCP* packet, cycle_t ti, CMD_CODE cmd_id, const uint8_t* param, uint16_t len)
{
  if (param == NULL)
  {
    if (len != 0) return CTCP_UTIL_ACK_PARAM_ERR;
  }

  if (CA_ckeck_cmd_param_len(cmd_id, len) != CA_ACK_OK)
  {
    return CTCP_UTIL_ACK_PARAM_ERR;
  }

  CCP_set_common_hdr(packet);
  CCP_set_id(packet, cmd_id);
  CCP_set_exec_type(packet, CCP_EXEC_TYPE_TL0);
  CCP_set_dest_type(packet, TCP_CMD_DEST_TYPE_TO_ME);
  CCP_set_ti(packet, ti);
  CCP_set_param(packet, param, len);

  return CTCP_UTIL_ACK_OK;
}

CTCP_UTIL_ACK CCP_form_block_deploy_cmd(CTCP* packet, uint8_t tl_no, bct_id_t block_no)
{
  uint8_t param[1 + SIZE_OF_BCT_ID_T];

  if ((tl_no >= TL_ID_MAX) || (block_no >= BCT_MAX_BLOCKS))
  {
    // �^�C�����C����line�ԍ��A�u���b�N�R�}���h�ԍ����͈͊O�̏ꍇ�ُ픻��
    return CTCP_UTIL_ACK_PARAM_ERR;
  }

  param[0] = tl_no;
  endian_memcpy(&param[1], &block_no, SIZE_OF_BCT_ID_T);

  return CCP_form_rtc(packet, Cmd_CODE_TLCD_DEPLOY_BLOCK, param, 1 + SIZE_OF_BCT_ID_T);
}

void CCP_convert_rtc_to_tlc(CTCP* packet, cycle_t ti)
{
  CCP_set_exec_type(packet, CCP_EXEC_TYPE_TL0);
  CCP_set_ti(packet, ti);
}

uint8_t CCP_get_1byte_param_from_packet(const CTCP* packet, uint8_t n)
{
  return 0;
}

uint16_t CCP_get_2byte_param_from_packet(const CTCP* packet, uint8_t n)
{
  return 0;
}

uint32_t CCP_get_4byte_param_from_packet(const CTCP* packet, uint8_t n)
{
  return 0;
}

uint64_t CCP_get_8byte_param_from_packet(const CTCP* packet, uint8_t n)
{
  return 0;
}

#pragma section
