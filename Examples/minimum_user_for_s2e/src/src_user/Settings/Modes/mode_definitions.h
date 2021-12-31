/**
 * @file
 * @brief ���[�h��`�ƃ��[�h�J�ڒ�`
 */
#ifndef MODE_DEFINISIION_H_
#define MODE_DEFINISIION_H_

typedef enum
{
  MD_MODEID_START_UP          = 0,          //  0: �N������
  MD_MODEID_INITIAL           = 1,          //  1: �N����A�y�у��[�h�؂�ւ��̎n�_
  MD_MODEID_RESERVED_1        = 14,         // 14: ���R�Ɏg�p�ł��郂�[�h
  MD_MODEID_RESERVED_2        = 15,         // 15: ���R�Ɏg�p�ł��郂�[�h
  MD_MODEID_MODE_MAX                        //
} MD_MODEID;

void MD_load_mode_list(void);
void MD_load_transition_table(void);

#endif
