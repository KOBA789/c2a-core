#ifndef ANOMALY_HANDLER_H_
#define ANOMALY_HANDLER_H_

#include <stddef.h> // for size_t

#include "../System/AnomalyLogger/anomaly_logger.h"
#include "../System/ApplicationManager/app_info.h"
#include "../CmdTlm/common_tlm_cmd_packet.h"
#include <src_user/CmdTlm/block_command_definitions.h>
#include "../CmdTlm/block_command_table.h"
#include <src_user/Settings/AnomalyLogger/anomaly_group.h>
#include <src_user/Settings/Applications/AnomalyHandlerRules/ah_rules.h>

#define AH_TLM_PAGE_SIZE (32)                                  //!< AnomalyHandler�̃��[���e�[�u����1�e�����g���p�P�b�g(=1�y�[�W)�Ɋi�[����郋�[�����i�y�[�W�l�[�V�����p�j
#define AH_TLM_PAGE_MAX  (4)                                   //!< AnomalyHandler�̃��[���e�[�u���̃y�[�W���i�y�[�W�l�[�V�����p�j
#define AH_MAX_RULES (AH_TLM_PAGE_SIZE * AH_TLM_PAGE_MAX)        //!< �ő剽�̃��[���i�A�m�}�� - �C�x���g�Ή��j��ێ��ł��邩

#define AH_LOG_TLM_PAGE_SIZE (32)                              //!< AnomalyHandler�̔����C�x���g���O�e�[�u����1�e�����g���p�P�b�g(=1�y�[�W)�Ɋi�[����锭���C�x���g���O���i�y�[�W�l�[�V�����p�j
#define AH_LOG_TLM_PAGE_MAX  (4)                               //!< AnomalyHandler�̔����C�x���g���O�e�[�u���̃y�[�W���i�y�[�W�l�[�V�����p�j
#define AH_LOG_MAX (AH_LOG_TLM_PAGE_SIZE * AH_LOG_TLM_PAGE_MAX)  //!< �ő剽�̃C�x���g���O��ێ��ł��邩
// 2018-12-11
// ��������āC AL_RECORD_MAX �ƃT�C�Y������K�v�Ȃ��́H
// �Ȃ������H

#include <src_user/Settings/Applications/anomaly_handler_params.h>

typedef enum
{
  AH_SINGLE,
  AH_CONTINUOUS,
  AH_CUMULATE
} AH_CONDITION;

typedef struct
{
  AL_AnomalyCode  code;
  AH_CONDITION cond;
  size_t       threshold;     // tlm�ł�unit8_t��....    AH_CUMULATE�p�̃J�E���^
  bct_id_t     bc_id;         // tlm�ł�unit8_t��....    // TODO 2019/08/30 BCT�̌`���ς�����̂ŁCTLM�������I�I�I
} AH_Rule;

typedef struct
{
  int     is_active;
  AH_Rule rule;
  size_t  counter;            // AH_CUMULATE�p�̃J�E���^
} AH_Element;

typedef struct
{
  size_t count;
  size_t pos;
} AH_Pointer;

typedef struct
{
  AH_Element elements[AH_MAX_RULES];
  AH_Pointer al_pointer;
  ObcTime    respond_at;
  size_t     latest_id;             // tlm�ł�unit8_t��....
  size_t     action_counter;        // tlm�ł�unit8_t��....
  size_t     latest_run_length;     // tlm�ł�unit8_t��....
  uint8_t    page_no;               // tlm�ł�unit8_t��....
} AnomalyHandler;

typedef struct
{
  size_t respond_at_master;
  size_t rule_num;
} AH_AnomalyRespond;

typedef struct
{
  AH_AnomalyRespond log[AH_LOG_MAX];
  size_t            pointer;
  uint8_t           page_no;
} AH_AnomalyRespondLog;


extern const AnomalyHandler* const anomaly_handler;
extern const AH_AnomalyRespondLog* const AH_respond_log;

AppInfo AH_create_app(void);

AppInfo print_ah_status(void);

void AH_activate_rule(size_t id);

void AH_inactivate_rule(size_t id);

// static�ȕ��̓f�t�H���g�ł͖��������C
// �����͏��������Ɏg����̂ŁC�I�𐧂ɂ���
void AH_add_rule(size_t id, const AH_Rule* ahr, uint8_t is_active);

CCP_EXEC_STS Cmd_AH_REGISTER_RULE(const CTCP* packet);

CCP_EXEC_STS Cmd_AH_ACTIVATE_RULE(const CTCP* packet);

CCP_EXEC_STS Cmd_AH_INACTIVATE_RULE(const CTCP* packet);

CCP_EXEC_STS Cmd_AH_CLEAR_LOG(const CTCP* packet);

CCP_EXEC_STS Cmd_AH_SET_PAGE_FOR_TLM(const CTCP* packet);

CCP_EXEC_STS Cmd_AHRES_LOG_CLEAR(const CTCP* packet);

CCP_EXEC_STS Cmd_AHRES_LOG_SET_PAGE_FOR_TLM(const CTCP* packet);

#endif
