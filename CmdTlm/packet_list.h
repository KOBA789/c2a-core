/**
 * @file  packet_list.h
 * @brief CTCP�̃��X�g�Ƃ��Ẵf�[�^�\�����`. �Е������X�g�Ƃ��Ď�������Ă���
 */
#ifndef PACKET_LIST_H_
#define PACKET_LIST_H_

#include <stddef.h>

#include "common_tlm_cmd_packet.h"
#include "block_command_table.h"

/**
 * @struct PL_NodeTag
 * @brief  �Е������X�g���\������e�m�[�h
 */
struct PL_NodeTag
{
  CTCP packet;
  struct PL_NodeTag* next;
};
typedef struct PL_NodeTag PL_Node;

/**
 * @struct PacketList
 * @brief  �p�P�b�g���X�g�{��
 * @note   �f�[�^�\���Ƃ��Ă� �Е������X�g
 *         �������ł͊O���Ŏ��O�ɗ̈悾���m�ۂ��Ă�����PL_Node�z��̐擪�|�C���^��stock�ɕۑ����ĘA�����X�g��, inactive_list_head�ɂ��R�s�[����.
           �g���Ƃ��ɂ�inactive_list����PL_Node������o����, ���̗̈��active_list�ɑ}������.
           �Ȃ������o�[�͑S�� private
 */
typedef struct
{
  uint16_t total_nodes_;        //!< �S�m�[�h�� (static �m��)
  uint32_t executed_nodes_;     //!< ���s���ꂽ�m�[�h��
  uint16_t active_nodes_;       //!< ���ݕЕ������X�g�ɓ����Ă���m�[�h��

  PL_Node* stock_;              //!< �m�ۂ���Ă���̈�S�̂̐擪
  PL_Node* inactive_list_head_; //!< �m�ۂ���Ă���̈�̓��A�g���Ă��Ȃ����̂̐擪, stack
  PL_Node* active_list_head_;   //!< �m�ۂ���Ă���̈�̓��A�g���Ă�����̂̐擪
  PL_Node* active_list_tail_;   //!< �m�ۂ���Ă���̈�̓��A�g���Ă�����̖̂��[
} PacketList;

/**
 * @enum  PL_ACK
 * @brief PacketList �֘A����̃G���[�R�[�h
 * @note  uint8_t
 */
typedef enum
{
  PL_SUCCESS,            //!< ����
  PL_LIST_FULL,          //!< PacketList ����t (inactive ����)
  PL_TLC_PAST_TIME,      //!< ���s���Ԋ��Ɍo��
  PL_TLC_ALREADY_EXISTS, //!< �������Ɋ��� Node ������
  PL_TLC_ON_TIME,        //!< ���s�������x
  PL_TLC_NOT_YET,        //!< �܂����s�����ł͂Ȃ�
  PL_BC_INACTIVE_BLOCK,  //!< ������ BC
  PL_BC_LIST_CLEARED,    //!< PL �N���A (������, active �S�폜) ���ꂽ
  PL_BC_TIME_ADJUSTED,   //!< �������� Node �����������ߒ������ꂽ
  PL_NO_SUCH_NODE        //!< ����� Node �͖���
} PL_ACK;

/**
 * @brief static �Ɋm�ۂ��ꂽ PL_Node �z����󂯎�肻�̗̈���g�p���� PL ��������
 * @param[in] stock: �g�p���� PL_Node �z��
 * @param[in] size_t: �z��̃T�C�Y
 * @param[out] pli: ���������� PacketList
 * @return void
 */
void PL_initialize(PL_Node* stock, size_t size, PacketList* pli);

/**
 * @brief PacketList ���N���A
 * @param[in] pli: �N���A���� PacketList
 * @return void
 * @note �S active Node ���폜���� �S�� inactive �� stock �ɂ���
 */
void PL_clear_list(PacketList* pli);

/**
 * @brief PacketList �Ŏ��s���ꂽ�m�[�h����Ԃ�
 * @param[in] pli: PacketList
 * @return uint32_t: ���s���ꂽ�m�[�h��
 */
uint32_t PL_count_executed_nodes(const PacketList* pli);

/**
 * @brief PacketList �ŗL����(���s�҂�)�m�[�h����Ԃ�
 * @param[in] pli: PacketList
 * @return uint16_t: �L����(���s�҂�)�m�[�h��
 */
uint16_t PL_count_active_nodes(const PacketList* pli);

/**
 * @brief PacketList �Ŏg�p����Ă��Ȃ��m�[�h����Ԃ�
 * @param[in] pli: PacketList
 * @return uint16_t: �g�p����Ă��Ȃ��m�[�h��
 */
uint16_t PL_count_inactive_nodes(const PacketList* pli);

/**
 * @brief PacketList ���󂩂ǂ���
 * @param[in] pli: PacketList
 * @return int 1: True 0: False
 */
int PL_is_empty(const PacketList* pli);

/**
 * @brief PacketList ����t���ǂ���
 * @param[in] pli: PacketList
 * @return int 1: True 0: False
 */
int PL_is_full(const PacketList* pli);

/**
 * @brief PacketList �� active �擪�m�[�h���擾
 * @param[in] pli: PacketList
 * @return const PL_Node*
 */
const PL_Node* PL_get_head(const PacketList* pli);

/**
 * @brief PacketList �� active ���[�m�[�h���擾
 * @param[in] pli: PacketList
 * @return const PL_Node*
 */
const PL_Node* PL_get_tail(const PacketList* pli);

/**
 * @brief ����Node ���擾
 * @param[in] node: ���m�[�h
 * @return const PL_Node*
 */
const PL_Node* PL_get_next(const PL_Node* node);

/**
 * @brief PacketList �̐擪�� packet ��}��
 * @param[in] pli: PacketList
 * @param[in] packet: �}������ packet
 * @return PL_ACK
 */
PL_ACK PL_push_front(PacketList* pli, const CTCP* packet);

/**
 * @brief PacketList �̖����� packet ��}��
 * @param[in] pli: PacketList
 * @param[in] packet: �}������ packet
 * @return PL_ACK
 */
PL_ACK PL_push_back(PacketList* pli, const CTCP* packet);

/**
 * @brief ���� Node �̒���� packet ��}��
 * @param[in] pli: PacketList
 * @param[in] pos: ����ɑ}������� packet
 * @param[in] packet: �}������ packet
 * @return PL_ACK
 */
PL_ACK PL_insert_after(PacketList* pli, PL_Node* pos, const CTCP* packet);

/**
 * @brief ���� Node �̒���� packet ��}��
 * @param[in] pli: PacketList
 * @param[in] packet: �}������ packet
 * @param[in] now: ����̎w����s����
 * @return PL_ACK
 * @note TaskList �� TimeLine ��������g���̂� now �� uint32_t
 */
PL_ACK PL_insert_tl_cmd(PacketList* pli, const CTCP* packet, uint32_t now);

/**
 * @brief PacketList ��� BC ��W�J����
 * @param[in] pli: PacketList
 * @param[in] block: �W�J���� BC �� ID
 * @param[in] start_at: �J�n�����
 * @return PL_ACK
 * @note TaskList �� TimeLine ��������g���̂� start_at �� uint32_t
 */
PL_ACK PL_deploy_block_cmd(PacketList* pli, const bct_id_t block, uint32_t start_at);

/**
 * @brief PacketList �̐擪�� time ���r
 * @param[in] pli: PacketList
 * @param[in] time: ��r���鎞��
 * @return PL_ACK
 * @note TaskList �� TimeLine ��������g���̂� start_at �� time
 */
PL_ACK PL_check_tl_cmd(PacketList* pli, uint32_t time);

/**
 * @brief �擪�m�[�h�𗎂Ƃ�
 * @param[in] pli: PacketList
 * @return void
 */
void PL_drop_executed(PacketList* pli);

/**
 * @brief �w�肳�ꂽ�m�[�h�𗎂Ƃ�
 * @param[in] pli: PacketList
 * @param[in] prev: ���Ƃ����O Node
 * @param[in] current: ���Ƃ� Node
 * @return void
 */
void PL_drop_node(PacketList* pli, PL_Node* prev, PL_Node* current);

#endif
