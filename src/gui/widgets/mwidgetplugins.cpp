#include <QtPlugin>

#include "mwidgetplugins.h"
#include "msurface.h"
#include "mbutton.h"
#include "mdial.h"
#include "mslider.h"


/**
 * @short Export MSurface to be used by designer
 */
MWidgetPlugin::MWidgetPlugin(const char *name, QObject *parent)
        : QObject(parent) {
    setObjectName(name);
    initialized = false;
}

void MWidgetPlugin::initialize(QDesignerFormEditorInterface * /* core */) {
    if (initialized)
        return;

    initialized = true;
}

bool MWidgetPlugin::isInitialized() const {
    return initialized;
}

QWidget *MWidgetPlugin::createWidget(QWidget *parent) {
    if (name()=="MSurface")
        return new MSurface(parent);
    else if (name()=="MButton")
        return new MButton(0,0,parent);
    else if (name()=="MDial")
        return new MDial(0,parent);
    else if (name()=="MSlider")
        return new MSlider(0,Qt::Vertical, parent);
    return NULL;
}

QString MWidgetPlugin::group() const {
    return "Monkey Synth";
}

QIcon MWidgetPlugin::icon() const {
    return QIcon();
}

QString MWidgetPlugin::toolTip() const {
    return "";
}

QString MWidgetPlugin::whatsThis() const {
    return "";
}

bool MWidgetPlugin::isContainer() const {
    return name()=="MSurface";
}
QString MWidgetPlugin::name() const {
    return objectName();
}

QString MWidgetPlugin::domXml() const {
    return QString(
               "<ui language=\"c++\">\n"
               " <widget class=\"%1\" name=\"%2\">\n"
               "  <property name=\"geometry\">\n"
               "   <rect>\n"
               "    <x>0</x>\n"
               "    <y>0</y>\n"
               "    <width>100</width>\n"
               "    <height>100</height>\n"
               "   </rect>\n"
               "  </property>\n"
               " </widget>\n"
               "</ui>").arg(name()).arg(name().toLower());
}

QString MWidgetPlugin::includeFile() const {
    return QString("%1.h").arg(name()).toLower();
}


/**
 * @short Export the collection of widgets.
 */
MWidgetPlugins::MWidgetPlugins(QObject *parent)
        : QObject(parent) {
    widgets.append(new MWidgetPlugin("MSurface",this));
    widgets.append(new MWidgetPlugin("MButton",this));
    widgets.append(new MWidgetPlugin("MDial",this));
    widgets.append(new MWidgetPlugin("MSlider",this));
}

QList<QDesignerCustomWidgetInterface*> MWidgetPlugins::customWidgets() const {
    return widgets;
}

Q_EXPORT_PLUGIN2(msynthwidgets, MWidgetPlugins)

