#include <scgms/rtl/FilterLib.h>
#include <scgms/rtl/referencedImpl.h>
#include <scgms/utils/winapi_mapping.h>
#include <scgms/utils/string_utils.h>
#include <filters/config.h>
#include "scgms.h"

#if defined(FREERTOS)
#include <uart_print.h>
#endif

#if defined(WASM) || defined (ESP32)
void print(const char * str)
{
	printf("%s\n", str);
}
#endif

scgms::SFilter_Executor Global_Filter_Executor;

const char * get_config_data()
{
	return config_data;
}

void create_level_event(int level_input)
{
	scgms::UDevice_Event event{ scgms::NDevice_Event_Code::Level };
	event.level() = level_input;
	if(Global_Filter_Executor)
	{
		Global_Filter_Executor.Execute(std::move(event));
	}
}

void create_shutdown_event()
{
	scgms::UDevice_Event event{ scgms::NDevice_Event_Code::Shut_Down };
	if(Global_Filter_Executor)
	{
		Global_Filter_Executor.Execute(std::move(event));
	}
}

int build_filter_chain(const char*  configuration_input)
{
	const char* config;
	if(configuration_input == NULL)
	{
		configuration_input = config_data;
		print("Creating SCGMS filter chain from config.h");	
	}
	else
	{
		print("Creating SCGMS filter chain from input");	
	}
	print("------------------------------------------");
	print("Configuration:");
	print("------------------------------------------");
	refcnt::Swstr_list errors = refcnt::Swstr_list{};
	errors.get();
	scgms::SPersistent_Filter_Chain_Configuration configuration{};
	if (configuration == NULL)
	{
		print("Failed to construct SPersistent_Filter_Chain_Configuration");
		return -1;
	}

	print(configuration_input);
	print("------------------------------------------");

	print("Config errors:");
	configuration->Load_From_Memory(configuration_input, strlen(configuration_input), errors.get());
	print("------------------------------------------");

	print("Filter executor errors:");
	Global_Filter_Executor = scgms::SFilter_Executor{ configuration.get(), nullptr, nullptr, errors };
	bool success = true;
	errors.for_each([](auto str) {auto newstr = Narrow_WString(str);print(newstr.c_str());success = false;});
	print("------------------------------------------");

	if(Global_Filter_Executor && success)
	{
		print("Filter chain is ready to execute:");
		print("------------------------------------------");
	}
	else
	{
		print("Error constructing filter chain");
		print("------------------------------------------");
		return -1;
	}

	return 0;
}