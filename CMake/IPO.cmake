set(CMAKE_POLICY_DEFAULT_CMP0069 "NEW")

if(${ENABLE_IPO})
  include(CheckIPOSupported)
  check_ipo_supported(RESULT IPOSupported OUTPUT error)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ${IPOSupported})
endif()