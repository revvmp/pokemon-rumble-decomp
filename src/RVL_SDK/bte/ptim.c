#include <revolution/bte/bta_sys.h>
#include <revolution/bte/ptim.h>

void ptim_init(tPTIM_CB *p_cb, UINT16 period, UINT8 timer_id)
{
	GKI_init_timer_list(&p_cb->timer_queue);
	p_cb->period = period;
	p_cb->timer_id = timer_id;
}

void ptim_timer_update(tPTIM_CB *p_cb)
{
	TIMER_LIST_ENT *p_tle;
	BT_HDR *p_msg;

	GKI_update_timer_list(&p_cb->timer_queue, p_cb->period);

	while (p_cb->timer_queue.p_first && p_cb->timer_queue.p_first->ticks <= 0)
	{
		p_tle = p_cb->timer_queue.p_first;
		GKI_remove_from_timer_list(&p_cb->timer_queue, p_tle);

		if (p_tle->p_cback)
		{
			(*p_tle->p_cback)(p_tle);
		}
		else if (p_tle->event && (p_msg = GKI_getbuf(sizeof *p_msg)) != NULL)
		{
			p_msg->event = p_tle->event;
			bta_sys_sendmsg(p_msg);
		}
	}

	if (!p_cb->timer_queue.p_first)
		GKI_stop_timer(p_cb->timer_id);
}

void ptim_start_timer(tPTIM_CB *p_cb, TIMER_LIST_ENT *p_tle, UINT16 type,
                      INT32 timeout)
{
	if (!p_cb->timer_queue.p_first)
		GKI_start_timer(p_cb->timer_id, GKI_MS_TO_TICKS(p_cb->period), TRUE);

	GKI_remove_from_timer_list(&p_cb->timer_queue, p_tle);

	p_tle->event = type;
	p_tle->ticks = timeout;

	GKI_add_to_timer_list(&p_cb->timer_queue, p_tle);
}

void ptim_stop_timer(tPTIM_CB *p_cb, TIMER_LIST_ENT *p_tle)
{
	GKI_remove_from_timer_list(&p_cb->timer_queue, p_tle);

	if (!p_cb->timer_queue.p_first)
		GKI_stop_timer(p_cb->timer_id);
}
