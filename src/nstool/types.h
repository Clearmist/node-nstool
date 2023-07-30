#pragma once
#include <tc/types.h>
#include <tc/Exception.h>
#include <tc/Optional.h>
#include <tc/io.h>
#include <tc/io/IOUtil.h>
#include <tc/cli.h>
#include <fmt/core.h>

namespace nstool {

struct CliOutputMode
{
	bool show_basic_info;
	bool show_extended_info;
	bool show_layout;
	bool show_keydata;
    bool show_json;

	CliOutputMode() : show_basic_info(false), show_extended_info(false), show_layout(false), show_keydata(false), show_json(false)
	{}

	CliOutputMode(bool show_basic_info, bool show_extended_info, bool show_layout, bool show_keydata, bool show_json) : show_basic_info(show_basic_info), show_extended_info(show_extended_info), show_layout(show_layout), show_keydata(show_keydata), show_json(show_json)
	{}
};

struct ExtractJob {
	tc::io::Path virtual_path;
	tc::io::Path extract_path;
};

}
