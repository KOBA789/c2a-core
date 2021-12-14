/**
 * @file   block_command_definitions.h
 * @brief  �u���b�N�R�}���h��`
 * @note   ���̃R�[�h�͎�����������Ă��܂��I
 */
#ifndef BLOCK_COMMAND_DEFINITIONS_H_
#define BLOCK_COMMAND_DEFINITIONS_H_

// �o�^�����BlockCommandTable��block�ԍ����K��
typedef enum
{

  // Block Cmds for Mode Transition (�V�[�P���X���X�g)
  // ./src_user/Settings/Modes/Transitions/ �Œ�`
  BC_SL_START_UP_TO_INITIAL = 0,
  BC_SL_NOP  = 17,

  // Block Cmds for TaskList (�^�X�N���X�g) : 286-300
  // ./src_user/Settings/Modes/TaskLists/ �Œ�`
  BC_TL_START_UP = 20,
  BC_TL_INITIAL = 21,

  // Block Cmds for Composition (App Rotator, Combinar)
  // ./src_user/Settings/Modes/TaskLists/Composition/ �Œ�`
  BC_AR_DEBUG_DISPLAY_INI = 40,
  BC_AR_DRIVERS_UPDATE_INI = 42,
  BC_AR_GS_RELATED_PROCESS = 44,
  BC_AC_TLM_CMD_HIRATE = 50,

  // ==== �e�n�̈� ====
  // ./C2A/CmdTlm/NormalBlockCommandDefinition/�Œ�`
  // �A�m�}���n���h����BC_AH_�ȂǁC�ړ�����K�؂ɂ��邱�ƁI

  // CDH:60-79
  BC_HK_CYCLIC_TLM = 60,
  BC_RESERVED_FOR_HK = 77,    // EM�d�C�����ł̃R�}���h�t�@�C���Ƃ̃o�b�e�B���O��h��

  // COMM:90-99

  // ==== �n�ォ��up����g�̂݁h�̈� ====
  // C2A�ł͎g�p���Ȃ�

  // ==== �ǉ��̈� ====

  // Telemetry Manager
  BC_TLM_MGR0 = 350,
  BC_TLM_MGR1 = 351,
  BC_TLM_MGR2 = 352,
  BC_TLM_MGR3 = 353,
  BC_TLM_MGR4 = 354,
  BC_TLM_MGR5 = 355,
  BC_TLM_MGR6 = 356,
  BC_TLM_MGR7 = 357,
  BC_TLM_MGR8 = 358,
  BC_TLM_MGR9 = 359,

  // Test
  BC_TEST_EH_RESPOND = 360

  // BCT MAX : 382
} BC_DEFAULT_ID;

void BC_load_defaults(void);

#endif
