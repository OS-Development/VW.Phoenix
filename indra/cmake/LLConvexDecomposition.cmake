# -*- cmake -*-
include(Prebuilt)

set(LLCONVEXDECOMP_INCLUDE_DIRS ${LIBS_PREBUILT_DIR}/include)

if (INSTALL_PROPRIETARY AND NOT STANDALONE)
  use_prebuilt_binary(llconvexdecomposition)
  set(LLCONVEXDECOMP_LIBRARY llconvexdecomposition)
else (INSTALL_PROPRIETARY AND NOT STANDALONE)
  if (DARWIN)
    # For now use LLConvecDecompositionStub on MacOS until we have an HACD pre-built library for MacOS-X...
    use_prebuilt_binary(llconvexdecompositionstub)
    set(LLCONVEXDECOMP_LIBRARY llconvexdecompositionstub)
  else (DARWIN)
    include(HACD)
  endif (DARWIN)
endif (INSTALL_PROPRIETARY AND NOT STANDALONE)
