add_rules("mode.debug", "mode.release")

set_languages("c++17")




option("ansi-escapes")
  set_default(false)
  set_showmenu(true)
  set_description("Enable ansi escapes used in coloring the text from stdout.")

option("no-msgbox")
  set_default(false)
  set_showmenu(true)
  set_description("Disable all message boxes for non graphical builds (The strings are redirected to stdout instead)")

option("debug-function-calls")
  set_default(false)
  set_showmenu(true)
  set_description("Show extra debugging information (Source file, function name, line)")

option("single-line-config")
  set_default(false)
  set_showmenu(true)
  set_description("Write json config on a single line")


target("spm")
  set_kind("shared")
  set_options("ansi-escapes")
  set_options("no-msgbox")
  set_options("debug-function-calls")
  set_options("single-line-config")

    
  if has_config("ansi-escapes") then
		add_defines("ANSI_ESCAPES")
	end

	if has_config("no-msgbox") then
	  add_defines("NO_MSGBOX")
	end

	if has_config("debug-function-calls") then
	  add_defines("DEBUG_FN_CALLS")
	end
	
	if has_config("single-line-config") then
	  add_defines("SINGLE_LINE_CONFIG")
	end

  add_includedirs("externals/inc/")
  add_files("src/*.cpp")
  if is_plat("windows") then
    set_toolchains("msvc")
    add_syslinks("ws2_32", "user32", "iphlpapi")
  end

target("spm-serv")
  if is_plat("windows") then
    set_default(false)
  end
  set_kind("binary")
  add_includedirs("externals/inc/")
  add_defines("_GNU_SOURCE", "__EXTENSIONS__", "HAVE_SIGABBREV_NP", "HAVE_STDIO_EXT_H")
  add_files("src/server/*.c", "externals/inc/procps-ng/*.c")
  -- add_links("procps")
  add_links("systemd")

target("test")
  -- set_enabled(false)
  set_kind("binary")
  set_options("ansi-escapes")
  set_options("no-msgbox")
  set_options("debug-function-calls")

  if has_config("no-msgbox") then
    add_defines("NO_MSGBOX")
  end

  if has_config("ansi-escapes") then
    add_defines("ANSI_ESCAPES")
  end

  if has_config("debug-function-calls") then                                                                                                                                         
    add_defines("DEBUG_FN_CALLS")                                                                                                                                                   
  end
  
  if has_config("single-line-config") then
	  add_defines("SINGLE_LINE_CONFIG")
	end

  add_includedirs("externals/inc/")

  add_files("src/*.cpp", "src/test/main.cpp")

  if is_plat("windows") then
    set_toolchains("msvc")
    add_syslinks("ws2_32", "user32", "iphlpapi")
  end

--target("spm-ui")
--    set_default(false)
--    set_options("debug-function-calls")
--
--    if has_config("debug-function-calls") then
--       add_defines("DEBUG_FN_CALLS")
--    end
--
--    add_defines("wxDEBUG_LEVEL=2")
--    
--    set_kind("binary")
--    --add_packages("wxwidgets")
--     -- Run wx-config to get flags
--    on_load(function (target)
--        local cxxflags = os.iorun("wx-config --cxxflags")
--        local ldflags  = os.iorun("wx-config --libs")
--
--        -- Apply the flags
--        target:add("cxxflags", cxxflags, {force = true})
--        target:add("ldflags", ldflags, {force = true})
--    end)
--
--    add_includedirs("externals/inc/")
--    add_files("src/*.cpp", "src/frontend/frontend.cpp")
--