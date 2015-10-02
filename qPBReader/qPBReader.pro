#
# qPBReader project
#

QT               = core gui webkit xml xmlpatterns

RESOURCES        = res/qPBReader.qrc

LIBS            += \
                   ../miniunz/lib/libminiunz.a \
                   -lxml2 \
                   ../libxslt/$$(XSLT_BUILD)/lib/libxslt.a \

HEADERS          = \
                   include/qPBReader.h \
                   include/qPBReaderComboBox.h \
                   include/qPBReaderConfig.h \
                   include/qPBReaderConfigDialog.h \
                   include/qPBReaderContextMenu.h \
                   include/qPBReaderDialog.h \
                   include/qPBReaderDictionaryDialog.h \
                   include/qPBReaderDialogHeader.h \
                   include/qPBReaderDoc.h \
                   include/qPBReaderDocView.h \
                   include/qPBReaderEpub.h \
                   include/qPBReaderEpubOpf.h \
                   include/qPBReaderEpubSettings.h \
                   include/qPBReaderFontSelectorWidget.h \
                   include/qPBReaderFontSettingsDialog.h \
                   include/qPBReaderGoBackBtn.h \
                   include/qPBReaderGoBackDlg.h \
                   include/qPBReaderJSLoader.h \
                   include/qPBReaderLangLists.h \
                   include/qPBReaderPageCounter.h \
                   include/qPBReaderPagePos.h \
                   include/qPBReaderPlatform.h \
                   include/qPBReaderSeparatorWidget.h \
                   include/qPBReaderSettingsDialog.h \
                   include/qPBReaderSetValueWidget.h \
                   include/qPBReaderSetValueWidgetWithLabel.h \
                   include/qPBReaderTextLayoutSettingsDialog.h \
                   include/qPBReaderTOCGenerator.h \
                   include/qPBReaderTracer.h \
                   include/qPBReaderTypeSelectorWidget.h \
                   include/qPBReaderUnzip.h \
                   include/qPBReaderUtils.h \
                   include/qPBReaderVerticalWidget.h \
                   include/qPBReaderViewer.h \
                   include/qPBReaderZoomWidget.h \
                   include/qPBReaderZoomWidgetWithLabel.h

SOURCES          = \
                   src/qPBReader.cpp \
                   src/qPBReaderComboBox.cpp \
                   src/qPBReaderConfig.cpp \
                   src/qPBReaderConfigDialog.cpp \
                   src/qPBReaderContextMenu.cpp \
                   src/qPBReaderDialog.cpp \
                   src/qPBReaderDictionaryDialog.cpp \
                   src/qPBReaderDialogHeader.cpp \
                   src/qPBReaderDoc.cpp \
                   src/qPBReaderDocView.cpp \
                   src/qPBReaderEpub.cpp \
                   src/qPBReaderEpubOpf.cpp \
                   src/qPBReaderEpubSettings.cpp \
                   src/qPBReaderFontSelectorWidget.cpp \
                   src/qPBReaderFontSettingsDialog.cpp \
                   src/qPBReaderGoBackBtn.cpp \
                   src/qPBReaderGoBackDlg.cpp \
                   src/qPBReaderJSLoader.cpp \
                   src/qPBReaderLangLists.cpp \
                   src/qPBReaderPageCounter.cpp \
                   src/qPBReaderPagePos.cpp \
                   src/qPBReaderPlatform.cpp \
                   src/qPBReaderSeparatorWidget.cpp \
                   src/qPBReaderSettingsDialog.cpp \
                   src/qPBReaderSetValueWidget.cpp \
                   src/qPBReaderSetValueWidgetWithLabel.cpp \
                   src/qPBReaderTextLayoutSettingsDialog.cpp \
                   src/qPBReaderTOCGenerator.cpp \
                   src/qPBReaderTracer.cpp \
                   src/qPBReaderTypeSelectorWidget.cpp \
                   src/qPBReaderUnzip.cpp \
                   src/qPBReaderUtils.cpp \
                   src/qPBReaderVerticalWidget.cpp \
                   src/qPBReaderViewer.cpp \
                   src/qPBReaderZoomWidget.cpp \
                   src/qPBReaderZoomWidgetWithLabel.cpp

INCLUDEPATH     += include \
                   ../miniunz/include \
                   ../libxslt/$$(XSLT_BUILD)/include \
                   $$(SYSROOT)/usr/include/libxml2

DEPENDPATH      += include

PRE_TARGETDEPS  += ../miniunz/lib/libminiunz.a

OBJECTS_DIR      = obj
MOC_DIR          = obj/generated_src
RCC_DIR          = obj/generated_rcc
UI_SOURCES_DIR   = obj/generated_src
UI_HEADERS_DIR   = obj/generated_h

QMAKE_DISTCLEAN += -r obj

#CONFIG          += debug

TRANSLATIONS     = translations/qPBReader_fr.ts

TARGET           = ../bin/qPBReader
