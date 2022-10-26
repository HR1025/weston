if not get_option('backend-custom')
	subdir_done()
endif

config_h.set('BUILD_CUSTOM_COMPOSITOR', '1')

deps_custom = [
	dependency('libudev', required : true),
	# FIXME : bad solution, weston dependence
	dep_libdl,
	dep_libweston_private,
	dep_session_helper,
	dep_libdrm,
	dep_libinput_backend,
	dep_backlight,
]

srcs_custom = [
	'custom.cpp',
	'CustomBackend.h',
	'CustomBackend.cpp',
	'IRender.h',
	'IRender.cpp',
	'Render/PixmanRender.h',
	'Render/PixmanRender.cpp',
	'CustomDefine.h',
	'InputDevice.h',
	'InputDevice.cpp',
	'IImageWriter.h',
	'ImageWriterDispatcher.h',
	'ImageWriterDispatcher.cpp',
	'ImageWriterFactory.h',
	'ImageWriterFactory.cpp',
	'ImageWriter/FileImageWriter.h',
	'ImageWriter/FileImageWriter.cpp',
	'ImageWriter/DummyImageWriter.h',
	# include directly, no need to write there
	# 'custom_wrapper.cpp', 
	presentation_time_server_protocol_h,
]

if get_option('backend-custom-dump')
	config_h.set('BUILD_BACKEND_CUSTOM_DUMP', '1')
endif

# Hint : meson 通过 pkgconifg 来查找依赖, Poco 本身并不提供 .pc 文件, 需要自己手写一个
dep_poco = dependency('Poco', required: false)
if not dep_poco.found()
	config_h.set('BUILD_WITH_POCO', '0')
else
	config_h.set('BUILD_WITH_POCO', '1')
	deps_custom += dep_poco
	srcs_custom += [
		'ImageWriter/ShmImageWriter.h',
		'ImageWriter/ShmImageWriter.cpp',
	]
endif

plugin_custom = shared_library(
	'custom-backend',
	srcs_custom,
	include_directories: common_inc,
	dependencies: [ dep_libweston_private, dep_libdrm_headers, deps_custom ],
	name_prefix: '',
	install: true,
	install_dir: dir_module_libweston,
)
env_modmap += 'custom-backend.so=@0@;'.format(plugin_custom.full_path())
install_headers(backend_custom_h, subdir: dir_include_libweston_install)
