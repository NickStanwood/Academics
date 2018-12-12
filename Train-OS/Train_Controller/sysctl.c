/*================================================================================*
 * TITLE:	sysctl.c															  *
 * PURPOSE:		*
 * AUTHOR:	Emad Khan, Modified by Nick Stanwood								  *
 * CREATED:	April 2, 2012														  *
 * REVISION:	*
 * ===============================================================================*/

#include "sysctl.h"

struct SYSCTL_RCGC1 * const SYSCTL_RCGC1_R = (struct SYSCTL_RCGC1 *) 0x400FE104;

struct SYSCTL_RCGC2 * const SYSCTL_RCGC2_R  = (struct SYSCTL_RCGC2 *) 0x400FE108;
