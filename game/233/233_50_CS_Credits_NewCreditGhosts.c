#include <common.h>

int CS_Credits_NewCreditGhosts(void)
{
	struct Model *model = creditsBSS.dancerInst_invisible->model;
	int i;

	for (i = 0; i < 5; i++)
	{
		if (creditsBSS.creditsObj.creditGhostModel[i] != model)
			return 0;
	}

	return 1;
}
