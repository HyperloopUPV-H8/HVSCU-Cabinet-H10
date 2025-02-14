cmake_minimum_required (VERSION 3.14)

project (template-project ASM C CXX)
set(EXECUTABLE ${PROJECT_NAME}.elf)
set(STLIB_DIR ${CMAKE_CURRENT_LIST_DIR}/deps/ST-LIB)

option(USE_ETHERNET "Enable ethernet peripheral" OFF)
option(TARGET_NUCLEO "Targets the STM32H723 Nucleo development board" OFF)

if (PROJECT_IS_TOP_LEVEL AND (NOT CMAKE_CROSSCOMPILING))
  include(FetchContent)
  option(BUILD_GMOCK OFF)
  option(INSTALL_GTEST OFF)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
  )
  FetchContent_MakeAvailable(googletest)
  add_library(GTest::GTest INTERFACE IMPORTED)
  target_link_libraries(GTest::GTest INTERFACE gtest_main)
endif()

add_subdirectory(${STLIB_DIR})

set(HAL_HEADER 
  ${CMAKE_SOURCE_DIR}/Core/Inc/main.h
  ${CMAKE_SOURCE_DIR}/Core/Inc/stm32h7xx_hal_conf.h
  ${CMAKE_SOURCE_DIR}/Core/Inc/stm32h7xx_it.h
)
set(HAL_RUNES
  ${CMAKE_SOURCE_DIR}/Core/Src/Runes/Runes.cpp
)
set(HAL_SOURCE
  ${CMAKE_SOURCE_DIR}/Core/Src/stm32h7xx_hal_msp.c
  ${CMAKE_SOURCE_DIR}/Core/Src/stm32h7xx_it.c
  ${CMAKE_SOURCE_DIR}/Core/Src/syscalls.c
  ${CMAKE_SOURCE_DIR}/Core/Src/sysmem.c
  ${CMAKE_SOURCE_DIR}/Core/Src/system_stm32h7xx.c
)

set(HALAL_MOCK_RUNES
  ${CMAKE_SOURCE_DIR}/Core/Src/Runes/SimRunes.cpp
)

file(GLOB_RECURSE SOURCE_C ${CMAKE_SOURCE_DIR}/Core/*.c) 
list(REMOVE_ITEM SOURCE_C ${HAL_SOURCE})
file(GLOB_RECURSE SOURCE_CPP ${CMAKE_SOURCE_DIR}/Core/*.cpp) 
list(REMOVE_ITEM SOURCE_CPP ${HAL_RUNES})
list(REMOVE_ITEM SOURCE_CPP ${HALAL_MOCK_RUNES})
file(GLOB_RECURSE SOURCE_H ${CMAKE_SOURCE_DIR}/Core/*.h) 
list(REMOVE_ITEM SOURCE_H ${HAL_HEADER})
file(GLOB_RECURSE SOURCE_HPP ${CMAKE_SOURCE_DIR}/Core/*.hpp) 

add_executable(${EXECUTABLE}
$<$<BOOL:${CMAKE_CROSSCOMPILING}>:${HAL_HEADER}>
${SOURCE_H}
$<$<BOOL:${CMAKE_CROSSCOMPILING}>:${HAL_SOURCE}>
${SOURCE_C}
${SOURCE_HPP}
$<$<BOOL:${CMAKE_CROSSCOMPILING}>:${CMAKE_SOURCE_DIR}/Core/Startup/startup_stm32h723zgtx.s>
${CMAKE_SOURCE_DIR}/Core/Src/Runes/Pins.cpp
$<IF:$<BOOL:${CMAKE_CROSSCOMPILING}>,${HAL_RUNES},${HALAL_MOCK_RUNES}>
${SOURCE_CPP}
)

target_link_libraries(${EXECUTABLE} PRIVATE 
  st-lib
  $<$<NOT:$<BOOL:${CMAKE_CROSSCOMPILING}>>:GTest::gtest_main>
)

set_target_properties(${EXECUTABLE} PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED C_STANDARD 11 C_STANDARD_REQUIRED)

target_compile_definitions(${EXECUTABLE} PRIVATE
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:USE_HAL_DRIVER>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:STM32H723xx>

  $<$<NOT:$<BOOL:${CMAKE_CROSSCOMPILING}>>:SIM_ON>

  $<$<BOOL:${USE_ETHERNET}>:STLIB_ETH>
  $<IF:$<BOOL:${TARGET_NUCLEO}>,NUCLEO,BOARD>
  $<IF:$<BOOL:${TARGET_NUCLEO}>,HSE_VALUE=8000000,HSE_VALUE=25000000>
)

target_compile_options(${EXECUTABLE} PRIVATE
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-mcpu=cortex-m7>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-mfpu=fpv5-d16>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-mfloat-abi=hard>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-mthumb>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-specs=nosys.specs>

  -ffunction-sections
  -fdata-sections
  -fno-exceptions

  -Wno-psabi

  $<$<COMPILE_LANGUAGE:C>:-w>

  $<$<COMPILE_LANGUAGE:CXX>:-Wall>
  $<$<COMPILE_LANGUAGE:CXX>:-Wpedantic>
  $<$<COMPILE_LANGUAGE:CXX>:-Werror>
  $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>
  $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
)

# REMEMBER TO CHANGE `.vscode/C_cpp_properties.json` TO MATCH THIS LIST
target_include_directories(${EXECUTABLE} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Drivers/CMSIS/Device/ST/STM32H7xx/Include>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Drivers/CMSIS/Include>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/system>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/netif/ppp>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/lwip>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/lwip/apps>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/lwip/priv>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/lwip/prot>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/netif>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/compat>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/compat/posix>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/compat/posix/net>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/compat/posix/sys>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/src/include/compat/stdc>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Middlewares/Third_Party/LwIP/system/arch>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Drivers/BSP/Components>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/Drivers/BSP/Components/lan8742>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/LWIP/App>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:${STLIB_DIR}/LWIP/Target>
  ${STLIB_DIR}/Inc
  ${STLIB_DIR}/Inc/ST-LIB_LOW
  ${STLIB_DIR}/Inc/ST-LIB_HIGH
)

target_link_options(${EXECUTABLE} PRIVATE 
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-T${CMAKE_SOURCE_DIR}/STM32H723ZGTX_FLASH.ld>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-mcpu=cortex-m7>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-mthumb>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-mfpu=fpv5-d16>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-mfloat-abi=hard>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-specs=nosys.specs>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-lc>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-lm>                
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-lnosys>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-Wl,-Map=${PROJECT_NAME}.map,--cref>
  $<$<BOOL:${CMAKE_CROSSCOMPILING}>:-Wl,--gc-sections>
)

if (PROJECT_IS_TOP_LEVEL)
    # Create symlink to compile_commands.json for IDE to pick it up
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
    )
    # Create symlink to executable for IDE to pick it up
    add_custom_command(
        TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_BINARY_DIR}/${EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/out/build/latest.elf
    )
endif()
