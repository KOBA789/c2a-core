/**
 * @file   majority_vote_for3.h
 * @brief  3�d�璷�p�ėp�������֐�
 */
#ifndef MAJORITY_VOTE_FOR3_
#define MAJORITY_VOTE_FOR3_

#include <stdio.h>
#include <src_user/Library/stdint.h>

typedef enum
{
  MAJORITY_VOTE_MATCH    =  0,
  MAJORITY_VOTE_MAJORITY =  1,
  MAJORITY_VOTE_ERROR    =  2,
} MAJORITY_VOTE_FOR3_STATUS;

/**
 * @brief 3�d�璷�p�ėp�������֐� (�R�s�[)
 *
 *        3�d�璷�p�z����󂯎���đ��������s�������ʂ�res�ɓ���, �璷�̏�Ԃ�Ԃ�
 * @param[in] array1_arg ~ array3_arg ���肵����3�d�璷
 * @param[in] type_len sizeof(�^)������
 * @param[in] length �z��̒���, �������ł����̂�1�����Ȃ̂őS��������, �P�̂̏ꍇ��1��
 * @param[out] res_arg �Ԃ�l�̃|�C���^, ���炩���ߊm�ۂ��Ă����悤��
 * @retval MAJORITY_VOTE_ERROR:    �S�s��v�����Ȃ��Ƃ�1����, �ُ�
 * @retval MAJORITY_VOTE_MAJORITY: 2:1�����Ȃ��Ƃ�1����,     ����
 * @retval MAJORITY_VOTE_MATCH:    �S�Ăɂ��đS��v,         ����
 * @note �z��ɑ΂��čs����̂�, ������index�ő��������s���Ă��Ă�MAJORITY_VOTE_MAJORITY���Ԃ�܂�.
 */
MAJORITY_VOTE_FOR3_STATUS majority_vote_for3_copy(void* res_arg,
                                                  void* array1_arg,
                                                  void* array2_arg,
                                                  void* array3_arg,
                                                  size_t type_len,
                                                  uint32_t length);

/**
 * @brief 3�d�璷�p�ėp�������֐� (����)
 *
 *        3�d�璷�p�z����󂯎���đ��������s���A�S�Ă���v��������Ԃɂ���
 * @param[in] array1_arg ~ array3_arg ���肵����3�d�璷
 * @param[in] type_len sizeof(�^)������
 * @param[in] length �z��̒���, �������ł����̂�1�����Ȃ̂őS��������, �P�̂̏ꍇ��1��
 * @retval MAJORITY_VOTE_ERROR:    �S�s��v�����Ȃ��Ƃ�1����, �ُ�
 * @retval MAJORITY_VOTE_MAJORITY: 2:1�����Ȃ��Ƃ�1����,      ����
 * @retval MAJORITY_VOTE_MATCH:    �S�Ăɂ��đS��v,          ����
 * @note �z��ɑ΂��čs����̂�, ������index�ő��������s���Ă��Ă�MAJORITY_VOTE_MAJORITY���Ԃ�܂�.
 *       �܂�, ������g���Γ��������̂ŎO�d�璷���ꂽ���̂� [0] �̃|�C���^������đ���ł��܂�.
 */
MAJORITY_VOTE_FOR3_STATUS majority_vote_for3_sync(void* array1_arg,
                                                  void* array2_arg,
                                                  void* array3_arg,
                                                  size_t type_len,
                                                  uint32_t length);

#endif
