if (arena_resources)
    cmrc_add_resource_library(arena_resources ALIAS resource::ar NAMESPACE resource
            scripts/arena/actions/action.chai
            scripts/arena/actions/damage/damage.chai
            scripts/arena/actions/zone_heal/zone_heal.chai
            scripts/arena/actions/zone_damage/zone_damage.chai
            scripts/arena/actions/heal/heal.chai)
endif ()


macro(configure_files srcDir destDir)
    message(STATUS "Configuring directory ${destDir}")
    make_directory(${destDir})

    file(GLOB templateFiles RELATIVE ${srcDir} ${srcDir}/*)
    foreach (templateFile ${templateFiles})
        set(srcTemplatePath ${srcDir}/${templateFile})
        if (NOT IS_DIRECTORY ${srcTemplatePath})
            message(STATUS "Configuring file ${templateFile}")
            configure_file(${srcTemplatePath} ${destDir}/${templateFile} @ONLY)
        else ()
            configure_files(${srcTemplatePath} ${destDir}/${templateFile})
        endif ()
    endforeach (templateFile)
endmacro(configure_files)

configure_files(${CMAKE_CURRENT_SOURCE_DIR}/scripts/arena ${CMAKE_BINARY_DIR}/scripting_cache/scripts)
