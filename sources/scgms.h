#pragma once
#ifdef __cplusplus
extern "C" {
#endif
const char * get_config_data();
int build_filter_chain(const char* configuration); 
void create_level_event(int level_input);
#ifdef __cplusplus
}
#endif
