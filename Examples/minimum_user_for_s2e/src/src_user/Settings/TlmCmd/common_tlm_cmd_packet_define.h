/**
 * @file
 * @brief  CTCP�^�̒�`
 */
#ifndef COMMON_TLM_CMD_PACKET_DEFINE_H_
#define COMMON_TLM_CMD_PACKET_DEFINE_H_

#include "../../CmdTlm/Ccsds/TCPacket.h"

// �g�p����p�P�b�g�\���̂�CTCP�^�Ƃ��Ē�`����
typedef TCP CTCP;

// ����ID���`
typedef TCP_APID CTCP_DEST_ID;

// 2021-01-22 �ǉ�
// DEST_TYPE ���`
typedef TCP_CMD_DEST_TYPE CCP_DEST_TYPE;

// �e���R�}�p�P�b�g�̍ő咷���`
#define CTCP_MAX_LEN (TCP_MAX_LEN)

// ��������CMD������Destination ID���`
#define CTCP_MY_DST_ID (TCP_APID_MOBC_CMD)

#endif
