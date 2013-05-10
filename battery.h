enum e_BatteryParameters 
{
	BATTERY_CHARGE_MIN = 0,
	BATTERY_CHARGE_MAX,
};

char OS_InitBattery(void);
int OS_GetBatteryLevel(void);

void BATT_Calibrate(enum e_BatteryParameters parameter);