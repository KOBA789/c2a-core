/**
 * @file   common_tlm_cmd_packet_util.h
 * @brief  CTCP �̔ėpUtil
 */
#ifndef COMMON_TLM_CMD_PACKET_UTIL_H_
#define COMMON_TLM_CMD_PACKET_UTIL_H_

#include "common_tlm_cmd_packet.h"

/**
 * @enum   CTCP_UTIL_ACK
 * @brief  CTCP Utility �̔ėp�Ԃ�l
 * @note   uint8_t ��z��
 */
typedef enum
{
  CTCP_UTIL_ACK_OK = 0,       //!< ����I��
  CTCP_UTIL_ACK_PARAM_ERR     //!< �p�����^�G���[
} CTCP_UTIL_ACK;

/**
 * @brief  App���s�R�}���h�𐶐�
 * @param[in,out] packet: CTCP
 * @param[in]     ti: TI
 * @param[in]     id: AR_APP_ID
 * @return void
 */
void CCP_form_app_cmd(CTCP* packet, cycle_t ti, AR_APP_ID id);

/**
 * @brief  Realtime command �𐶐�
 * @param[in,out] packet: CTCP
 * @param[in]     packet: CMD_CODE
 * @param[in]     *param: �p�����^
 * @param[in]     len:    �p�����^��
 * @return CTCP_UTIL_ACK
 */
CTCP_UTIL_ACK CCP_form_rtc(CTCP* packet, CMD_CODE cmd_id, const uint8_t* param, uint16_t len);

/**
 * @brief  Timeline command �𐶐�
 * @param[in,out] packet: CTCP
 * @param[in]     ti:     TI
 * @param[in]     packet: CMD_CODE
 * @param[in]     *param: �p�����^
 * @param[in]     len:    �p�����^��
 * @return CTCP_UTIL_ACK
 */
CTCP_UTIL_ACK CCP_form_tlc(CTCP* packet, cycle_t ti, CMD_CODE cmd_id, const uint8_t* param, uint16_t len);

/**
 * @brief  BC�W�J command �𐶐�
 * @param[in,out] packet: CTCP
 * @param[in]     tl_no: Timeline no
 * @param[in]     block_no: BC ID
 * @return CTCP_UTIL_ACK
 */
CTCP_UTIL_ACK CCP_form_block_deploy_cmd(CTCP* packet, uint8_t tl_no, bct_id_t block_no);

/**
 * @brief  Realtime Command ���� Timeline Command �֕ϊ�
 * @param[in,out] packet: �ϊ����� packet
 * @param[in]     ti:     TI
 * @return void
 */
void CCP_convert_rtc_to_tlc(CTCP* packet, cycle_t ti);

#endif
