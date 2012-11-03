# -*- cmake -*-

set(FMOD OFF CACHE BOOL "Use FMOD sound library.")

if (FMOD)
  if (STANDALONE)
    if (ARCH MATCHES "x86_64")
      MESSAGE(FATAL_ERROR "fmod not available for 64-bit. fmod can be disabled by passing -DFMOD=OFF to cmake configure step")
    endif (ARCH MATCHES "x86_64")
    set(FMOD_FIND_REQUIRED ON)
    include(FindFMOD)
  else (STANDALONE)
    include(Prebuilt)
    use_prebuilt_binary(fmod)
    
    if (WINDOWS)
      set(FMOD_LIBRARY ${CMAKE_SOURCE_DIR}/../fmodapi375win/api/lib/fmodvc.lib)
    elseif (DARWIN)
      if (ARCH MATCHES "i386")
        set(FMOD_LIBRARY ${CMAKE_SOURCE_DIR}/../fmodapi375mac/api/lib/libfmodx86.a)
      else (ARCH MATCHES "i386")
        set(FMOD_LIBRARY ${CMAKE_SOURCE_DIR}/../fmodapi375mac/api/lib/libfmod.a)
      endif (ARCH MATCHES "i386")
    elseif (LINUX)
      set(FMOD_LIBRARY ${CMAKE_SOURCE_DIR}/../fmodapi375linux/api/libfmod-3.75.so)
    endif (WINDOWS)
    SET(FMOD_LIBRARIES ${FMOD_LIBRARY})

    if (WINDOWS)
      set(FMOD_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/../fmodapi375win/api/inc)
    elseif (DARWIN)
      set(FMOD_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/../fmodapi375mac/api/inc)
    elseif (LINUX)
      set(FMOD_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/../fmodapi375linux/api/inc)
    endif (WINDOWS)

  endif (STANDALONE)
endif (FMOD)
