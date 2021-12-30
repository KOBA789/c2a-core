/**
 * @file
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
 * @note   �������s���ȂƂ��C packet �� NOP RTC ��Ԃ�
 * @param[in,out] packet: CTCP
 * @param[in]     packet: CMD_CODE
 * @param[in]     *param: �p�����^
 * @param[in]     len:    �p�����^��
 * @return CTCP_UTIL_ACK
 */
CTCP_UTIL_ACK CCP_form_rtc(CTCP* packet, CMD_CODE cmd_id, const uint8_t* param, uint16_t len);

/**
 * @brief  Timeline command �𐶐�
 * @note   �������s���ȂƂ��C packet �� NOP TLC ��Ԃ�
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
 * @note   �������s���ȂƂ��C packet �� NOP RTC ��Ԃ�
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

/**
 * @brief  CCP packet ����C�T�C�Y�� 1 byte �̃R�}���h�������擾����
 * @note   �Z�O�����e�[�V�����ᔽ�̏ꍇ�́C 0 ��������ꂽ�|�C���^��Ԃ�
 * @note   CCP_get_param_from_packet �}�N�� �Ŏg�����Ƃ�z�肵�C���[�U�[�͂��̊֐��𒼐ڒ@�����Ƃ͑z�肵�Ă��Ȃ�
 * @param[in] packet: �擾���� packet
 * @param[in] n: N�Ԗڂ̈��� �i0�N�Z�j
 * @return �p�����^��ێ������|�C���^�i�^�͈Ӗ��������ĂȂ��j
 */
uint8_t* CCP_get_1byte_param_from_packet(const CTCP* packet, uint8_t n);

/**
 * @brief  CCP packet ����C�T�C�Y�� 2 byte �̃R�}���h�������擾����
 * @note   �Z�O�����e�[�V�����ᔽ�̏ꍇ�́C 0 ��������ꂽ�|�C���^��Ԃ�
 * @note   CCP_get_param_from_packet �}�N�� �Ŏg�����Ƃ�z�肵�C���[�U�[�͂��̊֐��𒼐ڒ@�����Ƃ͑z�肵�Ă��Ȃ�
 * @param[in] packet: �擾���� packet
 * @param[in] n: N�Ԗڂ̈��� �i0�N�Z�j
 * @return �p�����^��ێ������|�C���^�i�^�͈Ӗ��������ĂȂ��j
 */
uint16_t* CCP_get_2byte_param_from_packet(const CTCP* packet, uint8_t n);

/**
 * @brief  CCP packet ����C�T�C�Y�� 4 byte �̃R�}���h�������擾����
 * @note   �Z�O�����e�[�V�����ᔽ�̏ꍇ�́C 0 ��������ꂽ�|�C���^��Ԃ�
 * @note   CCP_get_param_from_packet �}�N�� �Ŏg�����Ƃ�z�肵�C���[�U�[�͂��̊֐��𒼐ڒ@�����Ƃ͑z�肵�Ă��Ȃ�
 * @param[in] packet: �擾���� packet
 * @param[in] n: N�Ԗڂ̈��� �i0�N�Z�j
 * @return �p�����^��ێ������|�C���^�i�^�͈Ӗ��������ĂȂ��j
 */
uint32_t* CCP_get_4byte_param_from_packet(const CTCP* packet, uint8_t n);

/**
 * @brief  CCP packet ����C�T�C�Y�� 8 byte �̃R�}���h�������擾����
 * @note   �Z�O�����e�[�V�����ᔽ�̏ꍇ�́C 0 ��������ꂽ�|�C���^��Ԃ�
 * @note   CCP_get_param_from_packet �}�N�� �Ŏg�����Ƃ�z�肵�C���[�U�[�͂��̊֐��𒼐ڒ@�����Ƃ͑z�肵�Ă��Ȃ�
 * @param[in] packet: �擾���� packet
 * @param[in] n: N�Ԗڂ̈��� �i0�N�Z�j
 * @return �p�����^��ێ������|�C���^�i�^�͈Ӗ��������ĂȂ��j
 */
uint64_t* CCP_get_8byte_param_from_packet(const CTCP* packet, uint8_t n);

/**
 * @brief  CCP packet ����CRAW �R�}���h�������擾����
 * @note   RAW �p�����^�����݂��Ȃ��ꍇ�́C 0 ��Ԃ�
 * @param[in]  packet: �擾���� packet
 * @param[out] dest: RAW �p�����^�R�s�[��
 * @param[in]  max_copy_len : �R�s�[����ő咷�D 0 �̏ꍇ�C������
 * @return �R�s�[��������
 */
uint16_t CCP_get_raw_param_from_packet(const CTCP* packet, void* dest, uint16_t max_copy_len);

/**
 * @def    CCP_get_param_from_packet(packet, n, type)
 * @brief  CCP packet ����Cn�Ԗڂ̃R�}���h�������擾����
 * @note   �Z�O�����e�[�V�����ᔽ�̏ꍇ�́C 0 ��Ԃ�
 * @note   �֐��ł͂Ȃ��}�N���ł��邱�Ƃɒ��ӂ���
 * @param[in] packet: �擾���� packet
 * @param[in] n: N�Ԗڂ̈��� �i0�N�Z�j
 * @param[in] type: �擾����p�����^�̌^�D���[�U�[��`�^�ł�OK�����C enum �͕s��
 * @return �p�����^�i�^�͈Ӗ��������ĂȂ��j
 * @note   �g�p��
 *         uint32_t param0 = CCP_get_param_from_packet(packet, 0, uint32_t);
 *         HOGE_ENUM param1 = (HOGE_ENUM)CCP_get_param_from_packet(packet, 1, uint8_t);
 */
#define CCP_get_param_from_packet(packet, n, type) \
(*((type*)( \
sizeof(type) == 1 ? \
(void*)CCP_get_1byte_param_from_packet(packet, n) : \
sizeof(type) == 2 ? \
(void*)CCP_get_2byte_param_from_packet(packet, n) : \
sizeof(type) == 4 ? \
(void*)CCP_get_4byte_param_from_packet(packet, n) : \
sizeof(type) == 8 ? \
(void*)CCP_get_4byte_param_from_packet(packet, n) : \
0)))

#endif
