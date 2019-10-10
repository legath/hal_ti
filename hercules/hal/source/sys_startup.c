/** @file sys_startup.c
*   @brief Startup Source File
*   @date 11-Dec-2018
*   @version 04.07.01
*
*   This file contains:
*   - Include Files
*   - Type Definitions
*   - External Functions
*   - VIM RAM Setup
*   - Startup Routine
*   .
*   which are relevant for the Startup.
*/

/*
* Copyright (C) 2009-2018 Texas Instruments Incorporated - www.ti.com
*
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

/* Include Files */

#include "../include/sys_common.h"
#include "../include/system.h"
#include "../include/sys_vim.h"
#include "../include/sys_core.h"
#include "../include/sys_selftest.h"
#include "../include/esm.h"
#include "../include/mibspi.h"

#include "../include/errata_SSWF021_45.h"

/* Definitions */
#define PLL_RETRIES 5U

/* Functions */
void handlePLLLockFail(void);

/* SourceId : STARTUP_SourceId_001 */
/* DesignId : STARTUP_DesignId_001 */
/* Requirements : HL_SR508 */
__attribute__ ((naked)) void _c_int00(void)
{
    /* Save the return address to a reserved register */
    register volatile void *reserve_r11 __asm("r11") __attribute__((unused));
    __asm volatile("mov r11, lr");

    /* Enable CPU Event Export */
    /* This allows the CPU to signal any single-bit or double-bit errors detected
     * by its ECC logic for accesses to program flash or data RAM.
     */
    _coreEnableEventBusExport_();

        /* Workaround for Errata CORTEXR4 66 */
        _errata_CORTEXR4_66_();

        /* Workaround for Errata CORTEXR4 57 */
        _errata_CORTEXR4_57_();

    /* Reset handler: the following instructions read from the system exception status register
     * to identify the cause of the CPU reset.
     */

    /* check for power-on reset condition */
    /*SAFETYMCUSW 139 S MR:13.7 <APPROVED> "Hardware status bit read check" */
    if ((SYS_EXCEPTION & POWERON_RESET) != 0U)
    {
        /* Add condition to check whether PLL can be started successfully */
        if (_errata_SSWF021_45_both_plls(PLL_RETRIES) != 0U)
        {
            /* Put system in a safe state */
            handlePLLLockFail();
        }
        /* clear all reset status flags */
        SYS_EXCEPTION = 0xFFFFU;

      /* continue with normal start-up sequence */
    }
    /*SAFETYMCUSW 139 S MR:13.7 <APPROVED> "Hardware status bit read check" */
    else if ((SYS_EXCEPTION & OSC_FAILURE_RESET) != 0U)
    {
        /* Reset caused due to oscillator failure.
        Add user code here to handle oscillator failure */

        /* TODO: Wire this up to an appropriate OS handler */
    }
    /*SAFETYMCUSW 139 S MR:13.7 <APPROVED> "Hardware status bit read check" */
    else if ((SYS_EXCEPTION & WATCHDOG_RESET) !=0U)
    {
        /* Reset caused due
         *  1) windowed watchdog violation - Add user code here to handle watchdog violation.
         *  2) ICEPICK Reset - After loading code via CCS / System Reset through CCS
         */
        /* Check the WatchDog Status register */
        if(WATCHDOG_STATUS != 0U)
        {
            /* Add user code here to handle watchdog violation. */
            /* TODO: Wire this up to an appropriate OS handler */

            /* Clear the Watchdog reset flag in Exception Status register */
            SYS_EXCEPTION = WATCHDOG_RESET;
        }
        else
        {
            /* Clear the ICEPICK reset flag in Exception Status register */
            SYS_EXCEPTION = ICEPICK_RESET;
        }
    }
    /*SAFETYMCUSW 139 S MR:13.7 <APPROVED> "Hardware status bit read check" */
    else if ((SYS_EXCEPTION & CPU_RESET) !=0U)
    {
        /* Reset caused due to CPU reset.
        CPU reset can be caused by CPU self-test completion, or
        by toggling the "CPU RESET" bit of the CPU Reset Control Register. */

        /* clear all reset status flags */
        SYS_EXCEPTION = CPU_RESET;
    }
    /*SAFETYMCUSW 139 S MR:13.7 <APPROVED> "Hardware status bit read check" */
    else if ((SYS_EXCEPTION & SW_RESET) != 0U)
    {
        /* Reset caused due to software reset.
        Add user code to handle software reset. */

        /* TODO: Wire this up to an appropriate OS handler */
    }
    else
    {
        /* Reset caused by nRST being driven low externally.
        Add user code to handle external reset. */

        /* TODO: Wire this up to an appropriate OS handler */
    }

    /* Check if there were ESM group3 errors during power-up.
     * These could occur during eFuse auto-load or during reads from flash OTP
     * during power-up. Device operation is not reliable and not recommended
     * in this case.
     * An ESM group3 error only drives the nERROR pin low. An external circuit
     * that monitors the nERROR pin must take the appropriate action to ensure that
     * the system is placed in a safe state, as determined by the application.
     */
    if ((esmREG->SR1[2]) != 0U)
    {
    /*SAFETYMCUSW 5 C MR:NA <APPROVED> "for(;;) can be removed by adding "# if 0" and "# endif" in the user codes above and below" */
    /*SAFETYMCUSW 26 S MR:NA <APPROVED> "for(;;) can be removed by adding "# if 0" and "# endif" in the user codes above and below" */
    /*SAFETYMCUSW 28 D MR:NA <APPROVED> "for(;;) can be removed by adding "# if 0" and "# endif" in the user codes above and below" */
        for(;;)
        {
        }/* Wait */
    }

    /* Initialize System - Clock, Flash settings with Efuse self check */
    systemInit();

    /* Initialize CPU RAM.
     * This function uses the system module's hardware for auto-initialization of memories and their
     * associated protection schemes. The CPU RAM is initialized by setting bit 0 of the MSIENA register.
     * Hence the value 0x1 passed to the function.
     * This function will initialize the entire CPU RAM and the corresponding ECC locations.
     */
    memoryInit(0x1U);

    /* Enable ECC checking for TCRAM accesses.
     * This function enables the CPU's ECC logic for accesses to B0TCM and B1TCM.
     */
    _coreEnableRamEcc_();

    /* Return to the caller */
    __asm("bx r11");
}

/** @fn void handlePLLLockFail(void)
*   @brief This function handles PLL lock fail.
*/
void handlePLLLockFail(void)
{
    while(1)
    {

    }
}
