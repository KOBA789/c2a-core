#ifndef AH_RULES_H_
#define AH_RULES_H_
/**
 * @file   ah_rules.h
 * @brief  AH�̃��[�U�[��`������Core���番��
 */

// �����_�ŁC�ő吔��128�I
typedef enum
{
  AH_RULE_SAMPLE,
} AH_RULE_NUM;

enum
{
  AL_RULE_INACTIVE = 0,
  AL_RULE_ACTIVE   = 1
};

void AH_Rule_default_load(void);

#endif
