#include <common.h>

int CS_Credits_IsTextValid(void)
{
	struct CreditsObj *creditsObj = &creditsBSS.creditsObj;

	if (creditsObj->epilogue_topString != 0)
		return 0;

	creditsObj->countdown = 360;
	return 1;
}
