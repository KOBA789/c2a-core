#ifndef BLOCK_COMMAND_USER_SETTINGS_H_
#define BLOCK_COMMAND_USER_SETTINGS_H_

#include <src_core/TlmCmd/block_command_table.h>
#include <src_core/TlmCmd/block_command_executor.h>

#define BCUS_MRAM_BCT_BLOCKS (10) // MRAM�Ɋm�ۂ���

/**
 * @brief BCT, BCE �Ɋւ��Ẵ������m�ۂ� getter, setter �� user settings �Ƃ��ď㏑������
 * @param[in] block_command_table: ���������� BlockCommandTable*
 * @param[in] block_command_executor: ���������� BlockCommandExecutor*
 * @note SRAM, �P��ł͂Ȃ� MRAM��, �O�d�璷�Œu���Ȃǂ̍ۂɏ㏑���Ŏg�p
 */
void BCUS_load_user_settings(BlockCommandTable* block_command_table, BlockCommandExecutor* block_command_executor);

/**
 * @brief ���� block ���N���A���ׂ����ǂ������ʂ���
 * @param[in] block: BC �� id
 * @return 0: No, 1: Yes
 * @note �s�����ł���� clear ���Ȃ�. �����ł���� clear ����.
 */
int BCUS_bc_should_clear(const bct_id_t block);

#endif
