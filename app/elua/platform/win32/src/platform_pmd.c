
#include <stdio.h>

#include "lplatform.h"
#include "platform_pmd.h"

int platform_pmd_init(PlatformPmdCfg *pmdCfg)
{
#define PRINT_FIELD(fIELD) printf("[platform_pmd_init]: " #fIELD " %d\n", pmdCfg->fIELD)

    PRINT_FIELD(battFullLevel);
    PRINT_FIELD(battRechargeLevel);
    PRINT_FIELD(poweronLevel);
    PRINT_FIELD(poweroffLevel);

    PRINT_FIELD(currentFirst);
    PRINT_FIELD(battlevelFirst);
    PRINT_FIELD(currentSecond);
    PRINT_FIELD(battlevelSecond);
    PRINT_FIELD(currentThird);

    return PLATFORM_OK;
}

int platform_ldo_set(PlatformLdoId id, int level)
{
    //printf("[platform_ldo_set]: %d %d\r\n", id, level);

    return PLATFORM_OK;
}

int platform_pmd_powersave(int sleep_wake)
{
    //printf("[platform_pmd_powersave]: %d\r\n", sleep_wake);

    return PLATFORM_OK;
}

int platform_pmd_get_charger(void)
{
    static int charger = 0;

    charger = !charger;

    return 1;
}