find_program(
  FORMAT_CODE_SCRIPT
  run_clang_format.sh
  HINTS ../scripts/
  NO_DEFAULT_PATH
  DOC "Path to format-code.sh"
)

add_custom_target(format-check
  COMMENT "Check code format via ../scripts/run_clang_format.sh ${CMAKE_CURRENT_SOURCE_DIR}/.."
  COMMAND ${FORMAT_CODE_SCRIPT} ${CMAKE_CURRENT_SOURCE_DIR}/.. )
add_custom_target(format
  COMMENT "Change code format via ${FORMAT_CODE_SCRIPT} ${CMAKE_CURRENT_SOURCE_DIR}/.."
  COMMAND ${FORMAT_CODE_SCRIPT} ${CMAKE_CURRENT_SOURCE_DIR}/.. -i)
