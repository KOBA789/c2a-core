/**
 * @file mode_manager.h
 * @brief ���[�h�J�ڂ̐���̒�`
 */
#ifndef MODE_MANAGER_H_
#define MODE_MANAGER_H_

#include "../TimeManager/obc_time.h"
#include <src_user/Settings/Modes/mode_definitions.h>
#include "../../CmdTlm/common_tlm_cmd_packet.h"
#include "../../CmdTlm/block_command_table.h"

#define MM_NOT_DEFINED (BCT_MAX_BLOCKS)

#include <src_user/Settings/Modes/mode_manager_params.h>

/**
 * @enum  MM_ACK
 * @brief ModeManager �֘A�֐��̕Ԃ�l
 * @note  uint8_t ��z��
 */
typedef enum
{
  MM_SUCCESS,         //!< ����
  MM_BAD_ID,          //!< ���[�h�ԍ��ُ�
  MM_BAD_BC_INDEX,    //!< ���[�h�p�� BC �� ID �ُ�
  MM_INACTIVE_BLOCK,  //!< ���[�h�p BC �� ����
  MM_OVERWRITE,       //!< �ʂ̃��[�h�J�ڎ��s��
  MM_ILLEGAL_MOVE,    //!< ���[�h�J�ڗp�� BC �����݂��Ȃ�
  MM_NOT_IN_PROGRESS, //!< ���[�h�J�ڒ��ł͂Ȃ�
  MM_TL_LOAD_FAILED   //!< Task List �ւ̃��[�h BC �o�^���s
} MM_ACK;

/**
 * @enum MM_STATUS
 * @brief ���[�h�J�ڏ��
 * @note  uint8_t ��z��
 */
typedef enum
{
  MM_STATUS_FINISHED,   //!< ���[�h�J�ڏI��
  MM_STATUS_IN_PROGRESS //!< ���[�h�J�ڎ��s��
} MM_STATUS;

/**
 * @struct MM_TransitionEdge
 * @brief MM TLM �p�̍\����
 * @note TransitionTable �S�̂��ڂ���� Tlm �� 1�y�[�W�Ɏ��܂�Ȃ�����
 */
typedef struct
{
  uint8_t  from;     //!< ���[�h�J�ڌ�
  uint8_t  to;       //!< ���[�h�J�ڐ�
  bct_id_t bc_index; //!< ���[�h�J�ڂɊY������ bc id
} MM_TransitionEdge;

/**
 * @struct ModeManager
 * @brief ModeManager �{��
 */
typedef struct
{
  bct_id_t mode_list[MD_MODEID_MODE_MAX];                            //!< �e���[�hID�ɑΉ�����u���b�N�R�}���hID��ۑ�����
  bct_id_t transition_table[MD_MODEID_MODE_MAX][MD_MODEID_MODE_MAX]; //!< �e���[�h�J��(���郂�[�hID���炠�郂�[�hID�ւ̑J��)�ɑΉ�����u���b�N�R�}���h��ۑ�����
  MM_STATUS stat;                                                    //!< ���[�h�J�ڏ��
  MD_MODEID previous_id;                                             //!< �ЂƂO�̃��[�hID
  MD_MODEID current_id;                                              //!< ���݂̃��[�hID
  MM_ACK mm_ack;                                                     //!< �G���[���ۑ��p

  MM_TransitionEdge transition_table_for_tlm[MD_MODEID_MODE_MAX * MD_MODEID_MODE_MAX]; //!< �e�����p�̃e�[�u��
} ModeManager;

extern const ModeManager* const mode_manager;

/**
 * @brief ���[�h�J�ڂ��Ǘ����� ModeManager �\���� (mode_manager_) �̏�����
 * @param void
 * @return void
 */
void MM_initialize(void);

/**
 * @brief TLM �p���̐���
 * @param void
 * @return uint8_t: �J�� Edge �{��
 * @note �e�����̏㕔�ŌĂяo�����悤�ɂ���ƃe�����������ɏ���ɏ�񂪍X�V����ĕ֗� (tlm update cmd ��łK�v������)
 */
uint16_t MM_update_transition_table_for_tlm(void);

/**
 * @brief Cmd_MM_SET_MODE_LIST�̎���
 * @param[in] mode: �Ή� Mode ID
 * @param[in] bc_index: Mode �ɑΉ����� BC ID
 * @return MM_ACK
 */
MM_ACK MM_set_mode_list(MD_MODEID mode, bct_id_t bc_index);

/**
 * @brief Cmd_MM_SET_TRANSITION_TABLE�̎���
 * @param[in] from: �J�ڌ�
 * @param[in] to: �J�ڐ�
 * @param[in] bc_index: �J�ڂɑΉ����� BC ID
 * @return MM_ACK
 */
MM_ACK MM_set_transition_table(MD_MODEID from, MD_MODEID to, bct_id_t bc_index);

/**
 * @brief mode �ɑΉ����� TL �� BCT ID ���擾����
 * @note �����A�T�[�V�����͍s��Ȃ��D�s���� mode �̏ꍇ�� 0 ��Ԃ��D
 * @param[in] mode: �Ή����郂�[�h
 * @return ���[�h�Ή����� BC ID
 */
bct_id_t MM_get_tasklist_id_of_mode(MD_MODEID mode);

CCP_EXEC_STS Cmd_MM_SET_MODE_LIST(const CTCP* packet);
CCP_EXEC_STS Cmd_MM_SET_TRANSITION_TABLE(const CTCP* packet);
CCP_EXEC_STS Cmd_MM_START_TRANSITION(const CTCP* packet);
CCP_EXEC_STS Cmd_MM_FINISH_TRANSITION(const CTCP* packet);
CCP_EXEC_STS Cmd_MM_UPDATE_TRANSITION_TABLE_FOR_TLM(const CTCP* packet);

#endif
