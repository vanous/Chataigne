/*
  ==============================================================================

    MultiplexList.cpp
    Created: 19 Dec 2020 12:00:35pm
    Author:  bkupe

  ==============================================================================
*/

#include "MultiplexList.h"
#include "Module/ModuleManager.h"
#include "ui/MultiplexListEditor.h"

BaseMultiplexList::BaseMultiplexList(const String& name, var params) :
    BaseItem(name, false),
    listSize(0)
{
    showInspectorOnSelect = false;
    isSelectable = false;
}

BaseMultiplexList::~BaseMultiplexList()
{
}

void BaseMultiplexList::setSize(int size)
{
    if (size == listSize) return;
    listSize = size;
    updateControllablesSetup();
}

void BaseMultiplexList::updateControllablesSetup()
{
    while (list.size() > listSize)
    {
        Controllable* c = list[list.size() - 1];
        list.removeAllInstancesOf(c);
        removeControllable(c);
    }

    while (list.size() < listSize)
    {
        Controllable* c = ControllableFactory::createControllable(getTypeString());
        c->isCustomizableByUser = true;
        c->setNiceName("#" + String(list.size() + 1));
        list.add(c);
        addControllable(c);
    }
}

var BaseMultiplexList::getJSONData()
{
    var data = BaseItem::getJSONData();
    data.getDynamicObject()->setProperty("listSize", listSize);
    return data;
}

void BaseMultiplexList::loadJSONData(var data, bool createIfNotThere)
{
    setSize(data.getProperty("listSize", 0));
    loadJSONDataMultiplexInternal(data);
    BaseItem::loadJSONData(data, createIfNotThere);
}

void BaseMultiplexList::notifyItemUpdated(int multiplexIndex)
{
    jassert(multiplexIndex >= 0);
    listListeners.call(&ListListener::listItemUpdated, multiplexIndex);
}


InputValueMultiplexList::InputValueMultiplexList(var params) :
    BaseMultiplexList(getTypeString(), params)
{
}

InputValueMultiplexList::~InputValueMultiplexList()
{
    for (auto& c : inputControllables)
    {
        if (c == nullptr || c.wasObjectDeleted()) continue;
        if (c->type == c->TRIGGER) ((Trigger*)c.get())->removeTriggerListener(this);
        else((Parameter*)c.get())->removeParameterListener(this);
    }
}

void InputValueMultiplexList::updateControllablesSetup()
{
    while (list.size() > listSize)
    {
        int index = list.size() - 1;
        Controllable* c = list[index];
        list.removeAllInstancesOf(c);
        removeControllable(c);
        if (Controllable * c = inputControllables[index])
        {
            if (c->type == c->TRIGGER) ((Trigger*)c)->removeTriggerListener(this);
            else((Parameter*)c)->removeParameterListener(this);
        }
    }

    while (list.size() < listSize)
    {
        TargetParameter* tp = addTargetParameter("#" + String(list.size() + 1), "Input Value Target");
        tp->customGetTargetFunc = &ModuleManager::showAllValuesAndGetControllable;
        tp->customGetControllableLabelFunc = &Module::getTargetLabelForValueControllable;
        tp->customCheckAssignOnNextChangeFunc = &ModuleManager::checkControllableIsAValue;

        list.add(tp);
        inputControllables.add(nullptr);
    }
}

void InputValueMultiplexList::onContainerParameterChangedInternal(Parameter* p)
{
    int index = list.indexOf(p);
    if (index != -1)
    {
        if (Controllable* c = inputControllables[index])
        {
            if (c->type == c->TRIGGER) ((Trigger*)c)->removeTriggerListener(this);
            else((Parameter*)c)->removeParameterListener(this);
        }

        inputControllables.set(index, nullptr);

        if (Controllable* c = ((TargetParameter*)p)->target)
        {
            if (c->type == c->TRIGGER) ((Trigger*)c)->addTriggerListener(this);
            else((Parameter*)c)->addParameterListener(this);
            inputControllables.set(index, c);

            if(index == 0) listListeners.call(&ListListener::listReferenceUpdated);
            notifyItemUpdated(index);
        }
    }
}

void InputValueMultiplexList::onExternalParameterRangeChanged(Parameter* p)
{
    if (inputControllables.indexOf(p) == 0) listListeners.call(&ListListener::listReferenceUpdated);
}

void InputValueMultiplexList::onExternalParameterValueChanged(Parameter* p)
{
    notifyItemUpdated(inputControllables.indexOf(p));
}

void InputValueMultiplexList::onExternalTriggerTriggered(Trigger* t)
{
    notifyItemUpdated(inputControllables.indexOf(t));
}

EnumMultiplexList::EnumMultiplexList(var params) :
    MultiplexList(EnumParameter::getTypeStringStatic() + " List")
{
}

EnumMultiplexList::~EnumMultiplexList()
{
}


void EnumMultiplexList::addOption(const String& key, const String& value)
{
   // if (referenceOptions.contains(e)) return;
    referenceOptions.add(new EnumParameter::EnumValue(key, value));

    for (auto& c : list)
    {
        EnumParameter* ep = (EnumParameter*)c;
        ep->addOption(key, value);
    }
}

void EnumMultiplexList::updateOption(int index, const String& key, const String& value)
{
    if (index >= referenceOptions.size())
    {
        for (int i = 0; i < referenceOptions.size()-1; i++) addOption("#" + String(i + 1), "[notset]");
        addOption(key, value);
        return;
    }

   referenceOptions.set(index, new EnumParameter::EnumValue(key, value));
   for (auto& c : list)
   {
       EnumParameter* ep = (EnumParameter*)c;
       ep->updateOption(index, key, value);
   }
}

void EnumMultiplexList::removeOption(const String& key)
{
    //if (!referenceOptions.contains(key)) return;
    for (int i = 0; i < referenceOptions.size(); i++)
    {
        if (referenceOptions[i]->key == key)
        {
            referenceOptions.remove(i);
            break;
        }
    }

    for (auto& c : list)
    {
        EnumParameter* ep = (EnumParameter*)c;
        ep->removeOption(key);
    }
}

void EnumMultiplexList::controllableAdded(Controllable* c)
{
    if (EnumParameter* ep = dynamic_cast<EnumParameter*>(c))
    {
        for(auto & ev : referenceOptions) ep->addOption(ev->key, ev->value);
    }

    MultiplexList::controllableAdded(c);
}

var EnumMultiplexList::getJSONData()
{
    var data = MultiplexList::getJSONData();
    var enumOptions(new DynamicObject());
    for(auto & ev : referenceOptions) enumOptions.getDynamicObject()->setProperty(ev->key, ev->value);
    data.getDynamicObject()->setProperty("enumOptions", enumOptions);
    return data;
}

void EnumMultiplexList::loadJSONDataMultiplexInternal(var data)
{
    if (data.hasProperty("enumOptions"))
    {
       NamedValueSet optionsData = data.getProperty("enumOptions", var()).getDynamicObject()->getProperties();
       for(auto & op : optionsData) addOption(op.name.toString(), op.value.toString());
    }
}

InspectableEditor* EnumMultiplexList::getEditor(bool isRoot)
{
    return new EnumMultiplexListEditor(this, isRoot);
}

