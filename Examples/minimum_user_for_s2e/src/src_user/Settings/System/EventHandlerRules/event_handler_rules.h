/**
 * @file   event_handler_rules.h
 * @brief  EH �� Rule ���ʃw�b�_
 */
#ifndef EVENT_HANDLER_RULES_H_
#define EVENT_HANDLER_RULES_H_

/**
 * @enum  AH_RULE_ID
 * @brief EH_Rule �� ID
 * @note  �ő吔�� EH_RULE_MAX �ŋK��
 * @note  uint16_t ��z��
 */
typedef enum
{
  EH_RULE_TEST0 = 100,
  EH_RULE_TEST1 = 101,
  EH_RULE_TEST2 = 102,
  EH_RULE_TEST3 = 103,
  EH_RULE_TEST4 = 104,
  EH_RULE_TEST5 = 105,
  EH_RULE_TEST6 = 106,
  EH_RULE_TEST7 = 107
} EH_RULE_ID;


/**
 * @brief  event_handler �̃f�t�H���g���[����ǂݍ���
 * @param  void
 * @return void
 */
void EH_load_default_rules(void);

// �ȉ��C���[�U�[��` ���[���o�^�֐��錾
void EH_load_rule_test(void);

#endif
