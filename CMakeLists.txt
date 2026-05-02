cmake_minimum_required(VERSION 3.20)
project(OpcodeAnalyzer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Charts)

set(CORE_SOURCES
    src/core/OpcodeTable.cpp
    src/core/Instruction.cpp
    src/core/ArithmeticInstruction.cpp
    src/core/MemoryInstruction.cpp
    src/core/ControlFlowInstruction.cpp
    src/core/LogicalInstruction.cpp
    src/core/IOInstruction.cpp
    src/core/InstructionFactory.cpp
    src/core/Program.cpp
    src/core/BinaryFile.cpp
    src/core/Disassembler.cpp
    # GenericInstruction.h is header-only; no .cpp needed
)

set(ANALYSIS_SOURCES
    src/analysis/FrequencyAnalyzer.cpp
    src/analysis/CategoryAnalyzer.cpp
    src/analysis/EntropyAnalyzer.cpp
    src/analysis/ComparisonAnalyzer.cpp
    src/analysis/Report.cpp
)

set(UI_SOURCES
    src/ui/ThemeManager.cpp
    src/ui/RecentFiles.cpp
    src/ui/SummaryView.cpp
    src/ui/DisassemblyView.cpp
    src/ui/ChartView.cpp
    src/ui/CompareView.cpp
    src/ui/MainWindow.cpp
)

qt_add_executable(OpcodeAnalyzer
    src/main.cpp
    ${CORE_SOURCES}
    ${ANALYSIS_SOURCES}
    ${UI_SOURCES}
    resources/resources.qrc
)

target_include_directories(OpcodeAnalyzer PRIVATE src)

target_link_libraries(OpcodeAnalyzer PRIVATE
    Qt6::Widgets
    Qt6::Charts
)

set_target_properties(OpcodeAnalyzer PROPERTIES
    WIN32_EXECUTABLE TRUE
)

# opcgen: örnek .opc dosyası üreten CLI aracı
add_executable(opcgen src/tools/opcgen.cpp)
target_include_directories(opcgen PRIVATE src)

# Windows: DLL'leri exe yanına kopyala
if(WIN32)
    find_program(WINDEPLOYQT windeployqt6
        HINTS "${Qt6_DIR}/../../../bin"
    )
    if(WINDEPLOYQT)
        add_custom_command(TARGET OpcodeAnalyzer POST_BUILD
            COMMAND ${WINDEPLOYQT}
                --no-translations
                --no-system-d3d-compiler
                "$<TARGET_FILE:OpcodeAnalyzer>"
            COMMENT "windeployqt6 çalıştırılıyor..."
        )
    endif()
endif()
