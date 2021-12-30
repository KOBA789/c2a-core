/**
 * @file
 * @brief �C�x���g���Ό^�������s��
 * @note  event_logger �̏������� BC ��W�J���� ( Event ���΂ɉ����đΉ����s�� )
 * @note  ���i�� EH �Ή��̑g�ݕ�
 *        ���i�� EH ��g�ޏꍇ�C EH_RuleSettings.event.group �� EL_CORE_GROUP_EH_MATCH_RULE ���w�肷��D
 *        ��������ƁC��ʂ̃��[�����}�b�`�����ꍇ�C�Y�����郋�[���Ή��͎��s����Ȃ��Ȃ�
 *        �Ⴆ�΁C UART �s�ʂ��l����
 *        �ݒ�Ƃ���
 *          - UART�s�ʂ����������� EL_UART �𔭍s����
 *          - EL_UART �� 5 �񔭐������� UART �h���C�o���Z�b�g���s�� EH_Rule1 ��ݒ肷��
 *          - EH_Rule1 �̑Ή� BC �� EH_Rule1 ���ēx�L��������悤�ɂ���
 *          - EH_Rule1 �̃}�b�` (group: EL_CORE_GROUP_EH_MATCH_RULE, local: EH_Rule1 �� EH_RULE_ID) �� 3 �񔭐������� UART ��H���Z�b�g���s�� EH_Rule2 ��ݒ肷��
 *        �Ƃ����ꍇ�C
 *          1. EL_UART �� 5 �񔭐�������C EH_Rule1 ������
 *          2. EL_UART �� 10 �񔭐�������C EH_Rule1 ������
 *          3. EL_UART �� 15 �񔭐�������C EH_Rule1 �͔��΂����ɁC EH_Rule2 ������
 *          4. EL_UART �� 20 �񔭐�������C EH_Rule1 ������
 *          5. ...
 *        �Ƃ����悤�ɂȂ�
 *        ���̂悤�ɁC EL_CORE_GROUP_EH_MATCH_RULE �Ŕ��΂��� EH_Rule �͏�ʃ��[���Ƃ��ĉ��߁C���s����C
 *        ��ʃ��[�������s�����Ƃ��́C���ʃ��[���͎��s����Ȃ��Ȃ�D
 *        �܂��C����� 2 �i�݂̂Ȃ炸�C 3 �i�ȏ���\�ł���D
 *        ���̎�@�����p����ƁC���[���̃I�[�o�[���C�h�ł���D
 *        �Ⴆ�΁C�ݒ�Ƃ���
 *          - UART�s�ʂ����������� EL_UART �𔭍s����
 *          - EL_UART �� 5 �񔭐������� UART �h���C�o���Z�b�g���s�� EH_Rule1 ��ݒ肷��
 *          - EH_Rule1 �̑Ή� BC �� EH_Rule1 ���ēx�L��������悤�ɂ���
 *          - EH_Rule1 �̃}�b�` (group: EL_CORE_GROUP_EH_MATCH_RULE, local: EH_Rule1 �� EH_RULE_ID) �� 1 �񔭐������� UART ��H���Z�b�g���s�� EH_Rule2 ��ݒ肷��
 *        �Ƃ���ƁC
 *          - EL_UART �� 5 �񔭐�������C EH_Rule1 �͔��΂����ɁC EH_Rule2 ������
 *        �ƂȂ�̂ŁC�����I�ɁC EH_Rule2 �� EH_Rule1 ���I�[�o�[���C�h���邱�Ƃ��ł���D
 *        ���̂悤�ɁC���ʂ̃��[�����C��ʂ̔��Ώ�����ς��邱�ƂŁC�_��ɃI�[�o�[���C�h�ł���D
 * @note  EH �ł� Event ���s�͈ȉ�
 *          - EL_CORE_GROUP_EVENT_HANDLER
 *            - EH �Ɋւ���l�X�ȃG���[
 *            - local id �� EH_EL_LOCAL_ID
 *          - EL_CORE_GROUP_EH_MATCH_RULE
 *            - EH_Rule �Ƀ}�b�`
 *            - local id �� EH_RULE_ID
 *          - EL_CORE_GROUP_EH_RESPOND_WITH_HIGHER_LEVEL_RULE
 *            - EH_Rule �Ƀ}�b�`�������C����ɏ�ʂ� EH_Rule �̃}�b�`�����𖞂��������߁C���΂̓L�����Z�����ꂽ�i��ʂŔ��΂����j
 *            - local id �� EH_RULE_ID
 */
#ifndef EVENT_HANDLER_H_
#define EVENT_HANDLER_H_

// TODO: �ݐϔ���couter�̃��Z�b�g
// TODO: �ݐϔ���couter��activate����0�N���A����

#include "event_logger.h"

#ifdef EL_IS_ENABLE_TLOG

#include <stddef.h>
#include "../TimeManager/obc_time.h"
#include "../../CmdTlm/block_command_table.h"
#include <src_user/Settings/System/EventHandlerRules/event_handler_rules.h>


#define EH_RULE_TLM_PAGE_SIZE (20)            //!< event_handler �̃��[���e�[�u����1�e�����g���p�P�b�g(=1�y�[�W)�Ɋi�[����郋�[�����i�y�[�W�l�[�V�����p�j
#define EH_RULE_TLM_PAGE_MAX  (8)             //!< event_handler �̃��[���e�[�u���̃y�[�W���i�y�[�W�l�[�V�����p�j
#define EH_RULE_MAX           (EH_RULE_ID)(EH_RULE_TLM_PAGE_SIZE * EH_RULE_TLM_PAGE_MAX)
                                              /*!< �ő剽�̃��[�� ( EL_Event - EH_Rule �Ή�) ��ێ��ł��邩
                                                   ��{�I�ɁC EH_RULE_ID �Ƃ��Ďg���̂ŁC�L���X�g����
                                                   enum��Œ�`���Ȃ��̂́C EH_Rule �̍ő�l���C�����ɒ�`���鐔������������������
                                                   �܂��C���̒l�́C Rule ����`�l�i�Ȃ����͏����l�j�Ƃ��Ă��g�� */

#define EH_LOG_TLM_PAGE_SIZE (64)                                          //!< EH�Ή��̃��O�e�[�u����1�e�����g���p�P�b�g(=1�y�[�W)�Ɋi�[����郍�O���i�y�[�W�l�[�V�����p�j
#define EH_LOG_TLM_PAGE_MAX  (2)                                           //!< EH�Ή��̃��O�e�[�u���̃y�[�W���i�y�[�W�l�[�V�����p�j
#define EH_LOG_MAX           (EH_LOG_TLM_PAGE_SIZE * EH_LOG_TLM_PAGE_MAX)  //!< �ő剽�� EH �Ή����O��ێ��ł��邩


#define EH_MAX_RULE_NUM_OF_EL_ID_DUPLICATES   (4)     //!< EL_Event �� ID ���d������ EH_Rule ���ő剽�܂ŏd�������Ă悢��
#define EH_MAX_RESPONSE_NUM_DEFAULT           (8)     //!< ��x�̎��s�őΉ�����ő吔�i�����l�j
#define EH_MAX_CHECK_EVENT_NUM_DEFAULT        (64)    /*!< ��x�̎��s�Ń`�F�b�N���� event_logger �� event log �̍ő�l�i�����l�j
                                                           TL���ł̎��s���Ԃ𒲐����邽�߂ɐݒ肷��D */

// �ȉ��̃t�@�C���ɂāC���̃p�����^���㏑���ݒ�ł���
// EH_RULE_TLM_PAGE_SIZE
// EH_RULE_TLM_PAGE_MAX
// EH_LOG_TLM_PAGE_SIZE
// EH_LOG_TLM_PAGE_MAX
// EH_MAX_RULE_NUM_OF_EL_ID_DUPLICATES
// EH_MAX_RESPONSE_NUM_DEFAULT
// EH_MAX_CHECK_EVENT_NUM_DEFAULT
#include <src_user/Settings/System/event_handler_params.h>


/**
 * @enum   EH_REGISTER_ACK
 * @brief  EH_register_rule �̕Ԃ�l
 * @note   uint8_t ��z��
 */
typedef enum
{
  EH_REGISTER_ACK_OK = 0,                   //!< ����ɓo�^����
  EH_REGISTER_ACK_ERR_FULL,                 //!< ����ȏ�o�^�ł��Ȃ�
  EH_REGISTER_ACK_ERR_RULE_OVERWRITE,       //!< ���łɓ��� ID �Ƀ��[�����o�^����Ă��邽�ߊ��p
  EH_REGISTER_ACK_ERR_DUPLICATE_FULL,       //!< ����ȏ�C EL_Event �̏d������ EH_Rule ��o�^�ł��Ȃ�
  EH_REGISTER_ACK_ILLEGAL_RULE_ID,          //!< �s���� EH_RULE_ID
  EH_REGISTER_ACK_ILLEGAL_GROUP,            //!< �s���� EL_Event.group
  EH_REGISTER_ACK_ILLEGAL_ERROR_LEVEL,      //!< �s���� EL_Event.err_level
  EH_REGISTER_ACK_ILLEGAL_MATCH_FLAG,       //!< �s���� should_match_err_level
  EH_REGISTER_ACK_ILLEGAL_CONDITION_TYPE,   //!< �s���� EH_RESPONSE_CONDITION_TYPE
  EH_REGISTER_ACK_ILLEGAL_COUNT_THRESHOLD,  //!< �s���� condition.count_threshold
  EH_REGISTER_ACK_ILLEGAL_BCT_ID,           //!< �s���� deploy_bct_id
  EH_REGISTER_ACK_ILLEGAL_ACTIVE_FLAG,      //!< �s���� is_active
  EH_REGISTER_ACK_UNKNOWN_ERR               //!< �s���ȃG���[
} EH_REGISTER_ACK;

/**
 * @enum   EH_CHECK_RULE_ACK
 * @brief  EH_Rule �o�^�󋵊m�F�p
 * @note   EH_{in}activate_rule �̕Ԃ�l�Ȃǂɂ��p����
 * @note   uint8_t ��z��
 */
typedef enum
{
  EH_CHECK_RULE_ACK_OK = 0,           //!< ���� or ���[���o�^�ς�
  EH_CHECK_RULE_ACK_INVALID_RULE_ID,  //!< EH_RULE_ID ���s��
  EH_CHECK_RULE_ACK_UNREGISTERED      //!< ���[�������o�^
} EH_CHECK_RULE_ACK;

/**
 * @enum   EH_RESPONSE_CONDITION_TYPE
 * @brief  EL_Event �o�^�ɑ΂��āC�ǂ̂悤�� EH �Ή������邩�̎��
 * @note   uint8_t ��z��
 */
typedef enum
{
  EH_RESPONSE_CONDITION_SINGLE,       //!< Event �o�^�ɑ΂��đ����� EH �Ή�
  EH_RESPONSE_CONDITION_CONTINUOUS,   //!< Event �o�^�� N �b�ȓ��ɍĂѓ��� Event �o�^���������邱�Ƃ� M ��A���Ŕ��������ꍇ�� EH �Ή�
  EH_RESPONSE_CONDITION_CUMULATIVE,   //!< Event �� M ��o�^�����ꍇ�� EH �Ή�
  EH_RESPONSE_CONDITION_TYPE_MAX
} EH_RESPONSE_CONDITION_TYPE;

/**
 * @struct EH_RuleSettings
 * @brief  EH_register_rule �̈����DRule�o�^�ɕK�v�ȏ��
 */
typedef struct
{
  struct
  {
    EL_GROUP group;                           //!< �����|���� EL_Event.group
    uint32_t local;                           //!< �����|���� EL_Event.local
    EL_ERROR_LEVEL err_level;                 /*!< �����|���� EL_Event.err_level
                                                   should_match_err_level == 0 �̂Ƃ��͂Ȃ�ł��悢 */
  } event;                                    //!< EH �ɑΉ����� EL_Event
  uint8_t should_match_err_level;             //!< EL_Event �̈�v�ɁC�G���[���x����v���܂߂邩�H
  struct
  {
    EH_RESPONSE_CONDITION_TYPE type;          //!< EH �Ή��̏������
    uint16_t count_threshold;                 /*!< �����ɉ񐔂�����ꍇ�Ɏg���C�Ή��܂ł� Event ��臒l
                                                   type == EH_RESPONSE_CONDITION_SINGLE �̂Ƃ��͂Ȃ�ł��悢 */
    uint32_t time_threshold_ms;               /*!< �A�������Ƃ݂Ȃ����� [ms]
                                                   type == EH_RESPONSE_CONDITION_CONTINUOUS �̂Ƃ��̂ݗL�� */
  } condition;                                //!< EH �Ή�����
  bct_id_t deploy_bct_id;                     //!< EH �Ή��ɓW�J���� BCT id
  uint8_t  is_active;                         //!< �Ή����L�����H �Ή���C�����I�ɖ����ɂȂ�
} EH_RuleSettings;

/**
 * @struct EH_Rule
 * @brief  EL_Event �ɑ΂��đΉ����� EH_Rule
 */
typedef struct
{
  EH_RuleSettings settings;                   //!< �ݒ�l
  uint16_t counter;                           //!< EH �Ή��܂ł̃J�E���^
  ObcTime  last_event_time;                   //!< �Ō�ɔ������� EL_Event �̎���
} EH_Rule;

/**
 * @struct EH_RuleTable
 * @brief  EH_Rule �̃e�[�u��
 */
typedef struct
{
  EH_Rule  rules[EH_RULE_MAX];      //!< idx �� EH_RULE_ID
  uint16_t registered_rule_num;     //!< �o�^�ς� rule ��
} EH_RuleTable;

/**
 * @struct EH_RuleSortIndex
 * @brief  EH_RuleTable ���� EH_Rule ��񕪒T�����Č����ł���悤�ɂ��邽�߂̃\�[�g�ς�index
 * @note   {group, local, duplicate_id} ���� rule_id ���������߂ɂ���
 * @note   sorted_idxes[registered_rule_num] �ȍ~�̖��o�^�́C group == EL_CORE_GROUP_NULL, rule_id == EH_RULE_MAX �Ə�����
 */
typedef struct
{
  EL_GROUP group;           //!< EH_Rule.event.group
  uint32_t local;           //!< EH_Rule.event.local
  uint8_t  duplicate_id;    //!< ���� {group. local} �� rule ������ꍇ�́C�d��ID
  EH_RULE_ID rule_id;       //!< EventHandler.rules �� idx
} EH_RuleSortedIndex;

/**
 * @struct EH_Log
 * @brief  EH �Ή��̃��O
 */
typedef struct
{
  EH_RULE_ID   rule_id;
  cycle_t      respond_time_in_master_cycle;
  CCP_EXEC_STS deploy_cmd_ack;        // FIXME: �����I�ɁC�Ō�� BC �W�J�����Ƃ��̃G���[��񂪎擾�ł���悤�ɂȂ�����C����ɂ���
} EH_Log;

/**
 * @struct EH_LogTable
 * @brief  EH �Ή��̃��O�e�[�u��
 */
typedef struct
{
  EH_Log   logs[EH_LOG_MAX];
  uint32_t respond_counter;
  uint16_t log_wp;
} EH_LogTable;

/**
 * @struct EH_ElEventCounter
 * @brief  EL �C�x���g�J�E���^�D EL �̃C�x���g�L�^���ƏƂ炵���킹�āC�Ή����ׂ��C�x���g��T�����邽�߂Ɏg��
 * @note   EL_EventStatistics �ɑΉ�����
 */
typedef struct
{
  uint32_t counter_total;                  //!< �O�� EH_execute ���s���� EL �ł̃C�x���g�L�^��
  uint32_t counters[EL_ERROR_LEVEL_MAX];   //!< �G���[���x�����Ƃ̋L�^��
} EH_ElEventCounter;

/**
 * @struct EH_RegisterFromCmd
 * @brief  �R�}���h�o�R�� EH_Rule ��o�^����Ƃ��Ɏg��������ԕϐ�
 */
typedef struct
{
  EH_RULE_ID      rule_id;      //!< �o�^�� EH_RULE_ID
  EH_RuleSettings settings;     //!< �o�^���� EH_RuleSettings
  EH_REGISTER_ACK register_ack; //!< EH_register_rule �̕Ԃ�l
} EH_RegisterFromCmd;

/**
 * @struct EH_TlmInfo
 * @brief  tlm�̃y�[�W�l�[�V�����̏��ێ�
 */
typedef struct
{
  struct
  {
    uint8_t page_no;
    EH_RULE_ID target_rule_id;
  } rule;
  struct
  {
    uint8_t page_no;
  } rule_sorted_index;
  struct
  {
    uint8_t page_no;
  } log;
} EH_TlmInfo;

/**
 * @struct EventHandler
 * @brief  EventHandler ��Info�\����
 */
typedef struct
{
  EH_RuleTable       rule_table;                //!< EH_Rule �̃e�[�u��
  EH_RuleSortedIndex sorted_idxes[EH_RULE_MAX]; //!< rules �̓񕪒T���p idx
  EH_LogTable        log_table;                 //!< EH �Ή����O�e�[�u��
  EH_ElEventCounter  el_event_counter;          //!< EH_ElEventCounter
  uint8_t            max_response_num;          /*!< ��x�� EH_execute �̎��s�őΉ�����ő吔
                                                     �����l�� EH_MAX_RESPONSE_NUM_DEFAULT
                                                     EL_Event ��ID���d������ EH_Rule ������̂ŁC���ۂ̑Ή����͂��������\�������� */
  uint16_t           max_check_event_num;       /*!< ��x�̎��s�Ń`�F�b�N���� event_logger �� event log �̍ő�l
                                                     �����l�� EH_MAX_CHECK_EVENT_NUM_DEFAULT */
  EH_RegisterFromCmd reg_from_cmd;              //!< �R�}���h�o�R�� EH_Rule ��o�^����Ƃ��Ɏg��������ԕϐ�
  EH_TlmInfo         tlm_info;                  //!< tlm�̂��߂̏��
} EventHandler;

extern const EventHandler* const event_handler;


/**
 * @brief  event_handler �̏�����
 * @param  void
 * @return void
 */
void EH_initialize(void);

/**
 * @brief  event_handler �̎��s
 *
 *         EL �ɂēo�^���ꂽ�C�x���g�𑖍����C�Ή�����BC��W�J����
 * @note   event_utility app �Ȃǂ��������s����邱�Ƃ�z��
 * @param  void
 * @return void
 */
void EH_execute(void);

/**
 * @brief  ���[���̓o�^
 * @note   ��{�I�ɂ́C EH_load_default_rules �����ł̂ݎg���邱�Ƃ�z��
 * @param  settings: EH_RuleSettings
 * @param  id: EH_RULE_ID
 * @return EH_REGISTER_ACK
 */
EH_REGISTER_ACK EH_register_rule(EH_RULE_ID id, const EH_RuleSettings* settings);

/**
 * @brief  ���[���̗L����
 * @note   ��{�I�ɂ̓R�}���h�ő��삷��̂ŁC���ڎg�����Ƃ͂��܂�z�肵�Ă��Ȃ�
 * @param  id: EH_RULE_ID
 * @return EH_CHECK_RULE_ACK
 */
EH_CHECK_RULE_ACK EH_activate_rule(EH_RULE_ID id);

/**
 * @brief  ���[���̖�����
 * @note   ��{�I�ɂ̓R�}���h�ő��삷��̂ŁC���ڎg�����Ƃ͂��܂�z�肵�Ă��Ȃ�
 * @param  id: EH_RULE_ID
 * @return EH_CHECK_RULE_ACK
 */
EH_CHECK_RULE_ACK EH_inactivate_rule(EH_RULE_ID id);

/**
 * @brief  �C�x���g�J�E���^�� EL �̂���ɍ��킹��
 * @note   EL �������Z�b�g�����ۂɌĂяo���Ȃ��ƁC�s��������������i�܂��C����ɉ�������邯�ǁj
 * @param  void
 * @return void
 */
void EH_match_event_counter_to_el(void);

/**
 * @brief  EH_LogTable �̍ŐV����n�Ԗڂ̑Ή����O���擾
 * @note   n ���傫������ꍇ�́C�ŐV�̂��̂�Ԃ�
 * @param  n: n�Ԗځi0�̂Ƃ��ŐV�j
 * @return EH_Log
 */
const EH_Log* EH_get_the_nth_log_from_the_latest(uint16_t n);


CCP_EXEC_STS Cmd_EH_INIT(const CTCP* packet);

CCP_EXEC_STS Cmd_EH_CLEAR_ALL_RULE(const CTCP* packet);

CCP_EXEC_STS Cmd_EH_LOAD_DEFAULT_RULE(const CTCP* packet);

CCP_EXEC_STS Cmd_EH_SET_REGISTER_RULE_EVENT_PARAM(const CTCP* packet);

CCP_EXEC_STS Cmd_EH_SET_REGISTER_RULE_CONDITION_PARAM(const CTCP* packet);

CCP_EXEC_STS Cmd_EH_REGISTER_RULE(const CTCP* packet);

CCP_EXEC_STS Cmd_EH_DELETE_RULE(const CTCP* packet);

CCP_EXEC_STS Cmd_EH_ACTIVATE_RULE(const CTCP* packet);

CCP_EXEC_STS Cmd_EH_INACTIVATE_RULE(const CTCP* packet);

CCP_EXEC_STS Cmd_EH_CLEAR_LOG(const CTCP* packet);

CCP_EXEC_STS Cmd_EH_SET_MAX_RESPONSE_NUM(const CTCP* packet);

CCP_EXEC_STS Cmd_EH_SET_MAX_CHECK_EVENT_NUM(const CTCP* packet);

CCP_EXEC_STS Cmd_EH_SET_PAGE_OF_RULE_TABLE_FOR_TLM(const CTCP* packet);

CCP_EXEC_STS Cmd_EH_SET_PAGE_OF_RULE_SORTED_IDX_FOR_TLM(const CTCP* packet);

CCP_EXEC_STS Cmd_EH_SET_PAGE_OF_LOG_TABLE_FOR_TLM(const CTCP* packet);

CCP_EXEC_STS Cmd_EH_SET_TARGET_ID_OF_RULE_TABLE_FOR_TLM(const CTCP* packet);

/**
 * @brief �V���� EL_Event ���������o���邽�߂̃J�E���^�������I�� EL �̃J�E���^�ɍ��킹��
 */
CCP_EXEC_STS Cmd_EH_MATCH_EVENT_COUNTER_TO_EL(const CTCP* packet);

#endif  // EL_IS_ENABLE_TLOG

#endif
