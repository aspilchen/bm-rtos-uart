
#include "prm.h"
#include "hw_types.h"      // For HWREG(...) macro
#include <stdint.h>

/******************************************************************************
 **              INTERNAL MACRO DEFINITIONS
 ******************************************************************************/
#define PRM_DEVICE      (0x44E00F00)
#define PRM_RSTST_OFFSET (0x8)
#define PRM_RSTST_REG    (PRM_DEVICE + PRM_RSTST_OFFSET)

/******************************************************************************
 **              INTERNAL FUNCTION PROTOTYPES
 ******************************************************************************/
static uint32_t prm_readRstst(void);

/******************************************************************************
 **              FUNCTION DEFINITIONS
 ******************************************************************************/
bool prm_isSet(const enum PrmRststEvent resetEvent)
{
	volatile uint32_t regVal = prm_readRstst();
	uint32_t bitVal = regVal & resetEvent;
	bool isSet = bitVal > 0;
	return isSet;
}

// Set all unreserved bits to 1 for clear
void prm_clear(void)
{
	const uint32_t CLEAR_MASK = 0x00000000
		| ICEPICK_RST 
		| EXTERNAL_WARM_RST 
		| WDT1_RST
		| GLOBAL_WARM_SW_RST
		| GLOBAL_COLD_RST;

	HWREG(PRM_RSTST_REG) |= CLEAR_MASK;
}

/******************************************************************************
 **              INTERNAL FUNCTIONS
 ******************************************************************************/
static uint32_t prm_readRstst(void)
{
	volatile uint32_t regVal = HWREG(PRM_RSTST_REG);
	return regVal;
}