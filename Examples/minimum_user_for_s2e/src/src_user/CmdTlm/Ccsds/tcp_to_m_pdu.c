#pragma section REPRO
/**
 * @file  tcp_to_m_pdu.c
 * @brief PacketList ���� TCPacket ������Ă��Ă���𑗐M�\�� M_PDU �ւƕϊ�����
 */

#include "tcp_to_m_pdu.h"

void T2M_initialize(TcpToMPdu* tcp_to_m_pdu)
{
  // TC Packet Read Pointer�̒l��������
  tcp_to_m_pdu->tcp_rp = 0;
  tcp_to_m_pdu->m_pdu_wp = 0;
  tcp_to_m_pdu->fhp_valid = 0;
  // �������o�҂����Ԃ̏����l��10�b
  // 32kbps�Ȃ�8VCDU/sec�̑��M�\��
  tcp_to_m_pdu->flush_interval = OBCT_sec2cycle(10);
  // �ŏI�X�V�����͌��ݎ����ɐݒ�
  tcp_to_m_pdu->last_updated = TMGR_get_master_total_cycle();

  return;
}

T2M_ACK T2M_form_m_pdu(TcpToMPdu* tcp_to_m_pdu, PacketList* pl, M_PDU* m_pdu)
{
  // M_PDU���������� or TC Packet���Ȃ��Ȃ�܂Ŏ��{
  while (tcp_to_m_pdu->m_pdu_wp != M_PDU_DATA_SIZE)
  {
    const TCP* packet;
    size_t tcp_len, tcp_left, m_pdu_left, write_len;

    if (PL_is_empty(pl))
    {
      // �e�����g��Queue�ɑ��o���ׂ��p�P�b�g���Ȃ��ꍇ
      // �ŏI�X�V��������̌o�ߎ��Ԃ��Z�o
      cycle_t delta = TMGR_get_master_total_cycle() - tcp_to_m_pdu->last_updated;

      if (tcp_to_m_pdu->m_pdu_wp == 0)
      {
        // M_PDU��Write Pointer��0�ő��o�f�[�^���Ȃ��ꍇ
        // ���o�f�[�^�Ȃ��Ƃ��ď����ł��؂�
        return T2M_NO_DATA_TO_SEND;
      }

      if (delta < tcp_to_m_pdu->flush_interval)
      {
        // �o�ߎ��Ԃ��������o�Ԋu�ɒB���Ă��Ȃ��ꍇ
        // ������ł��؂�AM_PDU�������Ƃ���0��Ԃ�
        return T2M_INVALID_M_PDU;
      }
      else
      {
        // �������o�̂��߂ɖ��߂�ׂ��f�[�^�ʂ��v�Z���A
        // Fill Packet�𐶐��AQueue�ɒǉ��B
        //
        // M_PDU�̎c��̈悪Fill Packet�̃w�b�_���ȉ��̏ꍇ�A
        // ���[�U�f�[�^��1��Fill Packet�����������B
        // ���̏ꍇ�A�������ꂽFill Packet�͎�M_PDU�ɂ܂�����B
        // ���̏�ԂŒǉ��̃e�����g������������Ȃ��ꍇ�́AFill
        // Packet�݂̂ō\�����ꂽM_PDU����x���o����邱�ƂɂȂ�B
        static TCP fill_; // �T�C�Y���傫�����ߐÓI�m��(�X�^�b�N�ی�)
        size_t fill_size = M_PDU_DATA_SIZE - tcp_to_m_pdu->m_pdu_wp;
        TCP_TLM_setup_fill_packet(&fill_, (uint16_t)fill_size);
        PL_push_back(pl, &fill_);
      }
    }

    // Queue�擪��TC Packet���擾
    // �L���p�P�b�g�܂���Fill�p�P�b�g���K�������Ă���B
    packet = &(PL_get_head(pl)->packet);

    // �������ރf�[�^�����v�Z
    tcp_len = TCP_TLM_get_packet_len(packet);
    tcp_left = tcp_len - tcp_to_m_pdu->tcp_rp;
    m_pdu_left = M_PDU_DATA_SIZE - tcp_to_m_pdu->m_pdu_wp;
    write_len = (tcp_left > m_pdu_left) ? m_pdu_left : tcp_left;

    // First Header Pointer�����ݒ肩�������݊J�n���p�P�b�g�擪
    if ((tcp_to_m_pdu->fhp_valid != 1) && (tcp_to_m_pdu->tcp_rp == 0))
    {
      // First Header Pointer�����݂�Write Pointer�̒l�ɐݒ�
      M_PDU_set_1st_hdr_ptr(m_pdu, (uint16_t)tcp_to_m_pdu->m_pdu_wp);
      // First Header Pointer�ݒ�ς݂��L�^
      tcp_to_m_pdu->fhp_valid = 1;
    }

    // M_PDU�f�[�^�̖����Ƀf�[�^��ǉ�
    M_PDU_set_data(m_pdu,
                   &(packet->packet[tcp_to_m_pdu->tcp_rp]),
                   tcp_to_m_pdu->m_pdu_wp,
                   write_len);

    // TC Pacekt Read Pointer���X�V
    tcp_to_m_pdu->tcp_rp += write_len;
    // M_PDU Write Pointer���X�V
    tcp_to_m_pdu->m_pdu_wp += write_len;
    // �ŏI�������ݎ������X�V
    tcp_to_m_pdu->last_updated = TMGR_get_master_total_cycle();

    // TC Packet�S�̂�M_PDU�ɏ������ݏI������ꍇ
    if (tcp_to_m_pdu->tcp_rp == tcp_len)
    {
      // �������݊�������TC Packet��Queue����j����Read Poineter�̒l��������
      PL_drop_executed(pl);
      tcp_to_m_pdu->tcp_rp = 0;
    }
  }

  // M_PDU�𖄂ߏI������̂�First Header Pointer�����ݒ�̏ꍇ
  // -> �p�P�b�g�擪��M_PDU�̒��Ɋ܂܂�Ȃ������ꍇ
  if (tcp_to_m_pdu->fhp_valid != 1)
  {
    // First Header Pointer���u�w�b�_�Ȃ��v�ɐݒ�
    M_PDU_set_1st_hdr_ptr(m_pdu, M_PDU_PTR_NO_HDR);
    // First Header Pointer�ݒ�ς݂��L�^
    tcp_to_m_pdu->fhp_valid = 1;
  }

  // M_PDU Write Pointer�̒l���N���A
  tcp_to_m_pdu->m_pdu_wp = 0;
  // First Header Pointer��Ԃ𖢐ݒ�ɕύX
  tcp_to_m_pdu->fhp_valid = 0;

  return T2M_SUCCESS;
}

#pragma section
