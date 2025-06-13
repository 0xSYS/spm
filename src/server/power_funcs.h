#pragma once






enum power_action
{
  Poweroff,
  Reboot,
  Standby,
  Hibernate
};

// Deprecated already
//void SysPowerOff();
//void SysReboot();
//void SysStandby();
void SysAction(enum power_action pa);
