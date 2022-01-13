#ifndef TIMELINE_COMMAND_DISPATCHER_H_
#define TIMELINE_COMMAND_DISPATCHER_H_

/**
 * @enum   TL_ID
 * @brief  TimeLine��I�ԂƂ��ɓ���I�Ɏg���R�[�h
 * @note   uint8_t��z��
 */
typedef enum
{
  TL_ID_DEPLOY_FROM_GS = 0,
  TL_ID_DEPLOY_BC      = 1,
  TL_ID_DEPLOY_TLM     = 2,
  TL_ID_MAX
} TL_ID;
// FIXME: �S�̓I�ɁC line_no �� int �Ȃ̂ŁC enum �ɂ���
// FIXME: TL��3�{�ȏ�ɂł���悤�ɂ���

// �z�Q�Ƃ�h�����߂ɂ�����include
#include "../TlmCmd/command_dispatcher.h"
#include "../TlmCmd/common_tlm_cmd_packet.h"
#include "../TlmCmd/packet_handler.h"
#include "../System/ApplicationManager/app_info.h"

// FIXME: �\���̂ɂ���
extern const CommandDispatcher* const timeline_command_dispatcher;
extern const int* TLCD_line_no_for_tlm;
extern const cycle_t* TLCD_tl_tlm_updated_at;
extern const CTCP* TLCD_tl_list_for_tlm[PH_TL0_CMD_LIST_MAX];
extern const int* TLCD_page_no;

AppInfo TLCD0_create_app(void);
AppInfo TLCD1_create_app(void);
AppInfo TLCD2_create_app(void);

/**
 * @brief TLM �̓��e�������X�V����.
 * @param[in] line_no TLCD_line_no_for_tlm �����邱�Ƃ�z�肵�Ă���
 * @return uint8_t ������ line_no ��Ԃ�, TL TLM �ɂ����f�����.
 * @note ���ꂪ TLM �̖`���ŌĂ΂�邱�Ƃ� TLM �̓��e������ɍX�V�����.
 */
uint8_t TLCD_update_tl_list_for_tlm(uint8_t line_no);

CCP_EXEC_STS Cmd_TLCD_CLEAR_ALL_TIMELINE(const CTCP* packet);
CCP_EXEC_STS Cmd_TLCD_CLEAR_TIMELINE_AT(const CTCP* packet);
CCP_EXEC_STS Cmd_TLCD_SET_SOE_FLAG(const CTCP* packet);
CCP_EXEC_STS Cmd_TLCD_SET_LOUT_FLAG(const CTCP* packet);
CCP_EXEC_STS Cmd_TLCD_SET_LINE_NO_FOR_TIMELINE_TLM(const CTCP* packet);
CCP_EXEC_STS Cmd_TLCD_DEPLOY_BLOCK(const CTCP* packet);
CCP_EXEC_STS Cmd_TLCD_SET_PAGE_FOR_TLM(const CTCP* packet);
CCP_EXEC_STS Cmd_TLCD_CLEAR_ERR_LOG(const CTCP* packet);

#endif
