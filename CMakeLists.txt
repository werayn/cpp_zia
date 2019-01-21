cmake_minimum_required(VERSION 3.0)

if (WIN32)
# Notre projet zia
project(zia)

# Fichier � compiler
set(SRCS
    windows/main.cpp
    )

# Notre ex�cutable
add_executable(zia ${SRCS})

endif (WIN32)

if (UNIX)

add_custom_target(zia
                  COMMAND make
                  WORKING_DIRECTORY linux/
  )

add_custom_target(clean_zia
                  COMMAND make clean
                  WORKING_DIRECTORY linux/
  )

add_custom_target(fclean_zia
                  COMMAND make fclean
                  WORKING_DIRECTORY linux/
  )

add_custom_target(re_zia
                  COMMAND make re
                  WORKING_DIRECTORY linux/
  )

add_custom_target(test_zia
                  COMMAND make test
                  WORKING_DIRECTORY linux/
)

endif(UNIX)
