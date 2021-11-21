#pragma section REPRO
/**
 * @file   block_command_loader.c
 * @brief  �u���b�N�R�}���h�o�^�֘A�̃��[�e�B���e�B
 *
 *         �{�@�\�̓u���b�N�R�}���h�̓o�^����wrap���Cuser���Ȍ������S�Ȍ`�Ńu���b�N
 *         �R�}���h�o�^���s����悤�ɂ��邽�߂̃��[�e�B���e�B�@�\�ł��D
 */

#include <string.h>
#include "block_command_loader.h"
#include "../Applications/timeline_command_dispatcher.h"
#include "block_command_executor.h"
#include <src_user/CmdTlm/command_definitions.h> // for rotate/combine block
#include "common_tlm_cmd_packet_util.h"

#define BCL_PARAM_MAX_LENGTH BCT_CMD_MAX_LENGTH

static void BCL_register_cmd_(cycle_t ti, CMD_CODE cmd_id);
static void BCL_register_app_(cycle_t ti, AR_APP_ID app_id);
static void BCL_clear_info_(void);


/**
 * @brief  �u���b�N�R�}���h�o�^���̏����ꎞ�ۑ����邽�߂̃o�b�t�@
 */
typedef struct
{
  CTCP packet;                                //!< �o�^���̃R�}���h���ꎞ�ۑ����邽�߂̃o�b�t�@
  uint8_t params[BCL_PARAM_MAX_LENGTH];       /*!< �o�^���̃p�����[�^���ꎞ�ۑ����邽�߂̃o�b�t�@
                                                   ���������y�ɂ��邽�߂Ɉ�R�}���h���ÓI�Ɋm�ۂ��Ă���
                                                   TODO: �T�C�Y���ߏ肩������Ȃ��̂Ŏ��Ԃ���Β��� */
  int param_idx;                              //!< params�z��̒��Ŏ��Ƀp�����[�^��o�^����\���index
} BlockCommandLoader;

static BlockCommandLoader block_command_loader_;


void BCL_load_bc(bct_id_t pos, void (*BCL_load_func)(void))
{
  // �ŏ��ɃS�~�f�[�^�������Ă���
  BCL_clear_info_();

  BCT_store_pos();
  BCT_clear_block(pos);

  BCL_load_func();

  // TODO: load���e�̌��؂�����
  //       ���s���G���[�Ȃ̂ŁC�����ςȂ̂͂Ƃ肠�����N�������ăA�m�}���Ŋm�F���H

  BCE_activate_block();
  BCT_restore_pos();
}

#if 0
void BCL_safe_load_tl(bct_id_t pos, void (*BCL_load_func)(void))
{
  // pass
}

void BCL_safe_load_sl(bct_id_t pos, void (*BCL_load_func)(void))
{
  // pass
}
#endif

void BCL_tool_register_cmd(cycle_t ti, CMD_CODE cmd_id)
{
  BCL_register_cmd_(ti, cmd_id);
  BCL_clear_info_();
}

void BCL_tool_register_rotate(cycle_t ti, bct_id_t bct_id)
{
#if SIZE_OF_BCT_ID_T == 1
  BCL_tool_prepare_param_uint8(bct_id);
#elif SIZE_OF_BCT_ID_T == 2
  BCL_tool_prepare_param_uint16(bct_id);
#elif SIZE_OF_BCT_ID_T == 4
  BCL_tool_prepare_param_uint32(bct_id);
#else
#error Illegal value for SIZE_OF_BCT_ID_T
#endif

  BCL_register_cmd_(ti, Cmd_CODE_BCT_ROTATE_BLOCK);
  BCL_clear_info_();
}

void BCL_tool_register_combine(cycle_t ti, bct_id_t bct_id)
{
#if SIZE_OF_BCT_ID_T == 1
  BCL_tool_prepare_param_uint8(bct_id);
#elif SIZE_OF_BCT_ID_T == 2
  BCL_tool_prepare_param_uint16(bct_id);
#elif SIZE_OF_BCT_ID_T == 4
  BCL_tool_prepare_param_uint32(bct_id);
#else
#error Illegal value for SIZE_OF_BCT_ID_T
#endif

  BCL_register_cmd_(ti, Cmd_CODE_BCT_COMBINE_BLOCK);
  BCL_clear_info_();
}

void BCL_tool_register_limit_combine(cycle_t ti, bct_id_t bct_id, step_t limit_step)
{
#if SIZE_OF_BCT_ID_T == 1
  BCL_tool_prepare_param_uint8(bct_id);
#elif SIZE_OF_BCT_ID_T == 2
  BCL_tool_prepare_param_uint16(bct_id);
#elif SIZE_OF_BCT_ID_T == 4
  BCL_tool_prepare_param_uint32(bct_id);
#else
#error Illegal value for SIZE_OF_BCT_ID_T
#endif

  BCL_tool_prepare_param_uint8((uint8_t)limit_step);

  BCL_register_cmd_(ti, Cmd_CODE_BCT_TIMELIMIT_COMBINE_BLOCK);
  BCL_clear_info_();
}

void BCL_tool_register_deploy(cycle_t ti, bct_id_t bct_id, TL_ID tl_id)
{
  BCL_tool_prepare_param_uint8((uint8_t)tl_id);

#if SIZE_OF_BCT_ID_T == 1
  BCL_tool_prepare_param_uint8(bct_id);
#elif SIZE_OF_BCT_ID_T == 2
  BCL_tool_prepare_param_uint16(bct_id);
#elif SIZE_OF_BCT_ID_T == 4
  BCL_tool_prepare_param_uint32(bct_id);
#else
#error Illegal value for SIZE_OF_BCT_ID_T
#endif

  BCL_register_cmd_(ti, Cmd_CODE_TLCD_DEPLOY_BLOCK);
  BCL_clear_info_();
}

void BCL_tool_register_app(cycle_t ti, AR_APP_ID app_id)
{
  BCL_register_app_(ti, app_id);
  BCL_clear_info_();
}

void BCL_tool_prepare_param_uint8(uint8_t val)
{
  block_command_loader_.params[block_command_loader_.param_idx++] = (uint8_t)((val      ) & 0xff);
}

void BCL_tool_prepare_param_int8(int8_t val)
{
  block_command_loader_.params[block_command_loader_.param_idx++] = (uint8_t)((val      ) & 0xff);
}

void BCL_tool_prepare_param_uint16(uint16_t val)
{
  block_command_loader_.params[block_command_loader_.param_idx++] = (uint8_t)((val >>  8) & 0xff);
  block_command_loader_.params[block_command_loader_.param_idx++] = (uint8_t)((val      ) & 0xff);
}

void BCL_tool_prepare_param_int16(int16_t val)
{
  block_command_loader_.params[block_command_loader_.param_idx++] = (uint8_t)((val >>  8) & 0xff);
  block_command_loader_.params[block_command_loader_.param_idx++] = (uint8_t)((val      ) & 0xff);
}

void BCL_tool_prepare_param_uint32(uint32_t val)
{
  block_command_loader_.params[block_command_loader_.param_idx++] = (uint8_t)((val >> 24) & 0xff);
  block_command_loader_.params[block_command_loader_.param_idx++] = (uint8_t)((val >> 16) & 0xff);
  block_command_loader_.params[block_command_loader_.param_idx++] = (uint8_t)((val >>  8) & 0xff);
  block_command_loader_.params[block_command_loader_.param_idx++] = (uint8_t)((val      ) & 0xff);
}

void BCL_tool_prepare_param_int32(int32_t val)
{
  block_command_loader_.params[block_command_loader_.param_idx++] = (uint8_t)((val >> 24) & 0xff);
  block_command_loader_.params[block_command_loader_.param_idx++] = (uint8_t)((val >> 16) & 0xff);
  block_command_loader_.params[block_command_loader_.param_idx++] = (uint8_t)((val >>  8) & 0xff);
  block_command_loader_.params[block_command_loader_.param_idx++] = (uint8_t)((val      ) & 0xff);
}

void BCL_tool_prepare_param_float(float val)
{
  uint32_t tmp = *((uint32_t*)&val);

  block_command_loader_.params[block_command_loader_.param_idx++] = (uint8_t)((tmp >> 24) & 0xff);
  block_command_loader_.params[block_command_loader_.param_idx++] = (uint8_t)((tmp >> 16) & 0xff);
  block_command_loader_.params[block_command_loader_.param_idx++] = (uint8_t)((tmp >>  8) & 0xff);
  block_command_loader_.params[block_command_loader_.param_idx++] = (uint8_t)((tmp      ) & 0xff);
}

// �������_�ł�uint64_t���g���Ȃ��������g���l�����Ȃ��̂ł������񖳌���
#if 0
void BCL_tool_prepare_param_double(double val)
{
  int i;

  // TODO: idx���߂�assertion������
  for (i = 0; i < sizeof(val); i++)
  {
    block_command_loader_.param_buffer[block_command_loader_.command_idx][block_command_loader_.param_idx++]
        = (unsigned char)((val >> (sizeof(val) - i) * 8) & 0xff);
  }
}
#endif


void BCL_register_cmd_(cycle_t ti, CMD_CODE cmd_id)
{
  CCP_form_tlc(&block_command_loader_.packet,
               ti,
               cmd_id,
               &block_command_loader_.params[0],
               (uint16_t)block_command_loader_.param_idx);
  BCT_register_cmd(&block_command_loader_.packet);
}

void BCL_register_app_(cycle_t ti, AR_APP_ID app_id)
{
  CCP_form_app_cmd(&block_command_loader_.packet, ti, app_id);
  BCT_register_cmd(&block_command_loader_.packet);
}

static void BCL_clear_info_(void)
{
  memset(&block_command_loader_, 0, sizeof(block_command_loader_));
}

#pragma section
