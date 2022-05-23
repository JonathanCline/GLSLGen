

macro(DEFINE_GLAD_TARGET in_VersionMajor in_VersionMinor)
	set(__glVersion ${in_VersionMajor}_${in_VersionMinor})
	set(__name glad_${__glVersion})
	
	set(__pinc ${__glVersion}/include)
	set(__psrc ${__glVersion}/src)
	
	add_library(${__name} STATIC ${__pinc}/glad/glad.h ${__psrc}/glad.c ${__pinc}/KHR/khrplatform.h)
	target_include_directories(${__name} PUBLIC "${__pinc}")

	set(out_Name ${__name})
endmacro()



DEFINE_GLAD_TARGET(4 6)

add_library(glad ALIAS glad_4_6)
