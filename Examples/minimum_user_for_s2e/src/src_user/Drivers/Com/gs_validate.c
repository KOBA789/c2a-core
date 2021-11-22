#pragma section REPRO
/**
 * @file gs_validate.c
 * @brief ��M���� TC Frame �̌��؂��s��
 */

#include "gs_validate.h"

#define GS_POSITIVE_WINDOW_WIDTH_DEFAULT (64) // FIXME: �v����

// �ȉ����؊֐�. ���O�ʂ�
static GS_VALIDATE_ERR GS_check_tcf_header_(const TCF* tc_frame);
static GS_VALIDATE_ERR GS_check_tcf_contents_(const TCF* tc_frame);
static GS_VALIDATE_ERR GS_check_tcs_headers_(const TCS* tc_segment);
static GS_VALIDATE_ERR GS_check_tcp_headers_(const TCP* tc_packet);
static GS_VALIDATE_ERR GS_check_fecw_(const uint8_t* data, size_t len);

static GS_VALIDATE_ERR GS_check_ad_cmd_(const TCF* tc_frame);
static GS_VALIDATE_ERR GS_check_bc_cmd_(const TCF* tc_frame);
static GS_VALIDATE_ERR GS_check_bd_cmd_(const TCF* tc_frame);

static GS_ValiateInfo gs_validate_info_;
const GS_ValiateInfo* const gs_validate_info = &gs_validate_info_;

void GS_validate_init(void)
{
  gs_validate_info_.lockout_flag = 0;
  gs_validate_info_.type_a_counter = 0;
  gs_validate_info_.type_b_counter = 0;
  gs_validate_info_.retransmit_flag = 0;
  gs_validate_info_.positive_window_width = GS_POSITIVE_WINDOW_WIDTH_DEFAULT;
}

GS_VALIDATE_ERR GS_validate_tc_frame(const TCF* tc_frame)
{
  GS_VALIDATE_ERR ret;

  size_t frame_length = TCF_get_frame_len(tc_frame);

  // TODO WINGS�����܂��Ή����ĂȂ��̂ň�U�R�����g�A�E�g
  GS_check_fecw_((const uint8_t*)tc_frame, frame_length);

  ret = GS_check_tcf_header_(tc_frame);
  if (ret != GS_VALIDATE_ERR_OK) return ret;

  switch (TCF_get_type(tc_frame))
  {
  case TCF_TYPE_AD:
    ret = GS_check_ad_cmd_(tc_frame);
    break;

  case TCF_TYPE_BC:
    ret = GS_check_bc_cmd_(tc_frame);
    break;

  case TCF_TYPE_BD:
    ret = GS_check_bd_cmd_(tc_frame);
    break;

  default:
    return GS_VALIDATE_ERR_TCF_TYPE;
  }

  return ret;
}

static GS_VALIDATE_ERR GS_check_tcf_header_(const TCF* tc_frame)
{
  if (TCF_get_ver(tc_frame) != TCF_VER_1) return GS_VALIDATE_ERR_TCF_VER;
  // if (TCF_get_scid(tc_frame) != TCF_SCID_SAMPLE_SATELLITE) return GS_VALIDATE_ERR_TCF_SCID;    // FIXME: �e�X�g�p�Ɉ�U�R�����g�A�E�g
  if (TCF_get_vcid(tc_frame) != TCF_VCID_REALTIME) return GS_VALIDATE_ERR_TCF_VCID;

  return GS_VALIDATE_ERR_OK;
}

static GS_VALIDATE_ERR GS_check_tcf_contents_(const TCF* tc_frame)
{
  GS_VALIDATE_ERR ack;

  // TCSegment Header�̌Œ�l�������Ó����m�F����
  ack = GS_check_tcs_headers_(&tc_frame->tcs);
  if (ack != GS_VALIDATE_ERR_OK) return ack;

  // TCPacekt�̃w�b�_�̂���TLM/CMD���ʕ������Ó����m�F����
  ack = GS_check_tcp_headers_(&tc_frame->tcs.tcp);
  if (ack != GS_VALIDATE_ERR_OK) return ack;

  // TCPacket��Packte Type��Command���ǂ����m�F����
  if (TCP_get_type(&tc_frame->tcs.tcp) != TCP_TYPE_CMD)
  {
    return GS_VALIDATE_ERR_TCP_TYPE_IS_NOT_CMD;
  }

  // TCPacket��Sequence Flag���P�p�P�b�g���m�F����
  if (TCP_get_seq_flag(&tc_frame->tcs.tcp) != TCP_SEQ_SINGLE)
  {
    return GS_VALIDATE_ERR_TCP_SEQ_IS_NOT_SINGLE;
  }

  return GS_VALIDATE_ERR_OK;
}

static GS_VALIDATE_ERR GS_check_tcs_headers_(const TCS* tc_segment)
{
  if (TCS_get_seq_flag(tc_segment) != TCS_SEQ_SINGLE)
  {
    return GS_VALIDATE_ERR_TCS_SEQ_FLAG;
  }

  // FIXME: �ȉ��v�C��? (20210714)
  // �����ł�NORMAL�ȊO��MAP_ID���T�|�[�g���Ȃ� (000010b)
  // GSTOS�������M����Segment��MAP_ID��0x08�ƂȂ��Ă���
  // �v�C��:�΍􂪊�������܂Ŏb��I�ɔ��菈���𖳌���

  return GS_VALIDATE_ERR_OK;
}

static GS_VALIDATE_ERR GS_check_tcp_headers_(const TCP* tc_packet)
{
  TCP_APID apid;

  if (TCP_get_ver(tc_packet) != TCP_VER_1) return GS_VALIDATE_ERR_TCP_VER;
  if (TCP_get_2nd_hdr_flag(tc_packet) != TCP_2ND_HDR_PRESENT)
  {
    // �����ł�Secondary Header���K�{�B
    return GS_VALIDATE_ERR_TCP_2ND_HDR_FLAG;
  }

  apid = TCP_get_apid(tc_packet);
  if ( !( apid == TCP_APID_MOBC_CMD
       || apid == TCP_APID_AOBC_CMD
       || apid == TCP_APID_TOBC_CMD ) )
  {
    return GS_VALIDATE_ERR_TCP_APID;
  }

  return GS_VALIDATE_ERR_OK;
}

static GS_VALIDATE_ERR GS_check_fecw_(const uint8_t* data, size_t len)
{
  int i, j;
  uint16_t shift_reg = 0xffff; // �����l�͑Sbit��1
  uint16_t xor_tap = 0x1021; // LSB�͏펞0��XOR���Ƃ��Ă���ƍl����B

  // �f�[�^���������[�v
  for (i = 0; i < len; ++i)
  {
    // MSB�ʒu��shift_reg�Ƒ����邽��8bit���V�t�g
    uint16_t tmp = (uint16_t)(data[i] << 8);

    // �r�b�g���������[�v
    for (j = 0; j < 8; ++j)
    {
      // MSB���m��XOR���r
      if ((shift_reg ^ tmp) & 0x8000)
      {
        // ���ʂ�1�̏ꍇ�̓V�t�g+XOR
        shift_reg <<= 1;
        shift_reg ^= xor_tap;
      }
      else
      {
        // ���ʂ�0�̏ꍇ�̓V�t�g�̂�
        shift_reg <<= 1;
      }

      // ���r�b�g�]���̂��߃V�t�g
      tmp <<= 1;
    }
  }

  // �f�[�^+FECW������Ȃ猋�ʂ�0�ƂȂ�
  if (shift_reg != 0) return GS_VALIDATE_ERR_FECW_MISSMATCH;

  return GS_VALIDATE_ERR_OK;
}

static GS_VALIDATE_ERR GS_check_ad_cmd_(const TCF* tc_frame)
{
  GS_VALIDATE_ERR ack;
  int seq_diff;

  if (gs_validate_info_.lockout_flag) return GS_VALIDATE_ERR_IN_LOCKOUT;

  ack = GS_check_tcf_contents_(tc_frame);
  if (ack != GS_VALIDATE_ERR_OK) return ack;

  seq_diff = TCF_get_frame_seq_num(tc_frame) - gs_validate_info_.type_a_counter;

  if (seq_diff < 0)
  {
    // Sequence Counter�̒l��mod-256�Ȃ̂�seq_diff�̒l�����̏ꍇ��
    // 256�𑫂��Ēl�͈̔͂�[1, 256]�ɕϊ�
    seq_diff += 256;
  }

  if (seq_diff == 0)
  {
    // seq_diff��0�A���Ȃ킿N(R) == V(R)�Ȃ琳���M
    // �đ��v���t���O�̃N���A�ƃV�[�P���X���̃C���N�������g
    gs_validate_info_.retransmit_flag = 0;
    ++gs_validate_info_.type_a_counter;
  }
  else
  {
    if (seq_diff < gs_validate_info_.positive_window_width)
    {
      gs_validate_info_.retransmit_flag = 1;
      return GS_VALIDATE_ERR_FARM1_POSITIVE_WINDOW_AREA;
    }
    else if (seq_diff < (256 - gs_validate_info_.positive_window_width))
    {
      gs_validate_info_.retransmit_flag = 1;
      return GS_VALIDATE_ERR_FARM1_LOCKOUT_AREA;
    }
    else
    {
      if (seq_diff == 255) return GS_VALIDATE_ERR_FARM1_SAME_NUMBER;
      else return GS_VALIDATE_ERR_FARM1_NEGATIVE_WINDOW_AREA;
    }
  }

  return GS_VALIDATE_ERR_OK;
}

static GS_VALIDATE_ERR GS_check_bc_cmd_(const TCF* tc_frame)
{
  // BC�R�}���h�̎�ʂ𔻒肵�A��������B
  // TCF�̍\����AD/BD�R�}���h�ɓ��������`�ƂȂ��Ă��邽�߁A
  // TCS��TCP�̃f�[�^�\����ǂݑւ��ď������s���Ă���B
  if (tc_frame->tcs.header[0] == TCF_BC_CMD_CODE_UNLOCK)
  {
    // Unlock�R�}���h�̏ꍇ��Lockout��Retransmit�t���O���N���A
    gs_validate_info_.lockout_flag = 0;
    gs_validate_info_.retransmit_flag = 0;

    // Type-B Coutner�̒l���X�V���ď����I��
    ++gs_validate_info_.type_b_counter;
  }
  else if ((tc_frame->tcs.header[0] == TCF_BC_CMD_CODE_SET_VR_0)
        && (tc_frame->tcs.tcp.packet[0] == TCF_BC_CMD_CODE_SET_VR_1))
  {
    // SET V(R)�R�}���h�̏ꍇ
    if (gs_validate_info_.lockout_flag == 0)
    {
      // Lockout��ԂłȂ��ꍇ��Type-A�J�E���^�̒l���w��l�ɐݒ肵
      // Retransmit�t���O���N���A
      gs_validate_info_.type_a_counter = tc_frame->tcs.tcp.packet[1];
      gs_validate_info_.retransmit_flag = 0;
    }

    // Type-B Coutner�̒l���X�V���ď����I��
    // Lockout��Ԃł�Type-B Counter�̒l�͍X�V����
    ++gs_validate_info_.type_b_counter;
  }
  else
  {
    // ��L�ȊO�̏ꍇ�͕s���Ɣ��f���ُ�I��
    return GS_VALIDATE_ERR_INVALID_BC_CMD;
  }

  return GS_VALIDATE_ERR_OK;
}

static GS_VALIDATE_ERR GS_check_bd_cmd_(const TCF* tc_frame)
{
  GS_VALIDATE_ERR ack;
  ack = GS_check_tcf_contents_(tc_frame);
  if (ack != GS_VALIDATE_ERR_OK) return ack;

  ++gs_validate_info_.type_b_counter;

  return GS_VALIDATE_ERR_OK;
}

uint32_t GS_form_clcw(void)
{
  // �l��0�Œ�̍��ڂ͕ϐ��̏������ő�ւ������I�ɂ͎w�肵�Ȃ�
  uint32_t clcw = 0;
  uint32_t val;

  // [FIXME] TRP���ł����炱���������D�i2021/01/17�j
  /*
  // XTRP-A Carrier Lock Status
  if (xtrp1->xtrp_rx_sts.act_monitor.bit.career_lock == 1)
  {
    // Carrier Lock On�̏ꍇ�̓t���O�ݒ�
    clcw |= 0x08000000; // **** 1*** **** **** **** **** **** ****
  }
  */

  // COP in Effect -> COP-1
  clcw |= 0x01000000; // **** **01 **** **** **** **** **** ****

  // [FIXME] TRP���ł����炱���������D�i2021/01/17�j
  /*
  // Sub-Carrier Lock + Rx Bit Rate
  if (xtrp1->xtrp_rx_sts.act_monitor.bit.sub_career_lock == 1)
  {
    // Sub-carrier Lock On�̏ꍇ�̓r�b�g���[�g�ʂ̃t���O�ݒ�
    if (xtrp1->xtrp_rx_sts.rx_bitrate == 0)
    {
      // Rx Bitrate 15.625bps (Low)
      clcw |= 0x00004000; // **** **** **** **** 01** **** **** ****
    }
    else if (xtrp1->xtrp_rx_sts.rx_bitrate <= 3)
    {
      // Rx Bitrate 125bps, 62.5bps, 31.25bps (Mid)
      clcw |= 0x00008000; // **** **** **** **** 10** **** **** ****
    }
    else
    {
      // Rx Bitrate 2kbps, 1kbps, 500bps, 250bps (High)
      clcw |= 0x0000c000; // **** **** **** **** 11** **** **** ****
    }
  }
  */

  // Lockout
  if (gs_validate_info_.lockout_flag)
  {
    clcw |= 0x00002000; // **** **** **** **** **1* **** **** ****
  }

  // Retransmit
  if (gs_validate_info_.retransmit_flag)
  {
    clcw |= 0x00000800; // **** **** **** **** **** 1*** **** ****
  }

  // FARM B Counter
  val = (gs_validate_info_.type_b_counter & 0x03) << 9;
  clcw |= val; // **** **** **** **** **** *xx* **** ****

  // Report Value
  clcw |= gs_validate_info_.type_a_counter; // **** **** **** **** **** **** xxxx xxxx

  return clcw;
}

GS_VALIDATE_ERR GS_set_farm_pw(uint8_t positive_window_width)
{
  if (positive_window_width < 1 || positive_window_width > 127)
  {
    return GS_VALIDATE_ERR_FARM1_POSITIVE_WINDOW_AREA;
  }

  gs_validate_info_.positive_window_width = positive_window_width;

  return GS_VALIDATE_ERR_OK;
}

#pragma section
