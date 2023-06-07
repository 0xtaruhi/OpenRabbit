add_rules("mode.debug", "mode.release")
set_languages("c++17")

add_subdirs("rabbit_App/3rdparty/TabToolbar")
if is_plat("windows") then
    add_cxxflags("/W3", "/utf-8", "/Zc:__cplusplus", "/EHsc")
    add_cxxflags("/D_CRT_SECURE_NO_WARNINGS")
else
    add_cxxflags("-Wall", "-Werror")
end

target("rabbit_App")
    add_rules("qt.application")
    set_kind("binary")
    add_defines("RABBIT_APP")
    add_files("rabbit_App/src/**.cpp")
    add_files("rabbit_App/include/**.h")
    add_files("rabbit_App/res/*.qrc")
    add_files("rabbit_App/3rdparty/TabToolbar/src/TabToolbar/*.qrc")
    add_frameworks("QtWidgets", "QtGui", "QtCore")
    add_includedirs("rabbit_App/include")
    add_includedirs("VLFDLibUSBDriver")
    add_deps("TabToolbar")
    add_links("VLFDLibUSB", "usb-1.0")
    add_linkdirs("VLFDLibUSBDriver", "libusb/libusb/.libs")
