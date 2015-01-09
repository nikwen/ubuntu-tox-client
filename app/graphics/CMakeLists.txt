file(GLOB UI_QML_JS_FILES *.png)

# Make the files visible in the qtcreator tree
add_custom_target(ubuntu-tox-client_ui_pngFiles ALL SOURCES ${UI_QML_JS_FILES})

install(FILES ${UI_QML_JS_FILES} DESTINATION ${UBUNTU-TOX-CLIENT_DIR}/graphics)

