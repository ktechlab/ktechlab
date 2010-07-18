
find_file( KDEV_IPLUGIN_H kdevplatform/interfaces/iplugin.h )

if(KDEV_IPLUGIN_H)
	file(STRINGS ${KDEV_IPLUGIN_H} KDEV_PLUGIN_VERSION
		REGEX "#define KDEVELOP_PLUGIN_VERSION.*" )
	string(REGEX REPLACE ".*\\ ([0-9]+)"
		"\\1" KDEV_PLUGIN_VERSION ${KDEV_PLUGIN_VERSION})
endif(KDEV_IPLUGIN_H)

