/**
 * @file
 * @brief user���e�����g���l�܂����TL����h���C�܂�CDH�Ȃǂ��e�����g�����Ǘ����₷�����邽�߂�App
 * @note  https://gitlab.com/ut_issl/c2a/c2a_core_oss/-/issues/81 �� telemetry_manager.h �̍ŉ������Q�ƁiFIXME: ���Ƃ�document�Ɉڂ��j
 */
#ifndef TELEMETRY_MANAGER_H_
#define TELEMETRY_MANAGER_H_

#include "../System/ApplicationManager/app_info.h"
#include "../TlmCmd/common_tlm_cmd_packet.h"
#include "../TlmCmd/common_tlm_cmd_packet_util.h"
#include "../TlmCmd/block_command_table.h"


#define TLM_MGR_USE_BC_NUM            (10)    //!< �p����BC�̐��D��{�I��10�Œ��z��
#define TLM_MGR_MAX_TLM_NUM_PER_BC    (10)    //!< 1BC�ɉ��̃e���������R�}���h��o�^�ł��邩�D��{�I��10�Œ��z��


#if BCT_MAX_CMD_NUM < TLM_MGR_MAX_TLM_NUM_PER_BC       // FIXME: BCT������������Ȃ���
#error BCT_MAX_CMD_NUM is not enough for TelemetryManager
#endif


/**
 * @enum   TLM_MGR_ERR_CODE
 * @note   uint8_t ��z��
 * @brief  �ėp�G���[�R�[�h
 */
typedef enum
{
  TLM_MGR_ERR_CODE_OK,
  TLM_MGR_ERR_CODE_MASTER_DUPLICATED,      //!< BC ���s�� master�ƂȂ� BC ���d�����Ă���
  TLM_MGR_ERR_CODE_MASTER_IS_ABSENT,       //!< BC ���s�� master�ƂȂ� BC �����݂��Ȃ�
  TLM_MGR_ERR_CODE_REGISTER_INFO_BC_FULL,  //!< RegisterInfo �ɂ���ȏ� BC ��o�^�ł��Ȃ�
  TLM_MGR_ERR_CODE_CMD_FULL,               //!< ����ȏ�e���������R�}���h��o�^�ł��Ȃ�
  TLM_MGR_ERR_CODE_OTHER_ERR               //!< ���̑��̃G���[
} TLM_MGR_ERR_CODE;


/**
 * @enum   TLM_MGR_BC_TYPE
 * @note   uint8_t ��z��
 * @brief  BC�̃^�C�v�D����ɂĎ��s��ʂ��ς��
 */
typedef enum
{
  TLM_MGR_BC_TYPE_MASTER,
  TLM_MGR_BC_TYPE_HK_TLM,
  TLM_MGR_BC_TYPE_SYSTEM_TLM,
  TLM_MGR_BC_TYPE_HIGH_FREQ_TLM,
  TLM_MGR_BC_TYPE_LOW_FREQ_TLM,
  TLM_MGR_BC_TYPE_RESERVE
} TLM_MGR_BC_TYPE;


/**
 * @struct TLM_MGR_BcInfo
 * @brief  BC�̏��
 */
typedef struct
{
  bct_id_t        bc_id;
  TLM_MGR_BC_TYPE bc_type;
} TLM_MGR_BcInfo;


/**
 * @struct TLM_MGR_RegisterInfo
 * @brief  �ǂ�BC���Ǘ����C�e���������R�}���h��o�^���C�������Ă������̏��
 * @note   TLM_MGR_BcInfo �̏�񂩂琶�������
 */
typedef struct
{
  uint8_t bc_info_idx[TLM_MGR_USE_BC_NUM];             //!< bc_info �̂ǂ� idx �� BC ���g�����Dstatic�m�ۂ̂��߁C�ő吔 TLM_MGR_USE_BC_NUM �̔z����m��
  uint8_t bc_info_idx_used_num;                        //!< �g�p���Ă��� BC ��
  uint8_t tlm_register_pointer_to_idx_of_bc_info_idx;  //!< ���Ƀe���������R�}���h��o�^�����ꍇ�Ɏg�p����� bc_info_idx �̔z�� idx
  uint8_t tlm_register_pointer_to_bc_cmd_idx;          //!< ���Ƀe���������R�}���h��o�^�����ꍇ�ɓo�^����BC pos.cmd
} TLM_MGR_RegisterInfo;


/**
 * @struct TelemetryManager
 * @brief  TelemetryManager �� AppInfo�\����
 */
typedef struct
{
  TLM_MGR_BcInfo bc_info[TLM_MGR_USE_BC_NUM];
  struct
  {
    TLM_MGR_RegisterInfo master;          //!< BC�S�̂�W�J���Ă��� master BC
    TLM_MGR_RegisterInfo hk_tlm;          //!< HK �e�����i1 Hz�j
    TLM_MGR_RegisterInfo system_tlm;      /*!< �S�n��system�œ���Ă�������tlm�i1 Hz�j
                                               user��tlm�����D�ǉ��̉e�����󂯂Ȃ�
                                               ����ɂ���āCtlm�̑S�̊Ǘ����e�ՂɂȂ�  */
    TLM_MGR_RegisterInfo high_freq_tlm;   //!< user �e�����i1 Hz�j
    TLM_MGR_RegisterInfo low_freq_tlm;    //!< user �e�����i1/10 Hz�j
    TLM_MGR_RegisterInfo reserve;         //!< ����g���ĂȂ�BC�D�����̂����Ȃ̂ŁC�����o�͍�邪�e���������R�}���h�͓o�^����Ȃ�
  } register_info;
  bct_id_t master_bc_id;
  uint8_t is_inited;                      //!< ����������Ă��邩�H
} TelemetryManager;


extern const TelemetryManager* const telemetry_manager;

AppInfo TLM_MGR_create_app(void);


/**
 * @brief ������
 */
CCP_EXEC_STS Cmd_TLM_MGR_INIT(const CTCP* packet);

/**
 * @brief master bc �̏�����
 */
CCP_EXEC_STS Cmd_TLM_MGR_INIT_MASTER_BC(const CTCP* packet);

/**
 * @brief HK�e������������
 */
CCP_EXEC_STS Cmd_TLM_MGR_CLEAR_HK_TLM(const CTCP* packet);

/**
 * @brief system�e������������
 */
CCP_EXEC_STS Cmd_TLM_MGR_CLEAR_SYSTEM_TLM(const CTCP* packet);

/**
 * @brief high_freq_tlm, low_freq_tlm ��������
 * @note  �֋X�� TLM_MGR_BC_TYPE_RESERVE �� BC �����������Ă��܂�
 */
CCP_EXEC_STS Cmd_TLM_MGR_CLEAR_USER_TLM(const CTCP* packet);

/**
 * @brief TLM���o�J�n
 * @note  master bc �̖����� Cmd_TLCD_DEPLOY_BLOCK �ɂ��� deploy block ���Ă��邾��
 */
CCP_EXEC_STS Cmd_TLM_MGR_START_TLM(const CTCP* packet);

/**
 * @brief TLM���o�ꎞ��~
 * @note  master bc �̖����� Cmd_TLCD_DEPLOY_BLOCK �� NOP �ɍ����ւ��Ă��邾��
 * @note  Cmd_TLCD_CLEAR_ALL_TIMELINE / Cmd_TLM_MGR_CLEAR_TLM_TL ���Ƒ��̂��̂������Ă��܂�
 * @note  Cmd_TLCD_CLEAR_ALL_TIMELINE / Cmd_TLM_MGR_CLEAR_TLM_TL �̂ق����K�؂ȏꍇ������̂ł悭�l���邱��
 */
CCP_EXEC_STS Cmd_TLM_MGR_STOP_TLM(const CTCP* packet);

/**
 * @brief TLM���o�pTL���N���A
 * @note  Cmd_TLCD_CLEAR_ALL_TIMELINE ���Ă��邾��
 * @note  Cmd_TLCD_CLEAR_ALL_TIMELINE ��GS���瑗���Ă��������C TL No ���}�W�b�N�i���o�[�ɂȂ�̂ŁD
 */
CCP_EXEC_STS Cmd_TLM_MGR_CLEAR_TLM_TL(const CTCP* packet);

/**
 * @brief HK�e������o�^
 */
CCP_EXEC_STS Cmd_TLM_MGR_REGISTER_HK_TLM(const CTCP* packet);

/**
 * @brief system�e������o�^
 */
CCP_EXEC_STS Cmd_TLM_MGR_REGISTER_SYSTEM_TLM(const CTCP* packet);

/**
 * @brief high_freq_tlm ��o�^
 */
CCP_EXEC_STS Cmd_TLM_MGR_REGISTER_HIGH_FREQ_TLM(const CTCP* packet);

/**
 * @brief low_freq_tlm ��o�^
 */
CCP_EXEC_STS Cmd_TLM_MGR_REGISTER_LOW_FREQ_TLM(const CTCP* packet);


// TODO: �����Ȃ�ݒ肪�ς��̂ł͂Ȃ��C�ݒ�ύX �� ���f�C�ɂ������D
// CCP_EXEC_STS Cmd_TLM_MGR_APPLY(const CTCP* packet);


// *** HOW TO USE ***
/*
1. �������FCmd_TLM_MGR_INIT
  - BC�������邽�߁CApp�̏������ł͌����_�łł��Ȃ��̂ŁC�����I�ɏ���������K�v����
    - initial SL�ɂ���Ă���������
2. �񃆁[�U�[�e�����̓o�^
  - HK, SYSTEM TLM��o�^���Ă��� ( Cmd_TLM_MGR_REGISTER_HOGE �� )
3. �e�������o�J�n
  - ����͔O�̈� Cmd_TLM_MGR_CLEAR_TLM_TL �����āC������TL2������
  - Cmd_TLM_MGR_START_TLM �ɂăe�������o�J�n
    - �~�߂�̂� Cmd_TLM_MGR_STOP_TLM
4. Cmd_TLM_MGR_REGISTER_HIGH_FREQ_TLM / Cmd_TLM_MGR_REGISTER_LOW_FREQ_TLM �ɂāC1 Hz, 1/10 Hz �̃��[�U�[�e�������e�l���D���ɓo�^�ł���
5. �������I������� Cmd_TLM_MGR_CLEAR_USER_TLM �����āC���[�U�[�e����������
6. ���Ɏg���l��������C4.����J��Ԃ�

*/

#endif
