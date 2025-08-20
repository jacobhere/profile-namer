zephyr_library()

# Add ZMK's public headers (zmk/app/include) to this library's include path.
# APPLICATION_SOURCE_DIR points to .../zmk/app during the ZMK build.
target_include_directories(${ZEPHYR_CURRENT_LIBRARY} PRIVATE
  ${APPLICATION_SOURCE_DIR}/include
)

zephyr_library_sources(src/profile_namer.c)