


#include "dbg.h"
#include "power_funcs.h"



#include <string.h>
#include <systemd/sd-bus.h>
#include <stdio.h>



/*
// Again deprecated
void SysPowerOff()
{
	sd_bus *bus = NULL;
	sd_bus_error error = SD_BUS_ERROR_NULL;
	int r;

	r = sd_bus_open_system(&bus);
	if (r < 0)
	{
	  Log(Err, "Failed to connect to system bus: %s\n", strerror(-r));
	}

  r = sd_bus_call_method(bus, "org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "PowerOff", &error, NULL, "b", 0);

  if (r < 0)
  {
	  Log(Err, "Failed to execute PowerOff: %s\n", error.message);
	}

  sd_bus_error_free(&error);
  sd_bus_unref(bus);                
}

void SysReboot()
{
	sd_bus *bus = NULL;
	sd_bus_error error = SD_BUS_ERROR_NULL;
	int r;

	r = sd_bus_open_system(&bus);
	if (r < 0)
	{
	  Log(Err, "Failed to connect to system dbus: %s\n", strerror(-r));
	}

	r = sd_bus_call_method(bus, "org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "Reboot", &error, NULL, "b", 0);

	if (r < 0)
	{
	  Log(Err, "Failed to execute Reboot: %s\n", error.message);
	}

	sd_bus_error_free(&error);
	sd_bus_unref(bus);
}

void SysStandby()
{
  sd_bus *bus = NULL;
	sd_bus_error error = SD_BUS_ERROR_NULL;
	int r;
 
	r = sd_bus_open_system(&bus);
	if (r < 0)
	{
	  Log(Err, "Failed to connect to system dbus: %s\n", strerror(-r));
	}
 
	r = sd_bus_call_method(bus, "org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "Suspend", &error, NULL, "b", 0);
 
	if (r < 0)
	{
	  Log(Err, "Failed to execute Suspend: %s\n", error.message);
	}
 
	sd_bus_error_free(&error);
	sd_bus_unref(bus);
}
*/


void SysAction(enum power_action pa)
{
  sd_bus *bus = NULL;
	sd_bus_error error = SD_BUS_ERROR_NULL;
	int r;
 
	r = sd_bus_open_system(&bus);
	if (r < 0)
	{
	  Log(Err, "Failed to connect to system dbus: %s\n", strerror(-r));
	}
	else
	{
	  if(pa == Poweroff)
		{
		  r = sd_bus_call_method(bus, "org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "PowerOff", &error, NULL, "b", 0);
		}
		else if(pa == Reboot)
		{
		  r = sd_bus_call_method(bus, "org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "Reboot", &error, NULL, "b", 0);
		}
		else if(pa == Standby)
		{
		  // This dosen't seem to be working proberly wtf
		  r = sd_bus_call_method(bus, "org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "Suspend", &error, NULL, "b", 0);
		}
		else if(pa == Hibernate)
		{
		  r = sd_bus_call_method(bus, "org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "Hibernate", &error, NULL, "b", 0);
		}
			
		if (r < 0)
		{
		  Log(Err, "Failed to execute power action: %s\n", error.message);
		}
		
		sd_bus_error_free(&error);
		sd_bus_unref(bus);
	}
}