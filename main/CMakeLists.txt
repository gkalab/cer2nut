idf_component_register(SRCS "vcpusb.cpp" "bleuart.cpp" "cer2nut.cpp" "cp210x_usb.cpp" 
                    "adapter/lib/CalibrationSquare.cpp" 
                    "adapter/lib/CertaboBoardMessageParser.cpp"
                    "adapter/lib/CertaboCalibrator.cpp"
                    "adapter/lib/CertaboParser.cpp"
                    "adapter/lib/CertaboPiece.cpp"
                    "adapter/lib/CertaboLedControl.cpp"
                    "adapter/lib/ChessnutAdapter.cpp"
                    "adapter/lib/ChessnutConverter.cpp"
                    "adapter/lib/Sentio.cpp"
                    "adapter/lib/Chess0x88.cpp"
                    INCLUDE_DIRS ".")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
