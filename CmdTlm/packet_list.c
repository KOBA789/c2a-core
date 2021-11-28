#pragma section REPRO
/**
 * @file  packet_list.c
 * @brief CTCP�̃��X�g�Ƃ��Ẵf�[�^�\�����`. �Е������X�g�Ƃ��Ď�������Ă���
 */
#include "packet_list.h"

#include "../System/TimeManager/time_manager.h"
#include "block_command_executor.h"
#include "block_command_table.h"
#include <src_user/Library/stdint.h>

/**
 * @brief �擪�m�[�h�𗎂Ƃ�
 * @param[in] pli: PacketList
 * @return void
 */
static void PL_drop_head_(PacketList* pli);

/**
 * @brief ���g�p�m�[�h��V�K�Ɏ擾
 * @param[in] pli: PacketList
 * @return PL_Node*
 */
static PL_Node* PL_get_free_node_(PacketList* pli);

void PL_initialize(PL_Node* stock, size_t size, PacketList* pli)
{
  pli->total_nodes_ = (uint16_t)size;
  pli->executed_nodes_ = 0;

  pli->stock_ = stock;
  PL_clear_list(pli);
}

// private�Ȋe���X�g��A�����X�g�`�� or NULL�ŏ���������B
void PL_clear_list(PacketList* pli)
{
  int i;

  // PL_node�̔z���A�����X�g�Ƃ��čĒ�`
  for (i = 0; i < pli->total_nodes_ - 1; ++i)
  {
    pli->stock_[i].next = &(pli->stock_[i + 1]);
  }

  pli->stock_[pli->total_nodes_ - 1].next = NULL;

  // �ŏ��͑Snode���������
  pli->active_nodes_ = 0;
  pli->inactive_list_head_ = pli->stock_;
  pli->active_list_head_ = NULL;
  pli->active_list_tail_ = NULL;
}

uint32_t PL_count_executed_nodes(const PacketList* pli)
{
  return pli->executed_nodes_;
}

uint16_t PL_count_active_nodes(const PacketList* pli)
{
  return pli->active_nodes_;
}

uint16_t PL_count_inactive_nodes(const PacketList* pli)
{
  return (uint16_t)(pli->total_nodes_ - pli->active_nodes_);
}

int PL_is_empty(const PacketList* pli)
{
  return (pli->active_list_head_ == NULL);
}

int PL_is_full(const PacketList* pli)
{
  return (pli->inactive_list_head_ == NULL);
}

const PL_Node* PL_get_head(const PacketList* pli)
{
  return pli->active_list_head_;
}

const PL_Node* PL_get_tail(const PacketList* pli)
{
  return pli->active_list_tail_;
}

const PL_Node* PL_get_next(const PL_Node* node)
{
  if (node != NULL) return node->next;

  return NULL;
}

PL_ACK PL_push_front(PacketList* pli, const CTCP* packet)
{
  PL_Node* new_pl_node = PL_get_free_node_(pli);

  if (new_pl_node == NULL) return PL_LIST_FULL;

  CTCP_copy_packet(&new_pl_node->packet, packet);

  new_pl_node->next = pli->active_list_head_;
  pli->active_list_head_ = new_pl_node;

  if (pli->active_list_tail_ == NULL)
  {
    pli->active_list_tail_ = new_pl_node;
  }

  ++pli->active_nodes_;

  return PL_SUCCESS;
}

PL_ACK PL_push_back(PacketList* pli, const CTCP* packet)
{
  PL_Node* new_pl_node = PL_get_free_node_(pli);

  if (new_pl_node == NULL) return PL_LIST_FULL;

  // �V�R�[�h
  // ���̃R�[�h���ƁCpacket���ׂĂł͂Ȃ��K�v�Œ���݂̂����R�s�[���Ȃ�
  CTCP_copy_packet(&(new_pl_node->packet), packet);

  // ���R�[�h
  // �������񓯎��� push back ����ƒx��
  // �S�̈�R�s�[����D
  // CTCP �� packet �Ƃ��������o�������Ƃ����肵�Ȃ��ėǂ�
  // new_pl_node->packet = *packet;

  new_pl_node->next = NULL;

  if (pli->active_list_tail_ == NULL)
  {
    pli->active_list_head_ = new_pl_node;
  }
  else
  {
    pli->active_list_tail_->next = new_pl_node;
  }

  pli->active_list_tail_ = new_pl_node;
  ++pli->active_nodes_;

  return PL_SUCCESS;
}

PL_ACK PL_insert_after(PacketList* pli, PL_Node* pos, const CTCP* packet)
{
  PL_Node* new_pl_node;

  if (pos == NULL) return PL_NO_SUCH_NODE;

  if (pos == pli->active_list_tail_) return PL_push_back(pli, packet);

  new_pl_node = PL_get_free_node_(pli);

  if (new_pl_node == NULL) return PL_LIST_FULL;

  CTCP_copy_packet(&new_pl_node->packet, packet);

  new_pl_node->next = pos->next;
  pos->next = new_pl_node;

  ++pli->active_nodes_;

  return PL_SUCCESS;
}

PL_ACK PL_insert_tl_cmd(PacketList* pli, const CTCP* packet, uint32_t now)
{
  cycle_t head, tail;
  cycle_t planed = CCP_get_ti(packet);

  if (now > planed) return PL_TLC_PAST_TIME; // �w����s���Ԃ����ɉ߂��Ă���
  if (PL_is_full(pli)) return PL_LIST_FULL;  // �o�^�]�T���Ȃ�

  // �����o�^����Ă��Ȃ�
  if (PL_is_empty(pli)) return PL_push_front(pli, packet);

  // ���R�}���h���o�^����Ă���
  head = CCP_get_ti(&(PL_get_head(pli)->packet));
  tail = CCP_get_ti(&(PL_get_tail(pli)->packet));

  if (tail < planed) // ���̂ǂ���x��
  {
    return PL_push_back(pli, packet);
  }
  else if (head > planed) // ���̂ǂ��葁��
  {
    return PL_push_front(pli, packet);
  }
  else if (head == planed || tail == planed) // �����w�肪������
  {
    return PL_TLC_ALREADY_EXISTS;
  }
  else // ���R�}���h�̊Ԃɑ}�����삪�K�v
  {
    int i;
    PL_Node* prev = (PL_Node*)PL_get_head(pli); // const_cast
    PL_Node* curr = prev->next;

    // �}���ꏊ�T��
    for (i = 1; i < pli->active_nodes_; ++i)
    {
      cycle_t curr_ti = CCP_get_ti(&(curr->packet));

      if (curr_ti < planed)
      {
        prev = curr;
        curr = prev->next;
      }
      else if (curr_ti > planed) // �}���ꏊ����
      {
        PL_insert_after(pli, prev, packet);
        break;
      }
      else // ���o�^�R�}���h�Ǝ����w�肪������
      {
        return PL_TLC_ALREADY_EXISTS;
      }
    }

    if (i == pli->active_nodes_)
    {
      // NOT REACHED
      return PL_NO_SUCH_NODE;
    }
  }

  return PL_SUCCESS;
}

PL_ACK PL_deploy_block_cmd(PacketList* pli, const bct_id_t block, uint32_t start_at)
{
  int i, j;
  int is_cleared = 0; // ���X�g�����N���A�̋L�^�p�ϐ�
  uint32_t adj = 0; // ���������̗ݐϗʕۑ��p�ϐ�
  uint8_t length;

  if (block >= BCT_MAX_BLOCKS) return PL_BC_INACTIVE_BLOCK;
  length = BCT_get_bc_length(block);

  if (!BCE_is_active(block)) return PL_BC_INACTIVE_BLOCK;

  // ���X�g�Ƀu���b�N�S�̂�o�^����]�T���Ȃ��ꍇ
  if (PL_count_inactive_nodes(pli) < length)
  {
    // ���X�g���N���A�������I�ɋ󂫗̈���m�ۂ���B
    PL_clear_list(pli);
    is_cleared = 1;
  }

  for (i = 0; i < length; ++i)
  {
    static CTCP temp_; // �T�C�Y���傫�����ߐÓI�̈�Ɋm��
    BCT_Pos pos;
    PL_ACK ack = PL_SUCCESS;

    // �R�}���h��ǂ݂����ATLC�Ƃ��Ď��s������ݒ�B
    pos.block = block;
    pos.cmd = (uint8_t)i;
    BCT_load_cmd(&pos, &temp_);
    CCP_set_ti(&temp_, (cycle_t)(start_at + adj + CCP_get_ti(&temp_)));
    CCP_set_exec_type(&temp_, CCP_EXEC_TYPE_TL0); // BLC -> TLC     // FIXME: TaskList��TL012�����ׂ� CCP_EXEC_TYPE_TL0 �ɂȂ��Ă��܂����C�킩��ɂ����Ȃ��H

    for (j = 0; j <= pli->active_nodes_; ++j)
    {
      // �R�}���h��TLC�ɓo�^�����݂�
      ack = PL_insert_tl_cmd(pli, &temp_, start_at);
      if (ack != PL_TLC_ALREADY_EXISTS) break;

      // ���ꎞ���Ŋ��ɓo�^����Ă����ꍇ�͎��������炵�čēo�^
      CCP_set_ti(&temp_, CCP_get_ti(&temp_) + 1);
      ++adj; // �ݐϒ������Ԃ��X�V����
    }
    if (ack != PL_SUCCESS) return ack;
  }

  // ���X�g�̋����N���A�����{�����ꍇ�B
  if (is_cleared == 1) return PL_BC_LIST_CLEARED;
  // �����������s�����ꍇ�B
  if (adj != 0) return PL_BC_TIME_ADJUSTED;

  return PL_SUCCESS;
}

PL_ACK PL_check_tl_cmd(PacketList* pli, uint32_t time)
{
  if (!PL_is_empty(pli)) // �R�}���h���X�g����łȂ�
  {
    const CTCP* packet = &pli->active_list_head_->packet;
    uint32_t planed = CCP_get_ti(packet);

    if (time == planed) return PL_TLC_ON_TIME;
    if (time > planed)  return PL_TLC_PAST_TIME;
  }

  return PL_TLC_NOT_YET;
}

void PL_drop_executed(PacketList* pli)
{
  PL_drop_head_(pli);
  ++(pli->executed_nodes_);
}

void PL_drop_node(PacketList* pli, PL_Node* prev, PL_Node* current)
{
  if (current == NULL) return;
  if (pli->active_list_head_ == NULL) return;
  if (prev == NULL)
  {
    PL_drop_head_(pli);
    return;
  }

  prev->next = current->next;
  if (pli->active_list_tail_ == current)
  {
    pli->active_list_tail_ = prev;
  }

  current->next = pli->inactive_list_head_;
  pli->inactive_list_head_ = current;
  --pli->active_nodes_;
}

static void PL_drop_head_(PacketList* pli)
{
  PL_Node* temp;

  if (pli->active_list_head_ == NULL) return;

  temp = pli->active_list_head_;
  pli->active_list_head_ = temp->next;

  if (pli->active_list_head_ == NULL)
  {
    pli->active_list_tail_ = NULL;
  }

  temp->next = pli->inactive_list_head_;
  pli->inactive_list_head_ = temp;

  --pli->active_nodes_;
}

static PL_Node* PL_get_free_node_(PacketList* pli)
{
  PL_Node* free_node = pli->inactive_list_head_;

  if (free_node == NULL) return NULL;

  pli->inactive_list_head_ = pli->inactive_list_head_->next;

  return free_node;
}

#pragma section
