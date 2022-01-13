#ifndef TASK_DISPATCHER_H_
#define TASK_DISPATCHER_H_

#include "../../TlmCmd/block_command_table.h"
#include "../../TlmCmd/command_dispatcher.h"
#include "../ApplicationManager/app_info.h"

#define TDSP_TASK_MAX BCT_MAX_CMD_NUM

/**
 * @struct TDSP_Info
 * @brief TaskDispatcher�̏��
 */
typedef struct
{
  CommandDispatcher tskd;     //!< �^�X�N���X�g�ւ̃|�C���^�ƁA���̑����s����ۑ�����\����
  bct_id_t task_list_id;      //!< �^�X�N���X�g�ɓW�J����u���b�N�R�}���h��ID
  cycle_t  activated_at;      //!< �u���b�N�R�}���h���^�X�N���X�g�ɓW�J���ꂽ���̃T�C�N����
} TDSP_Info;

/**
 * @enum  TDSP_ACK
 * @brief TDSP�̃G���[�X�e�[�^�X
 * @note  uint8_t
 */
typedef enum
{
  TDSP_SUCCESS,
  TDSP_DEPLOY_FAILED,    //!< �u���b�N�R�}���h�̃^�X�N���X�g�ւ̓W�J���ɃG���[������
  TDSP_CYCLE_OVERRUN,    //!< 1�T�C�N���ȓ��ɂ��ׂẴR�}���h�����s���I���Ȃ�����
  TDSP_STEP_OVERRUN,     //!< �R�}���h�����s�\��̃X�e�b�v�����߂��Ă���
  TDSP_TASK_EXEC_FAILED, //!< �R�}���h���s���ɃG���[������
  TDSP_INVAILD_BCT_ID,   //!< ���s���� BCT_ID ������������
  TDSP_INACTIVE_BCT_ID,  //!< ���s���� BCT_ID ������������Ă���
  TDSP_EMPTY_BC,         //!< ���s���� BC ���󂾂���
  TDSP_UNKNOWN
} TDSP_ACK;

extern const TDSP_Info* const TDSP_info;

/**
 * @brief �^�X�N�Ǘ����s��TDSP_Info�\����(TDSP_Info_)�̏�����
 * @note  �f�[�^�\���� Packet List
 */
void TDSP_initialize(void);

/**
 * @brief �w�肵�� BC �� TaskList �ɓW�J������̂Ƃ��ēo�^
 * @param[in] id: BC �� id
 * @note �W�J���̂�TDSP_deploy_block_as_task_list_�ōs��
 *       "BC �� TaskList �ɓW�J" �Ƃ� `src_user\Settings\Modes\TaskLists\InitialTL.c` �Ȃǂ��Q��.
 */
TDSP_ACK TDSP_set_task_list_id(bct_id_t id);

/**
 * @brief ���ݎ����ɉ����āATaskList ���� Cmd ������s���� or TaskList ���ēW�J����
 *
 *        �W�J����Ă���^�X�N���X�g�̎��s���� (cycle ���x��) ���r��, task_list_ �ɓo�^����Ă���^�X�N�����ԂɎ��s����.
 *        1�^�X�N����������� return ����. (while(1) �ŉ���Ă���̂ł����߂��Ă���.)
 *        ���s cycle �����݂������ꍇ, �e�^�X�N�� step �ɂ���Ď��s����, ���Ȃ��� switch �ɍ��킹�ď�������.
 *        ���ۂɃ^�X�N����������ꍇ, CDIS_dispatch_command -> PH_dispatch_command -> cmdExec �̏��Ɏ��s�����(�^�� Executer �� cmdExec).
 */
void TDSP_execute_pl_as_task_list(void);

/**
 * @brief �^�X�N���X�g�̓W�J���T�C�N���������݂̃T�C�N�����ɋ����I�ɍ��킹��
 */
void TDSP_resync_internal_counter(void);

/**
 * @brief �w�肵���u���b�N�R�}���h���A���Ƀ^�X�N���X�g�ɓW�J������̂Ƃ��ēo�^����R�}���h
 */
CCP_EXEC_STS Cmd_TDSP_SET_TASK_LIST(const CTCP* packet);

AppInfo print_tdsp_status(void);

#endif
