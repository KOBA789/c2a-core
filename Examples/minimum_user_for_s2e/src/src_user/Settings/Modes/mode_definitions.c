#pragma section REPRO
/**
 * @file
 * @brief ���[�h��`�ƃ��[�h�J�ڒ�`
 */
#include "mode_definitions.h"

#include <src_core/System/ModeManager/mode_manager.h>

#include "../../TlmCmd/block_command_definitions.h"


// ���[�h���Ƃ�TL���w��
void MD_load_mode_list(void)
{
  MM_set_mode_list(MD_MODEID_START_UP,      BC_TL_START_UP);
  MM_set_mode_list(MD_MODEID_INITIAL,       BC_TL_INITIAL);
  MM_set_mode_list(MD_MODEID_RESERVED_1,    BC_TL_INITIAL);
  MM_set_mode_list(MD_MODEID_RESERVED_2,    BC_TL_INITIAL);
}

void MD_load_transition_table(void)
{
  // �N�����̂ݎ��s
  MM_set_transition_table(MD_MODEID_START_UP, MD_MODEID_INITIAL, BC_SL_START_UP_TO_INITIAL);
}

#pragma section
