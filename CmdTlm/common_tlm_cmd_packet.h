/**
 * @file   common_tlm_cmd_packet.h
 * @brief  CTCP �̐錾
 * @note   CTCP: Common Tlm Cmd Packet
 * @note   CCP:  Common Cmd Packet
 * @note   CTP:  Common Tlm Packet
 */
#ifndef COMMON_TLM_CMD_PACKET_H_
#define COMMON_TLM_CMD_PACKET_H_

#include <stddef.h> // for size_t
#include "../System/TimeManager/obc_time.h"
#include <src_user/CmdTlm/command_definitions.h>
#include <src_user/Library/stdint.h>
#include <src_user/Applications/app_registry.h>

#include <src_user/Settings/CmdTlm/common_tlm_cmd_packet_define.h>
#include <src_user/Settings/CmdTlm/common_tlm_cmd_packet_params.h>

/**
 * @enum   CCP_EXEC_STS
 * @brief  �R�}���h���s���ʃR�[�h
 * @note   int��z��
 */
typedef enum
{
  CCP_EXEC_PACKET_FMT_ERR = -6,     //!< packet handler �ŗp����
  CCP_EXEC_ROUTING_FAILED = -5,     //!< command router �ŗp����
  CCP_EXEC_CMD_NOT_DEFINED = -4,    //!< cmdExec �ŗp����
  CCP_EXEC_ILLEGAL_CONTEXT = -3,    //!< �R�}���h���s���̂��̑��̃G���[
  CCP_EXEC_ILLEGAL_PARAMETER = -2,  //!< �R�}���h���s���̃p�����^�G���[
  CCP_EXEC_ILLEGAL_LENGTH = -1,     //!< �R�}���h���s���̃R�}���h�������G���[
  CCP_EXEC_SUCCESS = 0,             /*!< �R�}���h���s����
                                         �Ȃ��C�R�}���h�̎��s�̌��ʂȂ̂ŁC���̌�̏��������s���Ă��C�R�}���h�����s����Ă���΂����Ԃ� */
  CCP_EXEC_UNKNOWN                  //!< ���������p�D�g��Ȃ��D
} CCP_EXEC_STS;


#include "block_command_table.h"      // CTCP, CCP_EXEC_STS �̒�`��肠�Ƃ� include


/**
 * @enum   CTCP_TC_DSC (Tlm Cmd Discrimination)
 * @brief  tlm or cmd ���K��
 * @note   uint8_t��z��
 */
typedef enum
{
  CTCP_TC_DSC_CMD,
  CTCP_TC_DSC_TLM,
  CTCP_TC_DSC_UNKNOWN
} CTCP_TC_DSC;

/**
 * @enum   CCP_EXEC_TYPE
 * @brief  �R�}���h���s���
 * @note   0x0*��z��i���4bit�͑���C2A�𓋍ڂ����{�[�h�p�ɓ]�����邽�߂Ɏg�����߁j
 *         ���̒�`��TCP_DEST_TYPE�ɂ���
 * @note   TCP_CMD_EXEC_TYPE �� CCP_EXEC_TYPE �̒�`�͈�v (���m�ɂ́A�O�҂͌�҂Ɋ܂܂�Ă���)
 *         CCP_EXEC_TYPE CCP_get_exec_type(const CTCP* packet) ���Q��
 * @note   GS�����OBC.TL��CC2A����CCP_form_tlc�͂��ׂ�TL0�����ɂȂ�
 */
typedef enum
{
// GS : Ground Station Command
// TL : Timeline Command
// MC : Macro Command, Block Line Command�Ƃ�����
// RT : Realtime Command
  CCP_EXEC_TYPE_GS,
  CCP_EXEC_TYPE_TL0,
  CCP_EXEC_TYPE_MC,
  CCP_EXEC_TYPE_RT,
  CCP_EXEC_TYPE_TL1,
  CCP_EXEC_TYPE_TL2,
  CCP_EXEC_TYPE_UNKNOWN
} CCP_EXEC_TYPE;

/**
 * @enum   CTP_DEST_FLAG
 * @brief  �z����w��D�e�����̃L���[���w�肷��t���O�D
 * @note   4bit�̃t���O�D�d���ł���悤�ɁCbit���Ƃ̃t���O�Ƃ���
 */
typedef enum
{
  CTP_DEST_FLAG_HK = 0x01, // 00000001b: Housekeeping Telemetry
  CTP_DEST_FLAG_MS = 0x02, // 00000010b: Mission Telemetry
  CTP_DEST_FLAG_ST = 0x04, // 00000100b: Stored Telemetry
  CTP_DEST_FLAG_RP = 0x08, // 00001000b: Replay Telemetry
  CTP_DEST_FLAG_UNKNOWN
} CTP_DEST_FLAG;


// ********************
//  CTCP getter/setter
// ********************

/**
 * @brief  tlm �� cmd ���𔻒f
 * @param  packet: CTCP packet
 * @return CTCP_TC_DSC
 */
CTCP_TC_DSC CTCP_get_tc_dsc(const CTCP* packet);

/**
 * @brief  CTCP ���R�s�[
 * @note   user�������悾���C�������̂��߂����ׂĂł͂Ȃ��K�v�Œ�������R�s�[���Ȃ��ꍇ������
 * @param[in,out] dest: �R�s�[�� CTCP
 * @param[in]     src:  �R�s�[�� CTCP
 * @return void
 */
void CTCP_copy_packet(CTCP* dest, const CTCP* src);


// *******************
//  CCP getter/setter
// *******************

/**
 * @brief  TI ���擾
 * @param  packet: CTCP packet
 * @return ti
 */
cycle_t CCP_get_ti(const CTCP* packet);

/**
 * @brief  TI ��ݒ�
 * @param[in,out] packet: CTCP
 * @param[in]     ti: TI
 * @return void
 */
void CCP_set_ti(CTCP* packet, cycle_t ti);

/**
 * @brief  ���� (TCP�ł�APID) ���擾
 * @param  packet: CTCP packet
 * @return CTCP_DEST_ID
 */
CTCP_DEST_ID CCP_get_dest_id(const CTCP* packet);

/**
 * @brief  ���� (TCP�ł�APID) ��ݒ�
 * @param[in,out] packet: CTCP
 * @param[in]     id: ����
 * @return void
 */
void CCP_set_dest_id(CTCP* packet, CTCP_DEST_ID id);

/**
 * @brief  EXEC_TYPE ���擾
 * @param  packet: CTCP packet
 * @return CCP_EXEC_TYPE
 */
CCP_EXEC_TYPE CCP_get_exec_type(const CTCP* packet);

/**
 * @brief  EXEC_TYPE ��ݒ�
 * @param[in,out] packet: CTCP
 * @param[in]     type: EXEC_TYPE
 * @return void
 */
void CCP_set_exec_type(CTCP* packet, CCP_EXEC_TYPE type);

/**
 * @brief  DEST_TYPE ���擾
 * @param  packet: CTCP packet
 * @return CCP_DEST_TYPE
 * @note   2021/01/22�� CCP_EXEC_TYPE �̏��4bit��q�؂���`�Œǉ�����
 */
CCP_DEST_TYPE CCP_get_dest_type(const CTCP* packet);

/**
 * @brief  DEST_TYPE ��ݒ�
 * @param[in,out] packet: CTCP
 * @param[in]     type: DEST_TYPE
 * @return void
 * @note   2021/01/22�� CCP_EXEC_TYPE �̏��4bit��q�؂���`�Œǉ�����
 */
void CCP_set_dest_type(CTCP* packet, CCP_DEST_TYPE type);

/**
 * @brief  CMD ID ���擾
 * @param  packet: CTCP packet
 * @return CMD_CODE
 */
CMD_CODE CCP_get_id(const CTCP* packet);

/**
 * @brief  CMD ID ��ݒ�
 * @param[in,out] packet: CTCP
 * @param[in]     id: CMD_CODE
 * @return void
 */
void CCP_set_id(CTCP* packet, CMD_CODE id);

/**
 * @brief  CTCP (CCP) �̃p�P�b�g�����擾
 * @param  packet: CTCP packet
 * @return packet len
 */
uint16_t CCP_get_packet_len(const CTCP* packet);

/**
 * @brief  CMD �p�����^�����擾
 * @param  packet: CTCP packet
 * @return param len
 */
uint16_t CCP_get_param_len(const CTCP* packet);

/**
 * @brief  CMD �p�����^���̍ő�l���擾
 * @return max param len
 */
uint16_t CCP_get_max_param_len(void);

/**
 * @brief  �擪�̃p�����^�̃|�C���^���擾
 * @param  packet: CTCP packet
 * @return *param_head
 */
const uint8_t* CCP_get_param_head(const CTCP* packet);

/**
 * @brief  �p�����^��ݒ�
 * @param[in,out] packet: CTCP
 * @param[in]     param: �p�����^�z��̃|�C���^
 * @param[in]     len:   �p�����^��
 * @return void
 */
void CCP_set_param(CTCP* packet, const uint8_t* param, uint16_t len);

/**
 * @brief  ���ʕ����� Header ���\�z
 * @param[in,out] packet: CTCP
 * @return void
 */
void CCP_set_common_hdr(CTCP* packet);


// *******************
//  CTP getter/setter
// *******************

/**
 * @brief  CTP_DEST_FLAG ���擾
 * @param  packet: CTCP packet
 * @return CTP_DEST_FLAG
 */
CTP_DEST_FLAG CTP_get_dest_flag(const CTCP* packet);

/**
 * @brief  CTP_DEST_FLAG ��ݒ�
 * @param[in,out] packet: CTCP
 * @param[in]     flag: CTP_DEST_FLAG
 * @return void
 */
void CTP_set_dest_flag(CTCP* packet, CTP_DEST_FLAG flag);

/**
 * @brief  ADU�����Ȃ��̏ꍇ��User Data�擪�̃|�C���^���擾
 * @param  packet: CTCP packet
 * @note   TCP�̏ꍇ�CADU�����Ȃ����̃|�C���^���擾
 * @note   ���̊֐��Ŏ擾�����|�C���^�o�R��tlm�̃f�[�^���Z�b�g���鎖�����邽�߁Cconst�����Ă��Ȃ�
 */
uint8_t* CTP_get_user_data_head(CTCP* packet);

#endif
