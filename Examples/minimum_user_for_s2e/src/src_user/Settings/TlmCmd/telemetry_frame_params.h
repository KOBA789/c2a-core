/**
 * @file
 * @brief  TF��CoreTlm�p�����^�̃I�[�o�[���C�h�p�w�b�_�[
 */
#ifndef TELEMETRY_FRAME_PARAMS_H_
#define TELEMETRY_FRAME_PARAMS_H_

#undef TF_TLM_PAGE_SIZE
#undef TF_TLM_PAGE_MAX
#undef TF_MAX_TLMS

// 1�e�����g���p�P�b�g(=1�y�[�W)�Ɋi�[�����e�����g����
#define TF_TLM_PAGE_SIZE (64)
// �e�����g���e�[�u���y�[�W��
#define TF_TLM_PAGE_MAX (4)
// �R�}���h�e�[�u���T�C�Y
#define TF_MAX_TLMS (TF_TLM_PAGE_SIZE * TF_TLM_PAGE_MAX)

#endif
