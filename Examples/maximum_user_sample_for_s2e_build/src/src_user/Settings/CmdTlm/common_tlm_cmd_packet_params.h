/**
 * @file   common_tlm_cmd_packet_params.h
 * @brief  CTCP �p�����^�̃I�[�o�[���C�h�p�w�b�_�[
 */
#ifndef COMMON_TLM_CMD_PACKET_PARAMS_H_
#define COMMON_TLM_CMD_PACKET_PARAMS_H_

#include "../../CmdTlm/Ccsds/TCPacket.h"

#undef CTCP_MY_DST_ID

#define CTCP_MY_DST_ID (TCP_APID_MOBC_CMD)      //!< ��������CMD������Destination ID���`

#endif
