#pragma section REPRO
#include "SequenceItems.h"
#include "../../../CmdTlm/block_command_definitions.h"
#include <src_core/Applications/timeline_command_dispatcher.h>
#include <src_core/Library/endian_memcpy.h>
#include <src_core/CmdTlm/common_tlm_cmd_packet_util.h>
#include "../../../Settings/AnomalyLogger/anomaly_group.h"
#include "../mode_definitions.h"


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// SI���g�����Ƃ͂����񐄏��I�I�I
// ���ʂ�BC���g�����ƁI�I�I�I
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void SI_finish_transition(CTCP* packet)
{
  CCP_form_rtc(packet, Cmd_CODE_MM_FINISH_TRANSITION, NULL, 0);
}

void SI_start_hk_tlm(CTCP* packet)
{
  unsigned char param[1 + SIZE_OF_BCT_ID_T];
  bct_id_t bc_id;

  param[0] = TL_ID_DEPLOY_TLM; // TL2�ɓW�J
  bc_id = BC_HK_CYCLIC_TLM;
  endian_memcpy(&param[1], &bc_id, SIZE_OF_BCT_ID_T);
  CCP_form_rtc(packet, Cmd_CODE_TLCD_DEPLOY_BLOCK, &param[0], 1 + SIZE_OF_BCT_ID_T);
}


// BC�W�J
void SI_deploy_block(CTCP* packet, int line_no, bct_id_t block_no)
{
  unsigned char param[1 + SIZE_OF_BCT_ID_T];

  param[0] = (unsigned char)line_no;
  endian_memcpy(&param[1], &block_no, SIZE_OF_BCT_ID_T);
  CCP_form_rtc(packet, Cmd_CODE_TLCD_DEPLOY_BLOCK, &param[0], 1 + SIZE_OF_BCT_ID_T);
}

#pragma section
