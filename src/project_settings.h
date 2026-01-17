#pragma once

#include <godot_cpp/classes/project_settings.hpp>

namespace godot {

// Not a macro any longer.
Variant _GLOBAL_DEF(const String &p_var, const Variant &p_default, bool p_restart_if_changed = false, bool p_ignore_value_in_docs = false, bool p_basic = false, bool p_internal = false);
Variant _GLOBAL_DEF(const PropertyInfo &p_info, const Variant &p_default, bool p_restart_if_changed = false, bool p_ignore_value_in_docs = false, bool p_basic = false, bool p_internal = false);

#define GLOBAL_DEF(m_var, m_value) _GLOBAL_DEF(m_var, m_value)
#define GLOBAL_DEF_RST(m_var, m_value) _GLOBAL_DEF(m_var, m_value, true)
#define GLOBAL_DEF_NOVAL(m_var, m_value) _GLOBAL_DEF(m_var, m_value, false, true)
#define GLOBAL_DEF_RST_NOVAL(m_var, m_value) _GLOBAL_DEF(m_var, m_value, true, true)
#define GLOBAL_GET(m_var) ProjectSettings::get_singleton()->get_setting_with_override(m_var)

#define GLOBAL_DEF_BASIC(m_var, m_value) _GLOBAL_DEF(m_var, m_value, false, false, true)
#define GLOBAL_DEF_RST_BASIC(m_var, m_value) _GLOBAL_DEF(m_var, m_value, true, false, true)
#define GLOBAL_DEF_NOVAL_BASIC(m_var, m_value) _GLOBAL_DEF(m_var, m_value, false, true, true)
#define GLOBAL_DEF_RST_NOVAL_BASIC(m_var, m_value) _GLOBAL_DEF(m_var, m_value, true, true, true)

#define GLOBAL_DEF_INTERNAL(m_var, m_value) _GLOBAL_DEF(m_var, m_value, false, false, false, true)

/////////////////////////////////////////////////////////////////////////////////////////
// Cached versions of GLOBAL_GET.
// Cached but uses a typed variable for storage, this can be more efficient.
// Variables prefixed with _ggc_ to avoid shadowing warnings.
#define GLOBAL_GET_CACHED(m_type, m_setting_name) ([](const char *p_name) -> m_type {\
static_assert(std::is_trivially_destructible<m_type>::value, "GLOBAL_GET_CACHED must use a trivial type that allows static lifetime.");\
static m_type _ggc_local_var;\
static uint32_t _ggc_local_version = 0;\
static SpinLock _ggc_spin;\
uint32_t _ggc_new_version = ProjectSettings::get_singleton()->get_version();\
if (_ggc_local_version != _ggc_new_version) {\
	_ggc_spin.lock();\
	_ggc_local_version = _ggc_new_version;\
	_ggc_local_var = ProjectSettings::get_singleton()->get_setting_with_override(p_name);\
	m_type _ggc_temp = _ggc_local_var;\
	_ggc_spin.unlock();\
	return _ggc_temp;\
}\
_ggc_spin.lock();\
m_type _ggc_temp2 = _ggc_local_var;\
_ggc_spin.unlock();\
return _ggc_temp2; })(m_setting_name)


};