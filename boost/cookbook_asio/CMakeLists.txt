
file(GLOB files *.cpp)

foreach(f ${files})
    get_filename_component(filename ${f} NAME_WE)
    add_executable("${filename}" ${f})
    target_link_libraries("${filename}" ${Boost_LIBRARIES})
endforeach()

