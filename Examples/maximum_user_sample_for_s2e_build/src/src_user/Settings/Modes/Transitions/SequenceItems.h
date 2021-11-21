#ifndef SEQUENCE_ITEMS_H_
#define SEQUENCE_ITEMS_H_

#include <src_core/CmdTlm/common_tlm_cmd_packet.h>
#include "../../../CmdTlm/command_definitions.h"
#include <src_core/CmdTlm/block_command_table.h>

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// SI���g�����Ƃ͂����񐄏��I�I�I
// ���ʂ�BC���g�����ƁI�I�I�I
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void SI_finish_transition(CTCP* packet);

void SI_start_hk_tlm(CTCP* packet);

// BC�W�J
void SI_deploy_block(CTCP* packet, int line_no, bct_id_t block_no);


#endif // SEQUENCE_ITEMS_H_
