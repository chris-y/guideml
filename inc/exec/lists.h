struct MinNode
{
	struct MinNode* mln_Succ;
	struct MinNode* mln_Pred;
};

struct MinList
{
	struct MinNode* mlh_Head;
	struct MinNode* mlh_Tail;
	struct MinNode* mlh_TailPred;
};

#define List MinList
#define Node MinNode

static inline void AddTail(struct MinList* list, struct MinNode* entry)
{
	struct MinNode* last = (struct MinNode*)list;

	while(last->mln_Succ->mln_Succ) last = last->mln_Succ;
	entry->mln_Succ = (struct MinNode*)&(list->mlh_Tail);
	entry->mln_Pred = last;

	last->mln_Succ = entry;
	list->mlh_TailPred = entry;
}

static inline void NewList(struct MinList* head)
{
	head->mlh_Head = (struct MinNode*)&(head->mlh_Tail);
	head->mlh_TailPred = (struct MinNode*)&(head->mlh_Head);
	head->mlh_Tail = NULL;
}

static inline void Remove(struct MinNode* node)
{
	node->mln_Pred->mln_Succ = node->mln_Succ;
	if(node->mln_Succ)
		node->mln_Succ->mln_Pred = node->mln_Pred;
}
