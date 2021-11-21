#ifndef APP_MANAGER_H_
#define APP_MANAGER_H_

#include <stddef.h> // for size_t

#include "app_info.h"
#include "../../CmdTlm/common_tlm_cmd_packet.h"

#define AM_TLM_PAGE_SIZE (32)                               //!< AM��AppInfo�e�[�u����1�e�����g���p�P�b�g(=1�y�[�W)�Ɋi�[�����AppInfo���i�y�[�W�l�[�V�����p�j
#define AM_TLM_PAGE_MAX (4)                                 //!< AM��AppInfo�e�[�u���̃y�[�W���i�y�[�W�l�[�V�����p�j
#define AM_MAX_APPS (AM_TLM_PAGE_SIZE * AM_TLM_PAGE_MAX)    //!< �o�^�ł���ő�AppInfo���C����͓o�^�ł���ő�A�v�����Ɠ�����

#include <src_user/Settings/System/AppManagerParams.h>

typedef struct
{
  AppInfo ais[AM_MAX_APPS];
  int page_no;
} AppManager;

typedef enum
{
  AM_SUCCESS,
  AM_INVALID_ID,
  AM_NOT_REGISTERED
} AM_ACK;

const extern AppManager* const app_manager;

void AM_initialize(void);

AM_ACK AM_register_ai(size_t id,
                      const AppInfo* ai);

void AM_initialize_all_apps(void);

CCP_EXEC_STS Cmd_AM_REGISTER_APP(const CTCP* packet);

CCP_EXEC_STS Cmd_AM_INITIALIZE_APP(const CTCP* packet);

CCP_EXEC_STS Cmd_AM_EXECUTE_APP(const CTCP* packet);

CCP_EXEC_STS Cmd_AM_SET_PAGE_FOR_TLM(const CTCP* packet);

// 2019-07-18 �ǉ�
// min, max, prev�̂ݏ����Dinit�͂��̂܂�
CCP_EXEC_STS Cmd_AM_CLEAR_APP_INFO(const CTCP* packet);

#endif
