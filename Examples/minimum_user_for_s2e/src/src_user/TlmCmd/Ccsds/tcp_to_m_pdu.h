/**
 * @file
 * @brief PacketList ���� TCPacket ������Ă��Ă���𑗐M�\�� M_PDU �ւƕϊ�����
 */
#ifndef TCP_TO_M_PDU_H_
#define TCP_TO_M_PDU_H_

#include <stddef.h> // for size_t

#include <src_core/TlmCmd/packet_list.h>
#include "m_pdu.h"
#include <src_core/System/TimeManager/time_manager.h>

/**
 * @enum  T2M_ACK
 * @brief T2M �֐��̕Ԃ�l
 */
typedef enum
{
  T2M_SUCCESS,
  T2M_NO_DATA_TO_SEND,
  T2M_INVALID_M_PDU,
  TSM_UNKNOWN
} T2M_ACK;

/**
 * @struct TcpToMPdu
 * @brief  TCPacket �� M_PDU �ɕϊ�����Ƃ��ɕK�v�ȃp�����[�^
 */
typedef struct
{
  size_t tcp_rp;
  size_t m_pdu_wp;
  uint8_t fhp_valid;
  cycle_t flush_interval;
  cycle_t last_updated;
} TcpToMPdu;

/**
 * @brief T2M �\���̂̏�����
 * @param[out] tcp_to_m_pdu: ���������� T2M
 * @return void
 */
void T2M_initialize(TcpToMPdu* tcp_to_m_pdu);

/**
 * @brief TCPacket �� M_PDU �ɕϊ�����
 * @param[in] pl: TCPacket ���擾���� Packet List
 * @param[in] tcp_to_m_pdu: �ϊ����鎞�̃p�����[�^�Q
 * @param[out] m_pdu: ��������� M_PDU
 * @return T2M_ACK
 */
T2M_ACK T2M_form_m_pdu(TcpToMPdu* tcp_to_m_pdu, PacketList* pl, M_PDU* m_pdu);

#endif
