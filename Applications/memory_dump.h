#ifndef MEMORY_DUMP_H_
#define MEMORY_DUMP_H_

#include "../System/ApplicationManager/app_info.h"
#include "../TlmCmd/common_tlm_cmd_packet.h"

#define MEM_TLM_ID      (0xff)
#define MEM_DUMP_WIDTH  (CTCP_MAX_LEN - 18)     // �e�����p�P�b�g�ő咷 - �w�b�_��
#define MEM_MAX_SPAN    (0x00800000)            // �ő�_���v����16MB(ADU�T�C�Y����)

typedef enum
{
  MEM_SUCCESS,
  MEM_NO_DATA
} MEM_ACK;

typedef struct
{
  uint32_t begin;
  uint32_t end;
  uint32_t adu_size;
  uint16_t adu_seq;
  uint8_t  adu_counter;
  uint32_t dest;
  uint32_t rp;
} MemoryDump;

extern const MemoryDump* const memory_dump;

AppInfo MEM_create_app(void);


// 2018/08/24
// �����̉��߂��R�����g�Ƃ��Ēǉ�
CCP_EXEC_STS Cmd_MEM_SET_REGION(const CTCP* packet);

CCP_EXEC_STS Cmd_MEM_DUMP_REGION_SEQ(const CTCP* packet);
// 1�p�P�b�g�ɓ���؂�Ȃ��ꍇ�́C�ŏ���ADU�������ꂽ�ŏ��̃p�P�b�g�̂݃_���v
// ������x����ƁC���̎��̃p�P�b�g���_���v
// �Ō�͂����Ǝ~�܂�

CCP_EXEC_STS Cmd_MEM_DUMP_REGION_RND(const CTCP* packet);
// ADU�������ꂽ�ꍇ�C���̓r���̃p�P�b�g����_���v

CCP_EXEC_STS Cmd_MEM_DUMP_SINGLE(const CTCP* packet);
// �A�h���X���w�肵�āC�_���v�H
// Cmd_MEM_SET_REGION �͖����H

CCP_EXEC_STS Cmd_MEM_LOAD(const CTCP* packet);
// MEM�ɃA�b�v�����N���ď�������

CCP_EXEC_STS Cmd_MEM_SET_DESTINATION(const CTCP* packet);
// Cmd_MEM_COPY_REGION_SEQ�̃R�s�[����w��

CCP_EXEC_STS Cmd_MEM_COPY_REGION_SEQ(const CTCP* packet);
// dest��rp���w�蕝�����R�s�[���Ă���
// �����Cmd_MEM_DUMP_REGION_SEQ�Ɠ��l�ɁC���x���J��Ԃ����s���Ďg���D

#endif
