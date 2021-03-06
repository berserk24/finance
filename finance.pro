#-------------------------------------------------
#
# Project created by QtCreator 2014-01-19T13:50:03
#
#-------------------------------------------------

QT	+= core gui sql
QT	+= printsupport
QT	+= network
QT	+= gui-private

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = finance
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    general_window.cpp \
    class_rss_form.cpp \
    class_ref_firm.cpp \
    class_ref_client.cpp \
    class_ref_tarif.cpp \
    class_load_pp.cpp \
    class_pp.cpp \
    class_ref_pp.cpp \
    class_ref_balans_rs.cpp \
    class_ref_balans_client.cpp \
    class_ref_client_pay.cpp \
    update_client_balans.cpp \
    class_print_pp.cpp \
    class_manage_users.cpp \
    class_report_client.cpp \
    class_setings_table.cpp \
    class_settings.cpp \
    class_report_load_pp.cpp \
    class_ref_auto_actions.cpp \
    class_save_bik_ref.cpp \
    class_update_ref_banks.cpp \
    downloadmanager.cpp \
    class_create_pp.cpp

HEADERS  += widget.h \
    general_window.h \
    class_rss_form.h \
    class_ref_firm.h \
    class_ref_client.h \
    class_ref_tarif.h \
    class_load_pp.h \
    class_pp.h \
    class_ref_pp.h \
    class_ref_balans_rs.h \
    class_ref_balans_client.h \
    class_ref_client_pay.h \
    update_client_balans.h \
    class_print_pp.h \
    class_manage_users.h \
    class_report_client.h \
    class_setings_table.h \
    class_settings.h \
    class_report_load_pp.h \
    class_ref_auto_actions.h \
    class_save_bik_ref.h \
    class_update_ref_banks.h \
    downloadmanager.h \
    qdbftable.h \
    qdbfrecord.h \
    qdbf_global.h \
    class_create_pp.h

FORMS    += widget.ui \
    general_window.ui \
    class_rss_form.ui \
    class_ref_firm.ui \
    class_ref_client.ui \
    class_ref_tarif.ui \
    class_load_pp.ui \
    class_ref_pp.ui \
    class_ref_balans_rs.ui \
    class_ref_balans_client.ui \
    class_ref_client_pay.ui \
    class_ref_group.ui \
    class_manage_users.ui \
    class_balans_group_client.ui \
    class_report_client.ui \
    class_setings_table.ui \
    class_settings.ui \
    class_report_load_pp.ui \
    class_ref_auto_actions.ui \
    class_update_ref_banks.ui \
    class_create_pp.ui

RESOURCES += \
    language.qrc


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/release/ -lQDbf
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/debug/ -lQDbf
else:unix:!macx: LIBS += -L$$PWD/ -lQDbf

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/release/libQDbf.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/debug/libQDbf.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/release/QDbf.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/debug/QDbf.lib
else:unix:!macx: PRE_TARGETDEPS += $$PWD/libQDbf.a
