#ifndef _INIFILE_HPP_
#define _INIFILE_HPP_

#include <map>
#include <string>
#include <glib.h>

#include "config_file.h"

class inifile : public config_file {
public:
	inifile();
	~inifile();
	/* read_only = true guarantees that the ini file will not be changed.
	 * if strict = true, check ini file version. */
	bool load(const std::string& path, bool read_only = false, bool strict = true);
	bool read_bool(const gchar *sect, const gchar *key, bool& val);
	bool read_int(const gchar *sect, const gchar *key, int& val);
	bool read_string(const gchar * sect, const gchar *key, std::string& val);
	bool read_strlist(const gchar * sect, const gchar *key, std::list<std::string>& slist);

	void write_bool(const gchar *sect, const gchar *key, bool val);
	void write_int(const gchar *sect, const gchar *key, int val);
	void write_string(const gchar *sect, const gchar *key, const std::string& val);
	void write_strlist(const gchar *sect, const gchar *key, const std::list<std::string>& slist);

	void notify_add(const gchar *sect, const gchar *key, 
			const sigc::slot<void, const baseconfval*>&);

private:
	typedef std::map<std::string, sigc::signal<void, const baseconfval *> >
		ChangeEventsMap;
	enum CheckResult { crOK, crError, crRetry };
	ChangeEventsMap change_events_map_;
	GKeyFile *gkeyfile_;
	std::string fname_;
	bool read_only_;

	template <typename T>
	void expose_event(const gchar *sect, const gchar *key, const T& val) {
		std::string name = std::string(sect) + "/" + key;
		ChangeEventsMap::iterator p = change_events_map_.find(name);
		if (p == change_events_map_.end())
			return;
		confval<T> cv(val);
		p->second.emit(&cv);
	}
	bool save();
	bool fix_and_save();
	bool convert_from_locale_enc();
	CheckResult check_config_version();
	CheckResult load_ini_file();
};

#endif//!_INIFILE_HPP_