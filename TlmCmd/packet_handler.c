#pragma section REPRO
#include "packet_handler.h"

#include <stddef.h> // for NULL
#include <string.h> // for memcpy

#include "../System/TimeManager/time_manager.h"
#include "command_analyze.h"
#include "block_command_table.h"
#include <src_user/TlmCmd/user_packet_handler.h>

PacketList PH_gs_cmd_list;
PacketList PH_rt_cmd_list;
PacketList PH_tl_cmd_list[TL_ID_MAX];
PacketList PH_ms_tlm_list;
PacketList PH_st_tlm_list;
PacketList PH_rp_tlm_list;

static PL_Node PH_gs_cmd_stock_[PH_GS_CMD_LIST_MAX];
static PL_Node PH_rt_cmd_stock_[PH_RT_CMD_LIST_MAX];
static PL_Node PH_tl0_cmd_stock_[PH_TL0_CMD_LIST_MAX];
static PL_Node PH_tl1_cmd_stock_[PH_TL1_CMD_LIST_MAX];
static PL_Node PH_tl2_cmd_stock_[PH_TL2_CMD_LIST_MAX];
static PL_Node PH_ms_tlm_stock_[PH_MS_TLM_LIST_MAX];
static PL_Node PH_st_tlm_stock_[PH_ST_TLM_LIST_MAX];
static PL_Node PH_rp_tlm_stock_[PH_RP_TLM_LIST_MAX];

static PH_ACK PH_analyze_cmd_(const CTCP* packet);
static PH_ACK PH_analyze_block_cmd_(const CTCP* packet);
static PH_ACK PH_analyze_tlm_(const CTCP* packet);

static PH_ACK PH_add_gs_cmd_(const CTCP* packet);
static PH_ACK PH_add_rt_cmd_(const CTCP* packet);
static PH_ACK PH_add_tl_cmd_(int line_no,
                            const CTCP* packet,
                            cycle_t now);
/**
 * @brief UTL_cmd �� TL_cmd �ɕϊ����� tl_cmd_list �ɒǉ�����
 * @note TODO�Fconst cast �ł���������������
 * @param[in] packet
 * @return PH_ACK
 */
static PH_ACK PH_add_utl_cmd_(const CTCP* packet);
static PH_ACK PH_add_ms_tlm_(const CTCP* packet);
static PH_ACK PH_add_st_tlm_(const CTCP* packet);
static PH_ACK PH_add_rp_tlm_(const CTCP* packet);

void PH_init(void)
{
  PL_initialize(PH_gs_cmd_stock_, PH_GS_CMD_LIST_MAX, &PH_gs_cmd_list);
  PL_initialize(PH_rt_cmd_stock_, PH_RT_CMD_LIST_MAX, &PH_rt_cmd_list);

  PL_initialize(PH_tl0_cmd_stock_, PH_TL0_CMD_LIST_MAX, &PH_tl_cmd_list[0]);
  PL_initialize(PH_tl1_cmd_stock_, PH_TL1_CMD_LIST_MAX, &PH_tl_cmd_list[1]);
  PL_initialize(PH_tl2_cmd_stock_, PH_TL2_CMD_LIST_MAX, &PH_tl_cmd_list[2]);

  PL_initialize(PH_ms_tlm_stock_, PH_MS_TLM_LIST_MAX, &PH_ms_tlm_list);
  PL_initialize(PH_st_tlm_stock_, PH_ST_TLM_LIST_MAX, &PH_st_tlm_list);
  PL_initialize(PH_rp_tlm_stock_, PH_RP_TLM_LIST_MAX, &PH_rp_tlm_list);

  PH_user_init();
}

// �p�P�b�g��͊֐�
// GSTOS����̃p�P�b�g�ȊO�����ׂĂ����ŏ��������
// Cmd_GENERATE_TLM�Ƃ����D
PH_ACK PH_analyze_packet(const CTCP* packet)
{
  switch (CTCP_get_tc_dsc(packet))
  {
  case CTCP_TC_DSC_CMD:
    return PH_analyze_cmd_(packet);

  case CTCP_TC_DSC_TLM:
    return PH_analyze_tlm_(packet);

  default:
    return PH_INVALID_DISCRIMINATOR;
  }

  return PH_UNKNOWN;
}

static PH_ACK PH_analyze_cmd_(const CTCP* packet)
{
  // ���[�U�[��`��
  // ��{�I�ɂ́C�ڑ�����Ă���C2A�𓋍ڂ����{�[�h��
  // ���̒i�K�i�L���[�C���O����C������������C PH_user_cmd_router �Ŏ��s�����̂ł͂Ȃ��C���̒i�K�j�œ]���������Ƃ��Ɏg��
  PH_ACK ack = PH_user_analyze_cmd(packet);
  if (ack != PH_UNKNOWN)
  {
    return ack;
  }

  switch (CCP_get_exec_type(packet))
  {
  case CCP_EXEC_TYPE_GS:
    return PH_add_gs_cmd_(packet);

  case CCP_EXEC_TYPE_TL0:
    return PH_add_tl_cmd_(0, packet, TMGR_get_master_total_cycle());

  case CCP_EXEC_TYPE_BC:
    return PH_analyze_block_cmd_(packet);

  case CCP_EXEC_TYPE_RT:
    return PH_add_rt_cmd_(packet);

  case CCP_EXEC_TYPE_UTL:
    return PH_add_utl_cmd_(packet);

  case CCP_EXEC_TYPE_TL1:
    return PH_add_tl_cmd_(1, packet, TMGR_get_master_total_cycle());

  case CCP_EXEC_TYPE_TL2:
    return PH_add_tl_cmd_(2, packet, TMGR_get_master_total_cycle());

  default:
    return PH_UNKNOWN;
  }
}

static PH_ACK PH_analyze_block_cmd_(const CTCP* packet)
{
  switch (BCT_register_cmd(packet))
  {
  case BCT_SUCCESS:
    return PH_BC_REGISTERED;

  case BCT_INVALID_BLOCK_NO:
    return PH_BC_INVALID_BLOCK_NO;

  case BCT_INVALID_CMD_NO:
    return PH_BC_INVALID_CMD_NO;

  case BCT_ISORATED_CMD:
    return PH_BC_ISORATED_CMD;

  case BCT_CMD_TOO_LONG:
    return PH_BC_CMD_TOO_LONG;

  default:
    return PH_UNKNOWN;
  }
}

static PH_ACK PH_analyze_tlm_(const CTCP* packet)
{
  CTP_DEST_FLAG flag = CTP_get_dest_flag(packet);

  // Housekeeping Telemetry
  if (flag & CTP_DEST_FLAG_HK) PH_add_ms_tlm_(packet);  // hk_tlm �̃t���O�������Ă��Ă��CMS_TLM�Ƃ��ď���������j�ɂ���

  // Mission Telemetry
  if (flag & CTP_DEST_FLAG_MS) PH_add_ms_tlm_(packet);

  // Stored Telemetry
  if (flag & CTP_DEST_FLAG_ST) PH_add_st_tlm_(packet);

  // Replay Telemetry
  if (flag & CTP_DEST_FLAG_RP) PH_add_rp_tlm_(packet);

  // [TODO] �v����:�eQueue���̓o�^�G���[����͖�����
  return PH_SUCCESS;
}

CCP_EXEC_STS PH_dispatch_command(const CTCP* packet)
{
  if (CTCP_get_tc_dsc(packet) != CTCP_TC_DSC_CMD)
  {
    // CMD�ȊO�̃p�P�b�g��������ُ픻��B
    return CCP_EXEC_PACKET_FMT_ERR;
  }

  if (CCP_get_dest_id(packet) == CTCP_MY_DST_ID)
  {
    // �������ẴR�}���h�̏ꍇ�͑Ή��������Ăяo���B
    return CA_execute_cmd(packet);
  }
  else
  {
    // �ʋ@�툶�R�}���h�̏ꍇ�̓p�P�b�g���z������
    return PH_user_cmd_router(packet);              // �����Ccore��cmd_router_�����݂������Ccore�ɂ����Ă��Ӗ����Ȃ��̂�user�݂̂ɂ���
  }
}

static PH_ACK PH_add_gs_cmd_(const CTCP* packet)
{
  PL_ACK ack = PL_push_back(&PH_gs_cmd_list, packet);

  if (ack != PL_SUCCESS) return PH_PL_LIST_FULL;

  return PH_SUCCESS;
}

static PH_ACK PH_add_rt_cmd_(const CTCP* packet)
{
  PL_ACK ack = PL_push_back(&PH_rt_cmd_list, packet);

  if (ack != PL_SUCCESS) return PH_PL_LIST_FULL;

  return PH_REGISTERED;
}

static PH_ACK PH_add_tl_cmd_(int line_no,
                            const CTCP* packet,
                            cycle_t now)
{
  PL_ACK ack = PL_insert_tl_cmd(&(PH_tl_cmd_list[line_no]), packet, now);

  switch (ack)
  {
  case PL_SUCCESS:
    return PH_TLC_REGISTERD;

  case PL_LIST_FULL:
    return PH_PL_LIST_FULL;

  case PL_TLC_PAST_TIME:
    return PH_TLC_PAST_TIME;

  case PL_TLC_ALREADY_EXISTS:
    return PH_TLC_ALREADY_EXISTS;

  default:
    return PH_UNKNOWN;
  }
}

static PH_ACK PH_add_utl_cmd_(const CTCP* packet)
{
  static CTCP temp_; // �T�C�Y���傫�����ߐÓI�̈�Ɋm��

  // utl_unixtime : time_manager.h �� utl_unixtime_epoch_ ���Q��
  // UTL_cmd �ł̓p�P�b�g�w�b�_�[�� ti �̕����� utl_unixtime ���i�[����Ă���
  cycle_t utl_unixtime = CCP_get_ti(packet);
  cycle_t ti = TMGR_get_ti_from_utl_unixtime(utl_unixtime);

  // TL_cmd �ɕϊ����� tl_cmd_list �ɒǉ�����
  CTCP_copy_packet(&temp_, packet);
  CCP_set_ti(&temp_, ti);
  CCP_set_exec_type(&temp_, CCP_EXEC_TYPE_TL0); // UTL -> TL0

  return PH_add_tl_cmd_(0, &temp_, TMGR_get_master_total_cycle());
}

static PH_ACK PH_add_ms_tlm_(const CTCP* packet)
{
  PL_ACK ack = PL_push_back(&PH_ms_tlm_list, packet);

  if (ack != PL_SUCCESS) return PH_PL_LIST_FULL;

  return PH_REGISTERED;
}

static PH_ACK PH_add_st_tlm_(const CTCP* packet)
{
  PL_ACK ack = PL_push_back(&PH_st_tlm_list, packet);

  if (ack != PL_SUCCESS) return PH_PL_LIST_FULL;

  return PH_REGISTERED;
}

static PH_ACK PH_add_rp_tlm_(const CTCP* packet)
{
  PL_ACK ack = PL_push_back(&PH_rp_tlm_list, packet);

  if (ack != PL_SUCCESS) return PH_PL_LIST_FULL;

  return PH_REGISTERED;
}

#pragma section
