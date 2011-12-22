TARGET  = usb_drv
SRC_CC  = main.cc \
          input_component.cc \
          storage_component.cc
LIBS    = cxx env server signal dde_linux26_usbhid dde_linux26_usbstorage
