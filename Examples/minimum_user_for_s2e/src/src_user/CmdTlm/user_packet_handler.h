/**
 * @file
 * @brief  packet_handler�̃R�}���h���[�^�[�C�R�}���h�A�i���C�U�̃��[�U�[��`����
 */
#ifndef USER_PACKET_HANDLER_H_
#define USER_PACKET_HANDLER_H_

#include <src_core/CmdTlm/packet_handler.h>
#include <src_core/CmdTlm/common_tlm_cmd_packet.h>

extern PacketList PH_aobc_cmd_list;
extern PacketList PH_tobc_cmd_list;

/**
 * @brief  PH�̃��[�U�[�ŗL������������
 *
 *         PacketList�̏��������s��
 * @param  void
 * @return void
 */
void PH_user_init(void);

/**
 * @brief  PH��analyze_cmd_�̃��[�U�[�����֐�
 *
 *         Core�̏������D�悳���
 *         C2A�𓋍ڂ����R���|�̃L���[�iGSC�CRTC�CTLC�CBC�j�ɑ����ɒ��ڒǉ������D
 *         ���m�ɂ́Cadd_rt_cmd_�̑��M��ł����C�����ɃL���[�C���O����CDI (dispatcher) �ɂ���ĉ�������D
 *         �܂�C�{OBC�ɂƂ��Ă�RTC�����C�]���Ώ�OBC�ɂƂ��Ă͊e��Cmd������Cmd��]�����鏈��
 * @param  *packet CTCP packet
 * @retval PH_FORWARDED: �����ɓ]������L���[�ɒǉ����ꂽ
 * @retval PH_PL_LIST_FULL: PL LIST FULL
 * @retval PH_FORWARDED�ȊO�iPH_UNKNOWN��z��j: ���[�U�[�����Ɉ���������Ȃ������ꍇ�DCore���̏����ɉ񂳂��
 */
PH_ACK PH_user_analyze_cmd(const CTCP* packet);

/**
 * @brief  PH��cmd_router_�̃��[�U�[�����֐�
 *
 *         C2A�𓋍ڂ����R���|��Cmd���]�������D
 *         �܂�C�]���Ώ�OBC�ɂƂ��Ă�RTC�����ɂȂ�
 *         ��������Driver��@���đ��M�܂ōs�����ƂɂȂ�i���s���Ԃ�cmdExec�Ɠ����������e����Ă���̂�OK�j
 * @param  *packet CTCP packet
 * @retval CCP_EXEC_SUCCESS �Ȃ�:   �����ɓ]�����ꂽ�D�]����̌��ʂ�Ԃ�
 * @retval CCP_EXEC_ROUTING_FAILED: �]�����s�i�ڍ׃G���[��DriverSuper���Q�Ɓj
 */
CCP_EXEC_STS PH_user_cmd_router(const CTCP* packet);

#endif
